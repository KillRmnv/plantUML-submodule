/// @file BaseDiagramBuilder.hpp
/// @brief Abstract base class for diagram builders.
/// 
/// Defines interface for converting sc-memory structures into UML diagram formats.
/// Supports inheritance-based specialization for different diagram types.

#pragma once

#include <string>
#include <memory>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

/// @class BaseDiagramBuilder
/// @brief Abstract builder interface for diagram generation from sc-memory.
/// 
/// Provides common framework for diagram builders processing sc-memory knowledge
/// structures. Subclasses implement specific diagram types (state, class, sequence).
/// Tracks visited nodes/edges to prevent duplication and manage traversal.
class BaseDiagramBuilder {
protected:
  /// Set of processed node addresses (prevents re-processing)
  std::shared_ptr<ScAddrSet> usedNodes;
  
  /// Set of processed edge addresses (prevents re-processing)
  std::shared_ptr<ScAddrSet> usedEdges;
  
  /// Diagram style flag (true for hierarchical, false for flat)
  bool type = false;

  /// Pointer to sc-memory context for element operations
  ScMemoryContext* context;
  
  /// Logger for debug/error messages
  utils::ScLogger* m_logger;

public:
  /// @brief Constructs diagram builder with memory context.
  /// @param context Pointer to ScMemoryContext for memory access
  /// @param logger Pointer to logger utility
  explicit BaseDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger);

  /// @brief Virtual destructor for proper cleanup
  virtual ~BaseDiagramBuilder();

  /// @brief Processes a single node from the knowledge structure.
  /// @param node The node address to process
  /// @param container The parent container (package/diagram)
  virtual void ProcessNode(ScAddr node, ScAddr container) = 0;

  /// @brief Processes outgoing edges from a node.
  /// @param node The source node
  /// @param container The parent container
  virtual void ProcessEdgesByNode(ScAddr node, ScAddr container) = 0;

  /// @brief Processes adjacent nodes reachable from current node.
  /// @param node The reference node
  /// @param container The parent container
  virtual void ProcessAdjacentNodes(ScAddr node, ScAddr container) = 0;

  /// @brief Generates final diagram output string.
  /// @return Diagram in target format (PlantUML, Graphviz, etc.)
  virtual std::string GetResultString() = 0;

  /// @brief Returns sc-memory context pointer.
  /// @return Pointer to ScMemoryContext
  ScMemoryContext* GetContext();

  /// @brief Returns logger utility pointer.
  /// @return Pointer to utils::ScLogger
  utils::ScLogger* GetLogger();

  /// @brief Sets diagram style/type flag.
  /// @param type True for hierarchical layout, false for flat
  void SetType(bool type);

  /// @brief Indicates if builder supports package hierarchies.
  /// @return True if builder handles nested packages, false otherwise
  virtual bool SupportsPackages() const;
};