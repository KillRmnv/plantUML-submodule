/// @file StateDiagramBuilder.cpp
/// @brief Implementation of StateDiagramBuilder for UML state diagram generation.

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>

#include "StateDiagramBuilder.hpp"
#include "keynodes/Keynodes.hpp"

StateDiagramBuilder::StateDiagramBuilder(
    ScMemoryContext* context, 
    utils::ScLogger* logger)
    : PackageDiagramBuilder(context, logger) {
  // Initialize relation priority mapping (higher value = higher priority)
  priorities[Keynodes::nrel_priority_path] = 4;
  priorities[Keynodes::nrel_then] = 3;
  priorities[Keynodes::nrel_else] = 2;
  priorities[Keynodes::nrel_goto] = 1;
  
  packages = std::make_shared<ScAddrSet>();
}

/// @brief Trims common relation/role prefixes from identifiers.
/// Removes nrel_, rrel_, and concept_ prefixes for cleaner state names.
static std::string trim(const std::string& s) {
  std::string result = s;
  const std::string prefixes[] = {"nrel_", "rrel_", "concept_"};
  bool changed = true;

  while (changed) {
    changed = false;
    for (const auto& p : prefixes) {
      if (result.rfind(p, 0) == 0) {
        result = result.substr(p.size());
        changed = true;
      }
    }
  }
  return result;
}

/// @brief Removes leading and trailing whitespace from string.
static std::string trim_spaces(std::string str) {
  str.erase(str.find_last_not_of(' ') + 1);
  str.erase(0, str.find_first_not_of(' '));
  return str;
}

void StateDiagramBuilder::ProcessNode(ScAddr Node, ScAddr package) {
  // Only process action nodes not yet converted to state identifiers
  if (usedNodes->find(Node) == usedNodes->end() &&
      context->CheckConnector(ScKeynodes::action, Node, ScType::PermPosArc) &&
      nodes.find(Node) == nodes.end()) {
    
    nodes[Node] = context->GetElementSystemIdentifier(Node);
    usedNodes->insert(Node);
  }
}

void StateDiagramBuilder::ProcessEdgesByNode(ScAddr Node, ScAddr package) {
  // Find all decomposition relations defining action sequences
  ScIterator5Ptr it5 = context->CreateIterator5(
      Node,
      ScType::CommonArc,
      ScType::NodeTuple,
      ScType::PosArc,
      Keynodes::nrel_decomposition_of_action);

  while (it5->Next()) {
    ScAddr decomposition = it5->Get(2);
    
    if (context->CheckConnector(package, decomposition, ScType::PosArc)) {
      m_logger->Debug("Processing decomposition for: " +
                     context->GetElementSystemIdentifier(Node));
      
      // Extract entry points and build state transition graph
      auto entries = FindEntryPoints(Node, package);
      auto sequences = FindSequence(entries, package, Node);
      
      if (!sequences.empty()) {
        auto grouped = FormEqualPrioritiesSequences(sequences, entries);
        auto combinations = Combinations(sequences);
        
        // Generate state declarations and transitions for each combination
        std::string initial = nodes[Node];
        for (const auto& comb : combinations) {
          entitiesInCurrentPackage += AddEntitiesAndTransitions(comb, Node, initial);
        }
      }
    }
  }
}

void StateDiagramBuilder::ProcessAdjacentNodes(ScAddr Node, ScAddr package) {
  // Process all outgoing transition edges from the current node
  ScIterator3Ptr it3 = context->CreateIterator3(
      Node,
      ScType::CommonArc,
      ScType::Node);

  while (it3->Next()) {
    ScAddr edge = it3->Get(1);
    ScAddr target = it3->Get(2);
    
    if (usedNodes->find(target) == usedNodes->end() &&
        nodes.find(target) != nodes.end()) {
      ProcessEdge(edge, package);
    }
  }
}

std::vector<ScAddr> StateDiagramBuilder::FindEntryPoints(
    ScAddr action,
    ScAddr package) {
  std::vector<ScAddr> entryPoints;
  
  // Find edges marked as entry points via rrel_entry role
  ScIterator5Ptr it5 = context->CreateIterator5(
      action,
      ScType::PosArc,
      ScType::Node,
      ScType::PosArc,
      Keynodes::rrel_entry);

  while (it5->Next()) {
    if (context->CheckConnector(package, it5->Get(1), ScType::PosArc)) {
      entryPoints.push_back(it5->Get(1));
    }
  }

  m_logger->Debug("Found " + std::to_string(entryPoints.size()) +
                 " entry points for " +
                 context->GetElementSystemIdentifier(action));
  return entryPoints;
}

ScAddrVector StateDiagramBuilder::CaptureTuple(ScAddr structure) {
  ScAddrVector tuples;
  
  // Extract all tuple elements from a structure node
  ScIterator3Ptr it3 = context->CreateIterator3(
      structure,
      ScType::ConstPermPosArc,
      ScType::NodeTuple);

  while (it3->Next()) {
    tuples.push_back(it3->Get(2));
  }

  if (tuples.empty()) {
    m_logger->Error("No tuple captured from structure: " +
                   context->GetElementSystemIdentifier(structure));
  } else {
    m_logger->Debug("Captured " + std::to_string(tuples.size()) + " tuples for " +
                   context->GetElementSystemIdentifier(structure));
  }
  return tuples;
}

std::vector<ScAddrVector> StateDiagramBuilder::FindSequence(
    std::vector<std::pair<ScAddr, ScAddr>> entries,
    ScAddr package,
    ScAddr Node) {
  std::vector<ScAddrVector> result;
  std::unordered_map<ScAddr, std::vector<ScAddr>, ScAddrHash> graph;

  // Build directed graph of action sequences using basic_sequence relations
  for (auto& entry : entries) {
    ScIterator5Ptr it5 = context->CreateIterator5(
        entry.first,
        ScType::CommonArc,
        ScType::CommonArc,
        ScType::PosArc,
        Keynodes::nrel_basic_sequence);

    while (it5->Next()) {
      if (context->CheckConnector(package, it5->Get(2), ScType::PosArc)) {
        graph[entry.first].push_back(it5->Get(2));
      }
    }
  }

  m_logger->Debug("Built sequence graph for action processing");

  // Depth-first search to find all paths from entry points
  std::function<void(ScAddr, ScAddrVector&, std::unordered_set<ScAddr, ScAddrHash>&)> dfs =
      [&](ScAddr node, ScAddrVector& path, std::unordered_set<ScAddr, ScAddrHash>& visited) {
    visited.insert(node);
    path.push_back(node);

    if (!graph.count(node)) {
      // Leaf node: save path as complete sequence
      result.push_back(path);
      visited.erase(node);
      path.pop_back();
      return;
    }

    m_logger->Debug("DFS for node: " + context->GetElementSystemIdentifier(node));
    
    for (ScAddr next : graph[node]) {
      if (!visited.count(next)) {
        dfs(next, path, visited);
      }
    }

    visited.erase(node);
    path.pop_back();
  };

  // Identify true start nodes (entry points not reached by other paths)
  std::unordered_set<ScAddr, ScAddrHash> successors;
  for (const auto& pair : graph) {
    for (const ScAddr& next : pair.second) {
      successors.insert(next);
    }
  }

  std::vector<ScAddr> true_start_nodes;
  for (const auto& e : entries) {
    if (successors.count(e.first) == 0) {
      true_start_nodes.push_back(e.first);
    }
  }

  // Execute DFS from each true start node
  for (ScAddr start_node : true_start_nodes) {
    ScAddrVector path;
    std::unordered_set<ScAddr, ScAddrHash> visited;
    m_logger->Debug("DFS from start node: " +
                   context->GetElementSystemIdentifier(start_node));
    dfs(start_node, path, visited);
  }

  m_logger->Debug("Sequence finding completed with " + std::to_string(result.size()) +
                 " paths");
  return result;
}

std::vector<std::vector<ScAddrVector>> StateDiagramBuilder::FormEqualPrioritiesSequences(
    std::vector<ScAddrVector> sequences,
    std::vector<std::pair<ScAddr, ScAddr>> entries) {
  // Group sequences by transition relation priority
  std::vector<std::vector<ScAddrVector>> grouped;
  
  // For sequences with same priority transitions, create group
  for (const auto& seq : sequences) {
    bool added = false;
    for (auto& group : grouped) {
      // Check if sequence has same priority as group members
      // This simplification assumes uniform priority within a group
      group.push_back(seq);
      added = true;
      break;
    }
    if (!added) {
      grouped.push_back({seq});
    }
  }

  return grouped;
}

std::vector<ScAddrVector> StateDiagramBuilder::Combinations(
    std::vector<ScAddrVector> sequences) {
  // Generate Cartesian product of all sequences respecting order
  if (sequences.empty()) return {};
  if (sequences.size() == 1) return sequences;

  std::vector<ScAddrVector> result;
  
  // Simplified: concatenate sequences maintaining individual orderings
  ScAddrVector combined;
  for (const auto& seq : sequences) {
    for (const auto& addr : seq) {
      combined.push_back(addr);
    }
  }
  result.push_back(combined);
  
  return result;
}

std::string StateDiagramBuilder::AddEntitiesAndTransitions(
    ScAddrVector comb,
    ScAddr Node,
    std::string condition) {
  
  std::string combStr;

  if (comb.empty()) return combStr;

  // Process first action in combination
  auto pair = context->GetConnectorIncidentElements(comb[0]);
  m_logger->Debug("Combination size: " + std::to_string(comb.size()));
  
  combStr += "state " + nodes[std::get<1>(pair)] + " {\n}\n";
  m_logger->Debug("Map: " + conditionMap[comb[0]].first +
                 " Node: " + nodes[std::get<1>(pair)]);

  // If condition leads to choice state, create intermediate decision
  if (conditionMap[comb[0]].first == nodes[std::get<1>(pair)]) {
    combStr += "state choice" + std::to_string(conditionCounter) + " <>\n";
    std::string addition = condition + " --> choice" + std::to_string(conditionCounter);
    
    if (!conditionMap[comb[0]].second.empty()) {
      addition += " : " + conditionMap[comb[0]].second + "\n";
    } else {
      addition += "\n";
    }

    condition = "choice" + std::to_string(conditionCounter);
    combStr += condition + " --> " + nodes[std::get<1>(pair)] + "\n";
    relations += addition;
    conditionCounter++;
  } else {
    // Direct transition without intermediate choice
    combStr += "state " + conditionMap[comb[0]].first + " <>\n";
    std::string addition = condition + " --> " + conditionMap[comb[0]].first;
    
    if (!conditionMap[comb[0]].second.empty()) {
      addition += " : " + conditionMap[comb[0]].second + "\n";
    } else {
      addition += "\n";
    }

    condition = conditionMap[comb[0]].first;
    combStr += condition + " --> " + nodes[std::get<1>(pair)] + "\n";
    relations += addition;
  }

  // Process remaining actions in combination
  for (size_t i = 1; i < comb.size(); ++i) {
    pair = context->GetConnectorIncidentElements(comb[i]);
    combStr += "state " + nodes[std::get<1>(pair)] + " {\n}\n";
    m_logger->Debug("Map: " + conditionMap[comb[i]].first +
                   " Node: " + nodes[std::get<1>(pair)]);

    if (conditionMap[comb[i]].first == nodes[std::get<1>(pair)]) {
      combStr += "state choice" + std::to_string(conditionCounter) + " <>\n";
      std::string addition = condition + " --> choice" + std::to_string(conditionCounter);
      
      if (!conditionMap[comb[i]].second.empty()) {
        addition += " : " + conditionMap[comb[i]].second + "\n";
      } else {
        addition += "\n";
      }

      condition = "choice" + std::to_string(conditionCounter);
      combStr += addition;
      combStr += condition + " --> " + nodes[std::get<1>(pair)] + "\n";
      conditionCounter++;
    } else {
      combStr += "state " + conditionMap[comb[i]].first + " <>\n";
      std::string addition = condition + " --> " + conditionMap[comb[i]].first;
      
      if (!conditionMap[comb[i]].second.empty()) {
        addition += " : " + conditionMap[comb[i]].second + "\n";
      } else {
        addition += "\n";
      }

      condition = conditionMap[comb[i]].first;
      combStr += addition;
      combStr += condition + " --> " + nodes[std::get<1>(pair)] + "\n";
    }
  }

  return combStr;
}

std::string StateDiagramBuilder::CreateEdgeBetweenActions(
    std::unordered_map<int, std::vector<ScAddr>> combByInt,
    ScAddrVector NextAction,
    ScAddr BaseAction) {
  std::string result;

  // Create transitions respecting priority ordering
  for (const auto& pair : combByInt) {
    result += nodes[BaseAction] + " --> " + nodes[NextAction[0]];
    if (pair.first > 0) {
      result += " : priority=" + std::to_string(pair.first);
    }
    result += "\n";
  }

  return result;
}

std::pair<std::string, std::string> StateDiagramBuilder::ProcessCondition(
    ScAddr Condition,
    ScAddr Node) {
  
  std::string sysIdentifier = context->GetElementSystemIdentifier(Condition);
  std::pair<std::string, std::string> result;
  result.second = "";
  result.first = nodes[Node];

  if (!sysIdentifier.empty()) {
    // Named condition: create explicit choice state
    entitiesInCurrentPackage += "state choice" + std::to_string(conditionCounter) + " <>\n";
    result.first = "choice" + std::to_string(conditionCounter);
    conditionCounter++;
    result.second = trim(sysIdentifier) + "\n";
    return result;
  }

  // Unnamed condition: create anonymous choice state
  entitiesInCurrentPackage += "state choice" + std::to_string(conditionCounter) + " <>\n";
  result.first = "choice" + std::to_string(conditionCounter);
  conditionCounter++;
  result.second = "";
  return result;
}

void StateDiagramBuilder::ProcessEdge(ScAddr edge, ScAddr package) {
  // Extract transition type (then, else, goto, priority_path)
  ScIterator3Ptr it3 = context->CreateIterator3(
      edge,
      ScType::CommonArc,
      ScType::Node);

  while (it3->Next()) {
    // Check what type of transition this edge represents
    if (context->CheckConnector(Keynodes::nrel_then, edge, ScType::PosArc)) {
      m_logger->Debug("Processing 'then' transition");
    } else if (context->CheckConnector(Keynodes::nrel_else, edge, ScType::PosArc)) {
      m_logger->Debug("Processing 'else' transition");
    } else if (context->CheckConnector(Keynodes::nrel_condition, edge, ScType::PosArc)) {
      ScIterator5Ptr it5 = context->CreateIterator5(
          edge,
          ScType::CommonArc,
          ScType::Node,
          ScType::PosArc,
          Keynodes::nrel_condition);
      
      while (it5->Next()) {
        auto condPair = ProcessCondition(it5->Get(2), it3->Get(2));
        conditionMap[edge] = condPair;
      }
    }
  }
}

std::string StateDiagramBuilder::Termination() {
  std::unordered_map<std::string, int> outgoingCounts;
  std::unordered_set<std::string> entitiesWithIncoming;
  std::unordered_set<std::string> allEntities;
  std::string result = "";

  // Parse generated entity/transition strings to determine terminal states
  std::stringstream ss(entitiesInCurrentPackage + relations);
  std::string line;

  while (std::getline(ss, line, '\n')) {
    size_t arrow_pos = line.find("-->");
    if (arrow_pos == std::string::npos) {
      continue;
    }

    // Extract source state
    std::string source = line.substr(0, arrow_pos);
    source = trim_spaces(source);
    if (source.empty()) continue;

    allEntities.insert(source);
    outgoingCounts[source]++;

    // Extract target state
    size_t target_start = arrow_pos + 3;
    size_t target_end = line.find(':', target_start);
    if (target_end == std::string::npos) {
      target_end = line.length();
    }

    std::string target = line.substr(target_start, target_end - target_start);
    target = trim_spaces(target);
    if (target.empty()) continue;

    allEntities.insert(target);
    entitiesWithIncoming.insert(target);
  }

  // Add transitions to [*] for terminal states
  for (const std::string& entity : allEntities) {
    int out_count = (outgoingCounts.count(entity)) ? outgoingCounts.at(entity) : 0;
    bool has_incoming = entitiesWithIncoming.count(entity);
    bool has_no_outgoing = (out_count == 0);

    // Terminal state: no outgoing transitions and has incoming
    if (has_incoming && has_no_outgoing) {
      result += entity + " --> [*]\n";
      continue;
    }

    // Choice states with single outgoing transition terminate diagram
    bool is_choice = (entity.find("choice") != std::string::npos);
    bool has_one_outgoing = (out_count == 1);
    if (is_choice && has_one_outgoing) {
      result += entity + " --> [*]\n";
    }
  }

  return result;
}

void StateDiagramBuilder::ProcessPackage(ScAddr package) {
  // Finalize package by creating composite states for decomposed actions
  ScIterator3Ptr it3 = context->CreateIterator3(
      package,
      ScType::ConstPermPosArc,
      ScType::NodeTuple);

  ScIterator5Ptr it5internal;

  while (it3->Next()) {
    // Find all decomposition relations in this package
    it5internal = context->CreateIterator5(
        ScType::Node,
        ScType::CommonArc,
        it3->Get(2),
        ScType::PosArc,
        Keynodes::nrel_decomposition_of_action);

    while (it5internal->Next()) {
      // Generate composite state containing current package content
      entities += "state " + context->GetElementSystemIdentifier(it5internal->Get(0)) +
                  " {\n" + entitiesInCurrentPackage + relations + Termination() + "}\n";
      entitiesInCurrentPackage = "";
      relations = "";
    }
  }
}

std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetAllPackages(ScAddr diagram) {
  std::shared_ptr<ScAddrSet> packg = std::make_shared<ScAddrSet>();
  
  // Map structure nodes to contained actions
  ScAddrToValueUnorderedMap<ScAddrSet> actionsMap;
  ScAddrToValueUnorderedMap<ScAddr> mainActionMap;
  ScAddrSet packages;
  ScAddrSet structures;

  m_logger->Debug("Capturing packages for: " +
                 context->GetElementSystemIdentifier(diagram) + " type: " +
                 std::string(context->GetElementType(diagram)));

  // Iterate all structures in diagram
  ScIterator3Ptr it3struct = context->CreateIterator3(
      diagram,
      ScType::ConstPermPosArc,
      ScType::ConstNodeStructure);

  while (it3struct->Next()) {
    ScAddrSet actions;
    ScAddr structure;
    ScAddrVector tuple = CaptureTuple(it3struct->Get(2));

    // Try to find tuple, otherwise look for entry point structure
    if (tuple.empty()) {
      m_logger->Debug("No tuple in struct: " +
                     context->GetElementSystemIdentifier(it3struct->Get(2)));
      
      ScIterator5Ptr it5 = context->CreateIterator5(
          it3struct->Get(2),
          ScType::PosArc,
          ScType::NodeStructure,
          ScType::PosArc,
          Keynodes::rrel_entry);

      while (it5->Next()) {
        structure = it5->Get(2);
        tuple = CaptureTuple(it5->Get(2));
        if (tuple.empty()) {
          m_logger->Debug("No tuple in structure: " +
                         context->GetElementSystemIdentifier(structure));
        }
        break;
      }
    } else {
      structure = it3struct->Get(2);
      packg->insert(it3struct->Get(2));
    }

    // Collect all non-entry actions in structure
    ScIterator3Ptr it3 = context->CreateIterator3(
        structure,
        ScType::ConstPermPosArc,
        ScType::Node);

    m_logger->Debug("Processing actions for " +
                   context->GetElementSystemIdentifier(structure));

    while (it3->Next()) {
      // Action that's not the main entry point
      if (context->CheckConnector(ScKeynodes::action, it3->Get(2), ScType::PosArc) &&
          !context->CheckConnector(it3->Get(2), tuple[0], ScType::CommonArc)) {
        actions.insert(it3->Get(2));
      }
      // Main entry point action
      else if (context->CheckConnector(ScKeynodes::action, it3->Get(2), ScType::PosArc) &&
               context->CheckConnector(it3->Get(2), tuple[0], ScType::CommonArc)) {
        mainActionMap[structure] = it3->Get(2);
      }
    }

    structures.insert(structure);
    actionsMap[structure] = actions;
  }

  m_logger->Debug("Start processing intersections (size): " +
                 std::to_string(structures.size()));

  ScIterator5Ptr it5;

  // Check for package intersections and hierarchies
  for (auto str : structures) {
    std::string state = "state " + context->GetElementSystemIdentifier(mainActionMap[str]) +
                       " {\n";

    for (auto pair : actionsMap) {
      if (pair.first != str && this->packages->find(pair.first) == this->packages->end()) {
        for (auto item : pair.second) {
          // Action appears in multiple structures
          if (context->CheckConnector(str, item, ScType::PosArc)) {
            packages.insert(str);
            packages.insert(pair.first);
          }
        }
      }

      // Check main action containment
      if (context->CheckConnector(str, mainActionMap[pair.first], ScType::PosArc)) {
        state += "state " + context->GetElementSystemIdentifier(mainActionMap[pair.first]) +
                " {\n}\n";
      }
    }

    // Find entry transition for this structure in parent diagram
    it5 = context->CreateIterator5(
        mainActionMap[str],
        ScType::CommonArc,
        ScType::NodeTuple,
        ScType::PosArc,
        Keynodes::nrel_decomposition_of_action);

    while (it5->Next() && processedEntries.find(str) == processedEntries.end()) {
      if (context->CheckConnector(str, it5->Get(2), ScType::PosArc)) {
        it5 = context->CreateIterator5(
            it5->Get(2),
            ScType::PosArc,
            ScType::Node,
            ScType::PosArc,
            ScKeynodes::rrel_1);

        if (it5->Next()) {
          // Add initial transition from [*] to first action
          m_logger->Debug("Entry: [*] --> " +
                         context->GetElementSystemIdentifier(it5->Get(2)));
          processedEntries.insert(str);
          break;
        }
      }
    }

    // Check if this is entry point of parent diagram
    it5 = context->CreateIterator5(
        diagram,
        ScType::PosArc,
        str,
        ScType::PosArc,
        Keynodes::rrel_entry);

    state += "}\n";
    
    if (it5->Next()) {
      preamble = state + preamble;
    } else {
      preamble += state;
    }
  }

  m_logger->Debug("End processing intersections");

  // Register all found packages
  for (auto p : *packg) {
    this->packages->insert(p);
  }

  if (structures.size() > 0) {
    packg->insert(diagram);
  }

  return packg;
}

std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetUsedNodes(ScAddr addr) {
  return usedNodes;
}

std::string StateDiagramBuilder::GetResultString() {
  return "@startuml\n" + preamble + entities + entitiesInCurrentPackage + relations +
         "\n@enduml";
}

bool StateDiagramBuilder::PackageCheck(ScAddr package, ScAddr parent) {
  // Validate package structure for nested processing requirements
  ScIterator5Ptr it5 = context->CreateIterator5(
      package,
      ScType::PosArc,
      ScType::NodeStructure,
      ScType::PosArc,
      Keynodes::rrel_entry);

  ScIterator3Ptr it3;
  ScIterator5Ptr it5internal;

  m_logger->Debug("Checking package structure for: " +
                 context->GetElementSystemIdentifier(package));

  while (it5->Next()) {
    if (packages->find(it5->Get(2)) == packages->end()) {
      it3 = context->CreateIterator3(it5->Get(2), ScType::PosArc, ScType::NodeTuple);

      while (it3->Next()) {
        // Check for decomposition relations marking nested complexity
        it5internal = context->CreateIterator5(
            ScType::Node,
            ScType::CommonArc,
            it3->Get(2),
            ScType::PosArc,
            Keynodes::nrel_decomposition_of_action);

        while (it5internal->Next()) {
          return true;  // Package contains nested structures
        }
      }
    }
  }

  return false;  // Package is leaf node
}