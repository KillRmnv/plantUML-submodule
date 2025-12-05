#include "ErDiagramBuilder.hpp"
#include <vector>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include "keynodes/Keynodes.hpp"
#include <unordered_map>
#include <string>
#include <vector>

ErDiagramBuilder::ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : ParticularDiagramBuilder(context, logger)
{
    
    entities_ = "";
    relationships_ = "";
    relations_ = "";
}

std::vector<ScAddr> ErDiagramBuilder::GetClassMembers(ScAddr class_node)
{
    std::vector<ScAddr> items;
    auto it = context->CreateIterator3( 
        class_node,
        ScType::ConstPermPosArc,
        ScType::ConstNode
    );
    while (it->Next())
        items.push_back(it->Get(2));
    return items;
}

std::vector<ScAddr> ErDiagramBuilder::GetAttributes(ScAddr entity)
{
    std::vector<ScAddr> attrs;
    auto it = context->CreateIterator3( 
        entity,
        ScType::ConstPermPosArc,
        ScType::ConstNode
    );
    while (it->Next()) {
        ScAddr candidate = it->Get(2);
        if (context->CheckConnector(Keynodes::concept_attribute, 
                                    candidate,
                                    ScType::ConstPermPosArc))
            attrs.push_back(candidate);
    }
    return attrs;
}

bool ErDiagramBuilder::IsOptional(ScAddr attr)
{
    return context->CheckConnector(Keynodes::concept_optional_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc);
}

bool ErDiagramBuilder::IsRequired(ScAddr attr)
{
    return context->CheckConnector(Keynodes::concept_key_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc);
}

std::vector<ScAddr> ErDiagramBuilder::GetChildAttributes(ScAddr attr)
{
    std::vector<ScAddr> child;
    auto it = context->CreateIterator3( 
        attr,
        ScType::ConstPermPosArc,
        ScType::ConstNode
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

std::string ErDiagramBuilder::ChenCardinality(ScAddr relNode)
{
    bool isIdentifying = context->CheckConnector(
        Keynodes::nrel_identifying_relationship, 
        relNode,
        ScType::ConstPermPosArc
    );
    
    std::string card = "---";

    if (context->CheckConnector(Keynodes::concept_one_to_one, relNode, ScType::ConstPermPosArc))
        card = "-1-";
    else if (context->CheckConnector(Keynodes::concept_one_to_many, relNode, ScType::ConstPermPosArc) ||
             context->CheckConnector(Keynodes::concept_many_to_one, relNode, ScType::ConstPermPosArc))
        card = "-N-"; 
    else if (context->CheckConnector(Keynodes::concept_many_to_many, relNode, ScType::ConstPermPosArc))
        card = "-N-";

    if (isIdentifying) {
        if (card == "-1-") return "=1=";
        if (card == "-N-") return "=N=";
    }

    if (card == "---") return "---";

    return card;
}


std::string ErDiagramBuilder::MakeEntityBlock(ScAddr entity)
{
    std::string name = context->GetElementSystemIdentifier(entity); 

    bool isWeak = context->CheckConnector( 
        Keynodes::concept_weak_entity,
        entity,
        ScType::ConstPermPosArc);

    std::string block = "entity " + name;
    if (isWeak)
        block += " <<weak>>";
    block += " {\n";

    for (ScAddr attr : GetAttributes(entity)) {
        std::string attr_name = context->GetElementSystemIdentifier(attr); 

        if (context->CheckConnector(Keynodes::concept_key_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc))
            attr_name += " <<key>>";

        if (IsOptional(attr))
            attr_name += "?";

        auto children = GetChildAttributes(attr);
        if (!children.empty()) {
            block += "  " + attr_name + " {\n";
            for (ScAddr child : children) {
                std::string child_name = context->GetElementSystemIdentifier(child); 

                if (context->CheckConnector(Keynodes::concept_key_attribute, 
                                            child,
                                            ScType::ConstPermPosArc))
                    child_name += " <<key>>";

                if (IsOptional(child))
                    child_name += "?";

                block += "    " + child_name + "\n";
            }
            block += "  }\n";
        } else {
            block += "  " + attr_name + "\n";
        }
    }

    block += "}\n\n";
    return block;
}

std::string ErDiagramBuilder::MakeRelationshipBlock(ScAddr relNode)
{
    std::string name = context->GetElementSystemIdentifier(relNode);
    if (name.empty()) return "";

    std::string block = "relationship " + name;
    
    
    if (context->CheckConnector(Keynodes::nrel_identifying_relationship, relNode, ScType::ConstPermPosArc))
        block += " <<identifying>>";

    block += " {\n";

    
    for (ScAddr attr : GetAttributes(relNode)) {
        std::string attr_name = context->GetElementSystemIdentifier(attr);
        
        block += "  " + attr_name + "\n";
    }
    block += "}\n\n";
    return block;
}

void ErDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package)
{
    if (context->CheckConnector(Keynodes::concept_entity, Node, ScType::ConstPermPosArc))
    {
        if (usedNodes->find(Node) == usedNodes->end())
        {
            
            entities_ += MakeEntityBlock(Node);
            usedNodes->insert(Node);
        }
    }
}


void ErDiagramBuilder::ProcessAdjacentNodes(ScAddr Node, ScAddr package)
{
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
        
        if (!context->CheckConnector(Keynodes::concept_entity, entity2, ScType::ConstPermPosArc))
            continue;
        
        std::string e1_name = context->GetElementSystemIdentifier(Node);
        std::string e2_name = context->GetElementSystemIdentifier(entity2);
        std::string rel_name = context->GetElementSystemIdentifier(relNode);

        if (e1_name.empty() || e2_name.empty() || rel_name.empty())
            continue;
            
        if (usedRelationships.find(relNode) == usedRelationships.end()) {
            relationships_ += MakeRelationshipBlock(relNode);
            usedRelationships.insert(relNode);
        }

        std::string card = ChenCardinality(relNode); 
        
        std::string line1 = rel_name + " " + card + " " + e1_name + "\n";
        
        std::string line2 = rel_name + " " + card + " " + e2_name + "\n";

        
        if (e1_name < e2_name) {
             relations_ += line1;
             relations_ += line2;
        } else if (e1_name == e2_name) {
             relations_ += line1; 
        }
    }
}


// --- ФИНАЛЬНАЯ СБОРКА РЕЗУЛЬТАТА ---
std::string ErDiagramBuilder::GetResultString()
{
    std::string result = "@startchen\n";
    result += "left to right direction\n\n";

    result += relationships_;
    result += entities_;
    result += relations_;

    result += "\n@endchen";
    return result;
}