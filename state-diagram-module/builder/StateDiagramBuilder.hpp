/// @file StateDiagramBuilder.hpp
/// @brief Builds UML state diagrams from sc-memory action structures.
/// 
/// This builder processes action nodes with decomposition relationships and 
/// generates PlantUML state machine diagrams representing the control flow.

#ifndef E880379C_8752_4327_BDEC_4CFBE899C1DB
#define E880379C_8752_4327_BDEC_4CFBE899C1DB

#include <string>
#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>


 #include "../../common/PackageDiagramBuilder.hpp"
 #include "../../common/DiagramBuilder.hpp"
#include <vector>
using namespace std;

/// @class StateDiagramBuilder
/// @brief Generates UML state diagrams from sc-memory action structures.
/// 
/// Inherits from PackageDiagramBuilder to support hierarchical package processing.
/// Extracts action nodes, their relationships, transitions, and conditions to 
/// construct PlantUML syntax for state machine visualization.
class StateDiagramBuilder : public PackageDiagramBuilder {

public:
  /// @brief Constructs a StateDiagramBuilder instance.
  /// @param context Pointer to the sc-memory context
  /// @param logger Pointer to the logger utility
  explicit StateDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger);

  /// @brief Processes a single action node.
  /// @param Node The action node to process
  /// @param package The container package
  void ProcessNode(ScAddr Node, ScAddr package) override;

  /// @brief Processes outgoing edges and transitions from a node.
  /// @param Node The source action node
  /// @param package The container package
  void ProcessEdgesByNode(ScAddr Node, ScAddr package) override;

  /// @brief Processes adjacent nodes connected via relations.
  /// @param Node The reference node
  /// @param package The container package
  void ProcessAdjacentNodes(ScAddr Node, ScAddr package) override;

  /// @brief Retrieves all package structures in the diagram.
  /// @param diagram The root diagram node
  /// @return Shared set of package addresses
  std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;

  /// @brief Retrieves all processed action nodes.
  /// @param diagram The root diagram node
  /// @return Shared set of used node addresses
  std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;

  /// @brief Returns the complete PlantUML diagram string.
  /// @return Formatted diagram in PlantUML syntax (@startuml...@enduml)
  std::string GetResultString() override;

  /// @brief Finalizes package processing and adds to diagram.
  /// @param package The package being completed
  void ProcessPackage(ScAddr package) override;

private:
  /// @brief Finds all entry point actions in a decomposition structure.
  /// @param action The main action node
  /// @param package The container package
  /// @return Vector of entry point edge addresses
  std::vector<std::pair<ScAddr, int>>  FindEntryPoints(ScAddr action, ScAddr package);

  /// @brief Extracts tuple elements from a structure.
  /// @param structure The structure node containing tuple edges
  /// @return Vector of tuple element addresses
  ScAddrVector CaptureTuple(ScAddr structure);

  /// @brief Builds all possible action sequences from entry points.
  /// @param entries Collection of entry point edges
  /// @param package The container package
  /// @param Node The root action node
  /// @return Vector of sequences, each containing ordered action addresses
  std::vector<ScAddrVector> FindSequence(
    std::vector<std::pair<ScAddr, int>> entries,
    ScAddr package, ScAddr Node);

  /// @brief Groups equal-priority action sequences.
  /// @param sequences All detected action sequences
  /// @param entries Entry point edges with their sources
  /// @return Vector of priority-grouped sequence combinations
  std::vector<std::vector<int>> FormEqualPrioritiesSequences(
    std::vector<ScAddrVector> sequences,
    std::vector<std::pair<ScAddr, int>> entries);

  /// @brief Generates all permutations of action sequences.
  /// @param sequences Input action sequences
  /// @return All possible combinations respecting sequence order
  std::vector<ScAddrVector> Combinations(std::vector<ScAddrVector> sequences);

  /// @brief Generates state and transition declarations for action combination.
  /// @param comb Combination of action addresses
  /// @param Node Reference action context
  /// @param condition Starting state identifier
  /// @return PlantUML entity and transition strings
  std::string AddEntitiesAndTransitions(ScAddrVector comb,ScAddr Node,std::string condition,int num);


  /// @brief Creates transitions between consecutive actions in a path.
  /// @param combByInt Indexed action combinations
  /// @param NextAction Target action node
  /// @param BaseAction Source action node
  /// @return PlantUML transition declarations
  std::string CreateEdgeBetweenActions(
      std::unordered_map<int, std::vector<ScAddr>> combByInt,
      ScAddrVector NextAction,
      ScAddr BaseAction);

  /// @brief Processes a transition edge with its relations.
  /// @param edge The transition edge address
  /// @param package The container package
  void ProcessEdge(ScAddr edge, ScAddr package);

  /// @brief Extracts condition information from a relation arc.
  /// @param Condition The condition relation node
  /// @param Node The context action node
  /// @return Pair of (condition_state_name, condition_label_text)
  std::pair<std::string, std::string> ProcessCondition(ScAddr Condition, ScAddr Node);

  /// @brief Validates package structure recursiveness.
  /// @param package The package to validate
  /// @param parent The parent package context
  /// @return True if package contains nested structures requiring processing
  bool PackageCheck(ScAddr package, ScAddr parent) override;

  /// @brief Determines and connects terminal states to [*] endpoint.
  /// @return PlantUML declarations for states without outgoing transitions
  std::string Termination(ScAddr package);
  std::string AddTransitions(ScAddr Node,std::string end,std::string cond);
  std::string FormRelations();
  std::string RemoveRedundantEntities(ScAddr package);
  // --- Member variables ---

  /// Counter for anonymous choice/decision states
  int conditionCounter = 0;

  /// Maps action node addresses to their PlantUML state identifiers
  ScAddrToValueUnorderedMap<std::vector<std::string>> nodes;

  /// Maps condition arcs to (state_name, label) pairs for transitions
  ScAddrToValueUnorderedMap<std::pair<std::string, std::string>> conditionMap;

  /// Cached set of all detected packages in current diagram
  std::shared_ptr<ScAddrSet> packages;

  /// Tracks processed entry point edges to avoid duplication
  ScAddrSet processedEntries;

  /// PlantUML state declarations for all packages
  std::string entities;

  /// PlantUML state declarations for current package
  std::string entitiesInCurrentPackage;

  /// PlantUML transition declarations
  std::string relations;

  /// PlantUML preamble containing package hierarchies
  std::string preamble;

  /// Maps transition relation keynodes to their priority levels
  /// Priority order: nrel_priority_path (4) > nrel_then (3) > nrel_else (2) > nrel_goto (1)
  ScAddrToValueUnorderedMap<int> priorities;
  ScAddrToValueUnorderedMap<std::vector<std::string>> relationsByAddr;

};

#endif /* E880379C_8752_4327_BDEC_4CFBE899C1DB */