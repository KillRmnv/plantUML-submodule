#include "ErDiagramBuilder.hpp"
#include <utility>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include "keynodes/Keynodes.hpp"
#include <unordered_map>
#include <string>

ErDiagramBuilder::ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : BaseDiagramBuilder(context, logger)
{
    m_logger->Debug("ErDiagramBuilder: Initialized.");
    entities_ = "";
    relationships_ = "";
    relations_ = "";
}

ScAddrVector ErDiagramBuilder::GetClassMembers(ScAddr class_node)
{
    ScAddrVector items;
    auto it = context->CreateIterator3( 
        class_node,
        ScType::ConstPermPosArc,
        ScType::ConstNode
    );
    while (it->Next())
        items.push_back(it->Get(2));

    m_logger->Debug("GetClassMembers: Found " + std::to_string(items.size()) + " members.");

    return items;
}

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

std::pair<std::string,std::string> ErDiagramBuilder::ChenCardinality(ScAddr classNode)
{
    std::string symbol="-";
    //TODO:add check connector on strong relation change from - to =
    std::pair<std::string,std::string> card = std::make_pair("---","---");

    if(Keynodes::concept_one_or_many==classNode){
    


        if(context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(1,N)"+symbol, "");
        }else{
            card = make_pair("",symbol+"(1,N)"+symbol);
        }
    }
    else if(Keynodes::concept_zero_or_one==classNode){
        if(context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(0,1)"+symbol, "");
        }else{
            card = make_pair("",symbol+"(0,1)"+symbol);
        }
    }
    else if(Keynodes::concept_zero_or_many==classNode){
        if(context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"(0,N)"+symbol, "");
        }else{
            card = make_pair("",symbol+"(0,N)"+symbol);
        }
    }else if(Keynodes::concept_one==classNode){
        if(context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"1"+symbol, "");
        }else{
            card = make_pair("",symbol+"1"+symbol);
        }
    }else if(Keynodes::concept_many==classNode){
        if(context->CheckConnector(Keynodes::concept_first_domain, classNode, ScType::PosArc)){
            card = make_pair(symbol+"N"+symbol, "");
        }else{
            card = make_pair("",symbol+"N"+symbol);
        }
    }

    return card;
}

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
    //TODO: make recursion of this cycle
    for (ScAddr attr : GetAttributes(entity,package)) {
        std::string attr_name = context->GetElementSystemIdentifier(attr); 

        if (context->CheckConnector(Keynodes::concept_key_attribute, 
                                    attr,
                                    ScType::ConstPermPosArc))
            attr_name += " <<key>>";

        if (IsOptional(attr))
            attr_name += "<<derived>>";

        auto children = GetChildAttributes(attr,package);
        if (!children.empty()) {
            block += "  " + attr_name + " {\n";
            for (ScAddr child : children) {
                std::string child_name = context->GetElementSystemIdentifier(child); 

                if (context->CheckConnector(Keynodes::concept_key_attribute, 
                                            child,
                                            ScType::ConstPermPosArc))
                    child_name += " <<key>>";

                if (IsOptional(child))
                    child_name += "<<derived>>";

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

std::string ErDiagramBuilder::MakeRelationshipBlock(ScAddr relNode,ScAddr package)
{
    std::string name = context->GetElementSystemIdentifier(relNode);
    if (name.empty()) return "";

    m_logger->Debug("MakeRelationshipBlock: Generating block for " + name);
    
    std::string block = "relationship " + name;
    
    
    if (context->CheckConnector(Keynodes::nrel_identifying_relationship, relNode, ScType::ConstPermPosArc))
        block += " <<identifying>>";

    block += " {\n";

    
    for (ScAddr attr : GetAttributes(relNode,package)) {
        std::string attr_name = context->GetElementSystemIdentifier(attr);
        if (context->CheckConnector(Keynodes::concept_key_attribute, 
            attr,
            ScType::ConstPermPosArc))
            attr_name += " <<key>>";

        if (IsOptional(attr))
            attr_name += "<<derived>>";

        block += "  " + attr_name + "\n";
    }
    block += "}\n\n";
    return block;
}

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
            
            if (!context->CheckConnector(Keynodes::concept_entity, entity2, ScType::ConstPermPosArc))
                continue;
            
            std::string e1_name = context->GetElementSystemIdentifier(Node);
            std::string e2_name = context->GetElementSystemIdentifier(entity2);
            std::string rel_name = context->GetElementSystemIdentifier(relNode);

            m_logger->Debug("ProcessAdjacentNodes: Found Arc. Relation: " + rel_name + " -> Target: " + e2_name);

            if (e1_name.empty() || e2_name.empty() || rel_name.empty()){
                m_logger->Debug("ProcessAdjacentNodes: One of identifiers is empty. Skipping.");
                continue;
            }
                
        
                m_logger->Debug("ProcessAdjacentNodes: New Relationship found: " + rel_name + ". Making block.");
            if(usedRelationships.find(relNode)==usedRelationships.end()){
                    relationships_ += MakeRelationshipBlock(relNode,package);
                    usedRelationships.insert(relNode);
            }
            ScIterator5Ptr it5=context->CreateIterator5(ScType::ConstNodeClass, ScType::PosArc, relNode, ScType::PosArc, package);
            while (it5->Next()) {
                std::pair<std::string,std::string> card = ChenCardinality(it5->Get(0));
            if(!card.first.empty()&&entityToRelation.find(Node)==entityToRelation.end()){
                entityToRelation[Node]=relNode;
                relations_+= e1_name + " " + card.first + " " + rel_name + "\n";
            }
            if(!card.second.empty())
            relations_+= rel_name + " " + card.second + " " + e2_name + "\n";
            }
            
            

        }
    }
}


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