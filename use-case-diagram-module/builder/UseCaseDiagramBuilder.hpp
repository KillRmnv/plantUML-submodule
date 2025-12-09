/// @file UseCaseDiagramBuilder.hpp
/// @brief Builds UML use case diagrams from sc-memory actor and use case structures.
/// 
/// Processes use case hierarchies, actor relationships, and semantic relations
/// to generate PlantUML use case diagrams with packages, actors, use cases, and interactions.

#ifndef C974FD10_B617_4AC4_9E70_53C4B4560763
#define C974FD10_B617_4AC4_9E70_53C4B4560763

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include "common/PackageDiagramBuilder.hpp"
#include "common/DiagramBuilder.hpp"

using namespace std;

/// @brief Hash functor for tuple of 3 ScAddr addresses.
/// 
/// Enables use of std::tuple<ScAddr, ScAddr, ScAddr> as key in unordered_map.
/// Uses FNV-like hash combining algorithm for even distribution.
struct ScAddrTripleHash
{
  size_t operator()(std::tuple<ScAddr, ScAddr, ScAddr> const & t) const
  {
    auto const& [a, b, c] = t;
    ScAddrHashFunc h;
    size_t r = 0;
    r ^= h(a) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
    r ^= h(b) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
    r ^= h(c) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
    return r;
  }
};

/// @brief Equality comparator for tuple of 3 ScAddr addresses.
/// 
/// Compares all three components for identity.
struct ScAddrTripleEq
{
  bool operator()(std::tuple<ScAddr, ScAddr, ScAddr> const & x,
                  std::tuple<ScAddr, ScAddr, ScAddr> const & y) const
  {
    return std::get<0>(x) == std::get<0>(y) && std::get<1>(x) == std::get<1>(y) &&
           std::get<2>(x) == std::get<2>(y);
  }
};

/// @class UseCaseDiagramBuilder
/// @brief Generates UML use case diagrams from sc-memory structures.
/// 
/// Inherits from PackageDiagramBuilder to support hierarchical package processing.
/// Processes:
/// - Actor entities and their inclusion relations
/// - Use case structures (concept_er_package)
/// - Interactions: nrel_extend_use_case, nrel_include_use_case, nrel_generalization_use_case
/// - Package nesting and hierarchy levels
class UseCaseDiagramBuilder : public PackageDiagramBuilder
{
public:
  /// @brief Constructs a UseCaseDiagramBuilder instance.
  /// @param context Pointer to the sc-memory context
  /// @param logger Pointer to the logger utility
  explicit UseCaseDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

  /// @brief Processes a single use case or actor node.
  /// @param Node The node to process (use case structure or actor)
  /// @param package The container package
  void ProcessNode(ScAddr Node, ScAddr package) override;

  /// @brief Processes outgoing relation edges from a node.
  /// @param Node The source node
  /// @param package The container package
  void ProcessEdgesByNode(ScAddr Node, ScAddr package) override;

  /// @brief Processes adjacent nodes reachable via semantic relations.
  /// @param Node The reference node
  /// @param package The container package
  void ProcessAdjacentNodes(ScAddr Node, ScAddr package) override;

  /// @brief Retrieves all package structures in the diagram.
  /// @param diagram The root diagram node
  /// @return Shared set of package addresses
  std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;

  /// @brief Retrieves all processed nodes.
  /// @param diagram The root diagram node
  /// @return Shared set of used node addresses
  std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;

  /// @brief Returns the complete PlantUML diagram string.
  /// @return Formatted diagram in PlantUML syntax (@startuml...@enduml)
  std::string GetResultString() override;

  /// @brief Finalizes package processing.
  /// @param package The package being completed
  void ProcessPackage(ScAddr package) override;

  /// @brief Validates package structure for nested processing.
  /// @param package The package to validate
  /// @param parent The parent package context
  /// @return True if package contains nested structures requiring recursion
  bool PackageCheck(ScAddr package, ScAddr parent) override;

private:
  /// @brief Processes a relation edge between nodes.
  /// 
  /// Determines relation type (extend, include, generalization) and generates
  /// appropriate PlantUML transition syntax. Handles actor-action associations.
  /// 
  /// @param edge The transition edge address
  /// @param relation The semantic relation keynode (extend/include/generalization)
  /// @param package The container package
  void ProcessEdge(ScAddr edge, ScAddr relation, ScAddr package);

  /// @brief Trims semantic prefixes from system identifiers.
  /// 
  /// Removes "nrel_", "rrel_", "concept_" prefixes for cleaner diagram labels.
  /// 
  /// @param s Input identifier string
  /// @return Trimmed identifier without prefix
  std::string trim(const std::string & s);

  /// @brief Removes leading and trailing whitespace from string.
  /// @param str Input string
  /// @return Trimmed string
  std::string trim_spaces(std::string str);

  /// Maps package hierarchy levels to sets of (actor, usecase, relation) tuples.
  /// Used for detecting and validating package nesting and cross-package relations.
  std::unordered_map<int, std::unordered_set<std::tuple<ScAddr, ScAddr, ScAddr>,
                                              ScAddrTripleHash, ScAddrTripleEq>>
      structuresByLevel;

  /// Maps use case structure addresses to their nesting levels.
  /// Tracks package depth for hierarchy validation.
  ScAddrToValueUnorderedMap<std::set<int>> addrMap;

  /// Current nesting level during recursive package processing.
  int level = 0;

  /// PlantUML entity declarations (actors, use cases) in current package.
  std::string entitiesInCurrentPackage;

  /// PlantUML entity declarations for all completed packages.
  std::string entities;

  /// PlantUML relation/interaction declarations (extend, include, generalization).
  std::string relations;

  /// PlantUML preamble with package hierarchies.
  std::string preamble;

  /// Maps node addresses to their system identifiers for use case labels.
  ScAddrToValueUnorderedMap<std::string> nameByNode;

  /// Maps structure addresses to their canonical use case names.
  ScAddrToValueUnorderedMap<std::string> nameByStruct;

  /// Maps actor addresses to sets of relations they participate in.
  /// Used to avoid duplicating actor-action associations.
  ScAddrToValueUnorderedMap<std::unordered_set<ScAddr, ScAddrHashFunc>>
      actorsToActions;
};

#endif /* C974FD10_B617_4AC4_9E70_53C4B4560763 */