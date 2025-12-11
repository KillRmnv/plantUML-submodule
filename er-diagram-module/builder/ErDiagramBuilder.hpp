
#pragma once 
#include <string>
#include <utility> 
#include <set>
#include <sc-memory/sc_addr.hpp>

#include "../../common/BaseDiagramBuilder.hpp"
#include "../../common/DiagramBuilder.hpp"
using namespace std;

/// @class ErDiagramBuilder
/// @brief Generates Chen Entity-Relationship (ER) diagrams from sc-memory structures.
/// 
/// Inherits from BaseDiagramBuilder to process entity-relationship models and generate
/// PlantUML Chen ER diagram syntax. Handles:
/// - Entity definitions (strong and weak entities)
/// - Attribute specifications (key, derived, multivalued)
/// - Relationship definitions with cardinality constraints
/// - Chen notation with proper ER syntax
class ErDiagramBuilder : public BaseDiagramBuilder{
public:
    /// @brief Constructs an ER diagram builder.
    /// @param context Pointer to the sc-memory context
    /// @param logger Pointer to the logger utility
    explicit ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    /// @brief Processes entity and relationship nodes.
    /// @param Node The node to process (entity or relationship)
    /// @param package The container package scope
    void ProcessNode(ScAddr Node,ScAddr package) override;

    /// @brief No edge-specific processing required for ER diagrams.
    /// Edge processing handled in ProcessAdjacentNodes instead.
    /// @param Node The source node
    /// @param package The container package
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override {} 

    /// @brief Processes adjacent entities connected via relationships.
    /// @param Node The reference entity node
    /// @param package The container package scope
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;

    /// @brief Returns the complete PlantUML ER diagram string.
    /// @return Formatted diagram in Chen ER syntax (@startchen...@endchen)
    std::string GetResultString() override;

private:
    /// @brief Retrieves all attributes belonging to an entity.
    /// Filters all connected nodes by concept_attribute check.
    /// @param entity The entity node
    /// @param package The container package
    /// @return Vector of attribute node addresses
    std::vector<ScAddr> GetAttributes(ScAddr entity,ScAddr package);

    /// @brief Checks if attribute is optional (not required).
    /// Returns true if attribute NOT marked as concept_key_attribute.
    /// @param attr The attribute node to check
    /// @return True if attribute is optional
    bool IsOptional(ScAddr attr);

    /// @brief Checks if attribute is required (key attribute).
    /// Returns true if attribute marked as concept_key_attribute.
    /// @param attr The attribute node to check
    /// @return True if attribute is required/key
    bool IsRequired(ScAddr attr);

    /// @brief Checks if attribute is multivalued.
    /// Returns true if marked as concept_multivalued_attribute.
    /// @param attr The attribute node to check
    /// @return True if attribute can have multiple values
    bool IsMultivalued(ScAddr attr);

    /// @brief Retrieves child attributes of a composite attribute.
    /// Composite attributes can contain nested sub-attributes.
    /// @param attr The parent attribute node
    /// @param package The container package
    /// @return Vector of child attribute node addresses
    std::vector<ScAddr> GetChildAttributes(ScAddr attr,ScAddr package);

    /// @brief Recursively generates PlantUML text for attribute with children.
    /// Handles nested attributes with proper indentation.
    /// Adds markers: <> for key, <> for derived, <> for multivalued.
    /// @param attr The attribute node to process
    /// @param package The container package
    /// @param indent Current indentation level (spaces for formatting)
    /// @return PlantUML text representation with nesting
    std::string ProcessAttributeRecursively(ScAddr attr, ScAddr package, const std::string& indent);

    /// @brief Determines Chen cardinality pair for a relationship.
    /// Checks for strong_relation to use "=" vs "-" symbols.
    /// Maps concept_one/many/zero_or_one/zero_or_many to Chen notation.
    /// Uses concept_first_domain to determine left/right placement.
    /// @param classNode The cardinality constraint node
    /// @return Pair of strings: (left_cardinality, right_cardinality)
    std::pair<std::string,std::string> ChenCardinality(ScAddr classNode, bool hasFirst);

    /// @brief Generates PlantUML entity block with attributes.
    /// Creates "entity name { ... }" block with nested attributes.
    /// Marks weak entities with notation.
    /// @param entity The entity node to convert
    /// @param package The container package
    /// @return PlantUML entity block as string
    std::string MakeEntityBlock(ScAddr entity,ScAddr package);

    /// @brief Generates PlantUML relationship block with attributes.
    /// Creates "relationship name { ... }" block with nested attributes.
    /// Marks identifying relationships with notation.
    /// @param relNode The relationship node to convert
    /// @param package The container package
    /// @return PlantUML relationship block as string
    std::string MakeRelationshipBlock(ScAddr relNode,ScAddr package); 
   
    /// PlantUML entity declarations (entity blocks)
    std::string entities_;

    /// PlantUML relationship declarations (relationship blocks)
    std::string relationships_;

    /// PlantUML relation declarations (connections between entities and relationships)
    std::string relations_;

    /// Tracks processed relationships to avoid duplication
    ScAddrSet usedRelationships; 

    /// Maps entity nodes to their relationships for cardinality tracking
    ScAddrToValueUnorderedMap<ScAddr> entityToRelation;

};


