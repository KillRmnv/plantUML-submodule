#include "ErDiagramBuilder.hpp"
#include <utility>
#include <vector>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include "keynodes/Keynodes.hpp"
#include <unordered_map>
#include <string>
#include <vector>

/// Constructor: Initialize builder with empty output strings
ErDiagramBuilder::ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : BaseDiagramBuilder(context, logger)
{
    m_logger->Debug("ErDiagramBuilder: Initialized.");
    entities_ = "";
    relationships_ = "";
    relations_ = "";
}

/// Finds all attributes connected to entity via constant positive arcs
/// and filters by checking concept_attribute classifier
ScAddrVector ErDiagramBuilder::GetAttributes(ScAddr entity,ScAddr package)
{
    ScAddrVector attrs;
    auto it = context->CreateIterator5( 
        entity,
        ScType::ConstPermPosArc,
        ScType::ConstNode,
        ScType::PosArc,
        package
    );
    while (it->Next()) {
        ScAddr candidate = it->Get(2);
        if (context->CheckConnector(Keynodes::concept_attribute, 
                                    candidate,
                                    ScType::ConstPermPosArc))
            attrs.push_back(candidate);
    }

    m_logger->Debug("GetAttributes: Found " + std::to_string(attrs.size()) + " attributes for entity " + std::to_string(entity.Hash()));

    return attrs;
}

/// Recursively processes attribute node with nested children
/// Adds Chen notation markers:
/// - <> for required (key) attributes
/// - <> for derived attributes (computed fields)
/// - <> for multivalued attributes (can repeat)
std::string ErDiagramBuilder::ProcessAttributeRecursively(ScAddr attr, ScAddr package, const std::string& indent)
{
    std::string attr_name = context->GetElementSystemIdentifier(attr);

    if (IsRequired(attr))
        attr_name += " <<key>>";

    if (IsDerived(attr))
        attr_name += "<<derived>>";

    if (IsMultivalued(attr))
        attr_name += " <<multi>>";
    
    auto children = GetChildAttributes(attr, package);

    std::string result;

    if (!children.empty()) {
        result += indent + attr_name + " {\n";
        for (ScAddr child : children) {

            result += ProcessAttributeRecursively(child, package, indent + "  ");
        }
        result += indent + "}\n";
    } else {
        result += indent + attr_name + "\n";
    }

    return result;
}

/// Checks if attribute is marked as derived (calculated from other attributes)
bool ErDiagramBuilder::IsDerived(ScAddr attr)
{
    return context->CheckConnector(Keynodes::concept_derived_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc);
}

/// Checks if attribute is required key attribute (primary key, uniqueness constraint)
bool ErDiagramBuilder::IsRequired(ScAddr attr)
{
    return context->CheckConnector(Keynodes::concept_key_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc);
}

/// Checks if attribute is multivalued (can have multiple values per entity instance)
bool ErDiagramBuilder::IsMultivalued(ScAddr attr)
{
    return context->CheckConnector(Keynodes::concept_multivalued_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc);
}

/// Retrieves child attributes nested under composite attribute
/// Used for hierarchical attribute structures
ScAddrVector ErDiagramBuilder::GetChildAttributes(ScAddr attr,ScAddr package)
{
    ScAddrVector child;
    auto it = context->CreateIterator5( 
        attr,
        ScType::ConstPermPosArc,
        ScType::ConstNode,
        ScType::PosArc,
        package
    );
    while (it->Next()) {
        ScAddr candidate = it->Get(2);
        if (context->CheckConnector(Keynodes::concept_attribute, 
                                    candidate,
                                    ScType::ConstPermPosArc))
            child.push_back(candidate);
    }
    return child;
}

// Determines Chen notation cardinality symbols for relationship
/// Uses concept_strong_relation to choose "=" (total participation) vs "-" (partial)
/// Maps cardinality concepts to Chen notation: 1, N, (0,1), (0,N), (1,N)
/// concept_first_domain indicates left/right positioning
std::pair<std::string,std::string> ErDiagramBuilder::ChenCardinality(ScAddr classNode, bool hasFirst)
{
    // Determine participation type: strong (=) or weak (-)
    std::string symbol;
    if (context->CheckConnector(Keynodes::concept_strong_relation, 
                                    classNode,
                                    ScType::ConstPermPosArc)){
                                        symbol="=";
                                    } 
    else{
        symbol="-";
    } 

    // Default cardinality pair
    std::pair<std::string,std::string> card = std::make_pair("---","---");

    // Map each cardinality concept to Chen notation
    if(Keynodes::concept_one_or_many==classNode){

        if(!hasFirst&&context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(1,N)"+symbol, "");
        }else {
            card = make_pair("",symbol+"(1,N)"+symbol);
        }
    }
    else if(Keynodes::concept_zero_or_one==classNode){

        if(!hasFirst&&context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(0,1)"+symbol, "");
        }else{
            card = make_pair("",symbol+"(0,1)"+symbol);
        }
    }
    else if(Keynodes::concept_zero_or_many==classNode){

        if(!hasFirst&&context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(0,N)"+symbol, "");
        }else{
            card = make_pair("",symbol+"(0,N)"+symbol);
        }
    }else if(Keynodes::concept_one==classNode){

        if(!hasFirst&&context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"1"+symbol, "");
        }else{
            card = make_pair("",symbol+"1"+symbol);
        }
    }else if(Keynodes::concept_many==classNode){

        if(!hasFirst&&context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"N"+symbol, "");
        }else{
            card = make_pair("",symbol+"N"+symbol);
        }
    }

    return card;
}

/// Generates PlantUML entity block with all attributes
/// Marks weak entities with special notation
/// Includes all nested attributes recursively
std::string ErDiagramBuilder::MakeEntityBlock(ScAddr entity,ScAddr package)
{
    std::string name = context->GetElementSystemIdentifier(entity); 

    m_logger->Debug("MakeEntityBlock: Generating block for " + name);

    bool isWeak = context->CheckConnector( 
        Keynodes::concept_weak_entity,
        entity,
        ScType::ConstPermPosArc);

    std::string block = "entity " + name;
    if (isWeak)
        block += " <<weak>>";
    block += " {\n";

    for (ScAddr attr : GetAttributes(entity, package)) {
        block += ProcessAttributeRecursively(attr, package, "  ");
    }

    block += "}\n\n";
    return block;
}

/// Generates PlantUML relationship block with all attributes
/// Marks identifying relationships (weak entity relationships) with notation
/// Includes nested attributes recursively
std::string ErDiagramBuilder::MakeRelationshipBlock(ScAddr relNode,ScAddr package)
{
    std::string name = context->GetElementSystemIdentifier(relNode);
    if (name.empty()) return "";

    m_logger->Debug("MakeRelationshipBlock: Generating block for " + name);
    
    std::string block = "relationship " + name;
    
    
    if (context->CheckConnector(Keynodes::concept_identifying_relationship, relNode, ScType::ConstPermPosArc))
        block += " <<identifying>>";

    block += " {\n";

    
    for (ScAddr attr : GetAttributes(relNode, package)) {
        block += ProcessAttributeRecursively(attr, package, "  ");
    }

    block += "}\n\n";
    return block;
}

/// Processes single node: if entity, generates entity block
/// Skips if node already processed (usedNodes tracking)
/// Only processes nodes in current package scope
void ErDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package)
{
    if(context->CheckConnector(package, Node, ScType::PosArc)){
        std::string nodeName = context->GetElementSystemIdentifier(Node);
        m_logger->Debug("ProcessNode: Inspecting node " + nodeName);

        if (context->CheckConnector(Keynodes::concept_entity, Node, ScType::ConstPermPosArc))
        {
            if (usedNodes->find(Node) == usedNodes->end())
            {   
            
                entities_ += MakeEntityBlock(Node,package);
                usedNodes->insert(Node);
            }
        }
    }
}

/// Processes adjacent entities connected via relationships
/// Finds all entities reachable from current node through relationship nodes
/// Generates cardinality-annotated connections between entities
/// Tracks used relationships to prevent duplication
void ErDiagramBuilder::ProcessAdjacentNodes(ScAddr Node, ScAddr package)
{
    if(context->CheckConnector(package, Node, ScType::PosArc)){
    
        std::string startNodeName = context->GetElementSystemIdentifier(Node);
        m_logger->Debug("ProcessAdjacentNodes: Starting search from " + startNodeName);
        
        ScIterator5Ptr it5 = context->CreateIterator5(
            Node,
            ScType::ConstCommonArc,
            ScType::ConstNode,
            ScType::ConstPosArc,
            ScType::ConstNodeNonRole
        );

        while (it5->Next()) {
            ScAddr entity2 = it5->Get(2);
            ScAddr relNode = it5->Get(4);
            
            // Filter: only process if adjacent node is also an entity
            if (!context->CheckConnector(Keynodes::concept_entity, entity2, ScType::ConstPermPosArc))
                continue;
            
            std::string e1_name = context->GetElementSystemIdentifier(Node);
            std::string e2_name = context->GetElementSystemIdentifier(entity2);
            std::string rel_name = context->GetElementSystemIdentifier(relNode);

            m_logger->Debug("ProcessAdjacentNodes: Found Arc. Relation: " + rel_name + " -> Target: " + e2_name);
            
            // Skip if any identifier is empty (incomplete structure)
            if (e1_name.empty() || e2_name.empty() || rel_name.empty()){
                m_logger->Debug("ProcessAdjacentNodes: One of identifiers is empty. Skipping.");
                continue;
            }
                
        
                m_logger->Debug("ProcessAdjacentNodes: New Relationship found: " + rel_name + ". Making block.");

            // Generate relationship block if not already processed
            if(usedRelationships.find(relNode)==usedRelationships.end()){
                    relationships_ += MakeRelationshipBlock(relNode,package);
                    usedRelationships.insert(relNode);
            }

            // Extract cardinality constraints and generate connection lines
            ScIterator5Ptr it5=context->CreateIterator5(ScType::ConstNodeClass, ScType::PosArc, relNode, ScType::PosArc, package);
            while (it5->Next()) {
                std::pair<std::string,std::string> card = ChenCardinality(it5->Get(0));
            // Add first connection (entity1 -> relationship) if left cardinality exists
            if(!card.first.empty()&&entityToRelation.find(Node)==entityToRelation.end()){
                entityToRelation[Node]=relNode;
                relations_+= e1_name + " " + card.first + " " + rel_name + "\n";
            }

            // Add second connection (relationship -> entity2) if right cardinality exists
            if(!card.second.empty())
            relations_+= rel_name + " " + card.second + " " + e2_name + "\n";
            }
        }
    }
}

/// Assembles final PlantUML ER diagram string in Chen notation
/// Combines relationships, entities, and connections in proper order
std::string ErDiagramBuilder::GetResultString()
{
    m_logger->Debug("GetResultString: Assembling result.");
    std::string result = "@startchen\n";
    result += "left to right direction\n\n";

    result += relationships_;
    result += entities_;
    result += relations_;

    result += "\n@endchen";
    return result;
}