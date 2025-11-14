#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include <memory>
#include <string>
#include "StateDiagramBuilder.hpp"

StateDiagramBuilder::StateDiagramBuilder()
    : ParticularDiagramBuilder()
{
}

void StateDiagramBuilder::ProcessNode(ScAddr Node)
{
    if(usedNodes.find(Node)!=usedNodes.end()){
        if(Node==ScKeynodes::action&&map.find(Node)==map.end()){
            map[Node]="state "+context->GetElementSystemIdentifier(Node)+"{\n}\n";
        }
    }
   
}
std::string StateDiagramBuilder::ProcessCondition(ScAddr Condition){

}
void StateDiagramBuilder::ProcessEdge(ScAddr edge,std::shared_ptr<ScAddrToValueUnorderedMap<ScAddr>> sequnce){
    ScIterator5Ptr it5=context->CreateIterator5(ScType::Node, edge, 
        ScType::Node, ScType::CommonArc, ScType::NodeNonRole);
        ScIterator3Ptr it3=context->CreateIterator3(edge, ScType::CommonEdge, ScType::NodeStructure);
        while(it5->Next()){
            
        }

        if(it3->Next()){
            std::string condition=ProcessCondition(it3->Get(2));
        }
        
}
void StateDiagramBuilder::ProcessEdgesByNode(ScAddr Node)
{
    std::shared_ptr<ScAddrToValueUnorderedMap<ScAddr>> sequnce=make_shared<ScAddrToValueUnorderedMap<ScAddr>>();

    if(Node==ScKeynodes::action){
       ScIterator3Ptr it3=context->CreateIterator3(Node, ScType::CommonArc, ScType::Node);
       if(it3->Next()){
            ScAddr action=it3->Get(2);
            it3=context->CreateIterator3(action, ScType::CommonEdge, ScType::Node);
            while(it3->Next()){
                ProcessEdge(it3->Get(1),sequnce);
            }
       }     
    }
}

void StateDiagramBuilder::ProcessAdjacentNodes(ScAddr Node)
{
    // TODO: implement
}

void StateDiagramBuilder::ProcessUnusedEdges(ScAddr package, std::shared_ptr<ScAddrSet> edges)
{
    // TODO: implement
}
ScAddrVector StateDiagramBuilder::CaptureTuple(ScAddr structure){
    ScAddr tuple;
    ScAddrVector tuples;
    ScIterator3Ptr it3=context->CreateIterator3(structure, ScType::ConstCommonArc,ScType::NodeTuple);
    while (it3->Next()) {
        tuples.push_back(tuple=it3->Get(2));
    }
    if(tuples.empty()){
        m_logger->Error("did not captured tuple");
    }
    m_logger->Debug("capture tuples");
    return tuples;
};
std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetAllPackages(ScAddr diagram)
{   
    std::shared_ptr<ScAddrSet> packages=std::make_shared<ScAddrSet>();
    ScIterator3Ptr it3struct=context->CreateIterator3(diagram, ScType::ConstCommonArc,ScType::NodeStructure);
    ScAddrToValueUnorderedMap<ScAddrSet> actionsMap;
    ScIterator3Ptr it3;
    //сбор всех действий в одном пакете если структурa
    while (it3struct->Next()) {
        ScAddrSet actions;
        ScAddrVector tuple=CaptureTuple(it3struct->Get(2));
        if(tuple.empty())
            m_logger->Error("no tuple in struct:"+context->GetElementSystemIdentifier(it3struct->Get(2)));
        ScAddr action=context->ResolveElementSystemIdentifier("action");
         it3=context->CreateIterator3(it3struct->Get(2), ScType::ConstCommonArc, ScType::Node);
        
            while(it3->Next()){
                if(context->CheckConnector(action, it3->Get(2),ScType::CommonArc) &&
                    !context->CheckConnector( it3->Get(2),tuple[0], ScType::CommonEdge)){
                        m_logger->Debug("trying to capture package for action"+context->
                            GetElementSystemIdentifier(it3->Get(2)));
                    actions.insert(it3->Get(2));     
                }
            }
            
        actionsMap[it3struct->Get(2)] = actions;
    }


    //проверка вхождений действий из одного пакета в другой
    it3struct=context->CreateIterator3(diagram, ScType::ConstCommonArc,ScType::NodeStructure);
    while(it3struct->Next()){
        for(auto pair: actionsMap){
            if(pair.first!=it3struct->Get(2)){
                for(auto item:   actionsMap[pair.first]){
                    if(context->CheckConnector(it3struct->Get(2),item,ScType::CommonArc)){
                        packages->insert(it3struct->Get(2));
                        break;
                    }
                }
            }
        }
    }

    return packages;
}

std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetUsedNodes(ScAddr diagram)
{
    // TODO: implement
    return nullptr;
}

std::string StateDiagramBuilder::GetResultString()
{
    // TODO: implement
    return {};
}
