/// @file PackageDiagramBuilder.hpp
/// @brief Base class for diagram builders supporting hierarchical packages.
/// 
/// Extends BaseDiagramBuilder with package-aware processing for diagrams
/// with nested/composite structures.

#pragma once

#include <memory>
#include <sc-memory/sc_addr.hpp>
#include "BaseDiagramBuilder.hpp"

/// @class PackageDiagramBuilder
/// @brief Builder for diagram types with package hierarchies.
/// 
/// Adds package processing capabilities to diagram generation. Used for diagram
/// types that support nested structures (state diagrams with composite states,
/// class diagrams with packages, etc.). Tracks packages and provides hooks for
/// package-specific processing.
class PackageDiagramBuilder : public BaseDiagramBuilder {
public:
  /// @brief Constructs package-aware diagram builder.
  /// @param context Pointer to the sc-memory context
  /// @param logger Pointer to the logger utility
  explicit PackageDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger);

  /// @brief Virtual destructor for proper cleanup
  ~PackageDiagramBuilder() override;

  /// @brief Indicates this builder supports package processing.
  /// @return Always returns true for package-aware builders
  bool SupportsPackages() const override final;

  /// @brief Processes completed package after all contained elements.
  /// 
  /// Called when package traversal is complete. Allows builders to finalize
  /// package representation (e.g., closing composite states, adding package
  /// properties to output).
  /// 
  /// @param package The package being finalized
  virtual void ProcessPackage(ScAddr package);

  /// @brief Validates package for nested processing.
  /// 
  /// Determines if a package contains sub-packages or structures requiring
  /// recursive processing. Used during traversal to identify which packages
  /// need deeper exploration.
  /// 
  /// @param package The package to validate
  /// @param parent The parent package context
  /// @return True if package contains nested structures, false if it's a leaf package
  virtual bool PackageCheck(ScAddr package, ScAddr parent);

  /// @brief Retrieves all packages in diagram structure.
  /// 
  /// Identifies and returns all package nodes contained in the given diagram.
  /// Used during initialization to determine traversal scope.
  /// 
  /// @param diagram The root diagram node
  /// @return Shared set of package addresses
  virtual std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram);

  /// @brief Retrieves processed nodes from diagram.
  /// 
  /// Returns the set of nodes that have been processed. Useful for
  /// querying builder state after diagram generation.
  /// 
  /// @param diagram The root diagram node
  /// @return Shared set of processed node addresses
  virtual std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram);
};