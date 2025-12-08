#ifndef CF32D435_A4E1_4F2A_9CDB_B2D80032CB3F
#define CF32D435_A4E1_4F2A_9CDB_B2D80032CB3F
/// @file DiagramBuilder.hpp
/// @brief Orchestrates diagram generation from sc-memory structures.
/// 
/// Manages the recursive traversal of knowledge structures and delegation
/// to specialized builders for processing packages and nodes.

#pragma once

#include <memory>
#include <sc-memory/sc_addr.hpp>
#include "BaseDiagramBuilder.hpp"

/// @class DiagramBuilder
/// @brief Orchestrates diagram structure generation.
/// 
/// Implements template method pattern for diagram generation. Handles recursive
/// package processing and delegates node/edge processing to builder implementations.
/// Manages traversal order: packages (depth-first) → nodes → edges → adjacent nodes.
class DiagramBuilder {
public:
  /// @brief Generates diagram structure from sc-memory.
  /// 
  /// Performs recursive traversal of diagram structure starting from root node.
  /// For builders supporting packages, processes all nested packages first in
  /// depth-first order, then processes nodes and edges in each package scope.
  /// 
  /// Traversal algorithm:
  /// 1. Check if builder supports packages (via SupportsPackages())
  /// 2. If yes: recursively process all nested package structures
  /// 3. For all packages (including root): process contained nodes
  /// 4. For each node: process outgoing edges and adjacent nodes
  /// 5. Call ProcessPackage() hook when package is complete
  /// 
  /// @param builder Polymorphic diagram builder instance
  /// @param diagram Root diagram/package node address
  /// @throws std::runtime_error if builder claims package support but isn't PackageDiagramBuilder
  void generateStructure(std::shared_ptr<BaseDiagramBuilder> builder, ScAddr diagram);
};




#endif /* CF32D435_A4E1_4F2A_9CDB_B2D80032CB3F */
