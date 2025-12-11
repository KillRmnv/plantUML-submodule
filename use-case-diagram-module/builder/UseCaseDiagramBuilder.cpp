#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include "UseCaseDiagramBuilder.hpp"
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include "../keynodes/Keynodes.hpp"

/// Delegates construction to the base PackageDiagramBuilder.
UseCaseDiagramBuilder::UseCaseDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : PackageDiagramBuilder( context, logger)
{
}

/// Iteratively removes known semantic prefixes ("nrel_", "rrel_", "concept_") 
/// to produce clean identifier names for the diagram.
std::string UseCaseDiagramBuilder::trim(const std::string &s)
{
    std::string result = s;

    const std::string prefixes[] = { "nrel_", "rrel_","concept_" };

    bool changed = true;

    while (changed)
    {
        changed = false;

        for (const auto &p : prefixes)
        {
            if (result.rfind(p, 0) == 0)  
            {
                result = result.substr(p.size());
                changed = true;
            }
        }
    }

    return result;
};

/// Removes leading and trailing whitespaces from the string.
std::string UseCaseDiagramBuilder::trim_spaces(std::string str)
{
    str.erase(str.find_last_not_of(' ') + 1);  
    str.erase(0, str.find_first_not_of(' '));  
    return str;
}

/// Finalizes the PlantUML package block. Appends the accumulated entities 
/// (actors/usecases) of the current scope to the global entity list 
/// and clears the buffer for the next package.
void UseCaseDiagramBuilder::ProcessPackage(ScAddr package) {
entities+="package "+trim(context->GetElementSystemIdentifier(package))+"{\n"+entitiesInCurrentPackage+
    "}\n";
    m_logger->Debug("package "+trim(context->GetElementSystemIdentifier(package))+"{\n"+entitiesInCurrentPackage+
    "}\n");
    entitiesInCurrentPackage="";
}

/// Validates if the node is a use case structure and not a nested package.
/// Resolves the system identifier to a human-readable name, registers it 
/// in the PlantUML buffer, and performs a level consistency check.
void UseCaseDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package)
{
            ScIterator5Ptr it5;
            if(context->GetElementType(Node)==ScType::ConstNodeStructure&&!context->
            CheckConnector(Keynodes::concept_er_package, Node, ScType::PosArc)){
                it5=context->CreateIterator5(ScType::NodeStructure, ScType::PosArc,Node,ScType::PosArc, package);
                bool check=true;
                while(it5->Next()){
                    if(it5->Get(0)!=package&&
                    context->CheckConnector(Keynodes::concept_er_package,it5->Get(0), ScType::PosArc)){
                        check=false;
                        break;
                    }
                   
                }
                
                if(check&&nameByStruct.find(Node)==nameByStruct.end()){
                    ScIterator3Ptr it3=context->CreateIterator3(Node, ScType::PosArc,ScType::Node);
                    while (it3->Next()) {
                            it5=context->CreateIterator5(ScType::ConstNode, ScType::ConstCommonArc, 
                                it3->Get(2), ScType::PosArc,ScType::ConstNodeNonRole);
                            while(it5->Next()){
                                if(context->CheckConnector(Node, it5->Get(1), ScType::PosArc)){
                                    nameByNode[it5->Get(2)]=trim(context->GetElementSystemIdentifier(it5->Get(4)));
                                    nameByStruct[Node]=nameByNode[it5->Get(2)];
                                    entitiesInCurrentPackage+="usecase "+nameByNode[it5->Get(2)] +"\n";
                                    bool check=false;
                                    for(auto pair:structuresByLevel){
                                        if(pair.second.find(addrMap[Node])!=pair.second.end()){
                                            if(check)
                                                throw 1;
                                            else
                                                check=true;
                                        }
                                    }
                                }
                            }
                    }
                }
            }
}

/// Analyzes the semantic relation type (extend, include, generalization, or actor association)
/// and formats the corresponding PlantUML arrow syntax (e.g., ..>, --|>, -->).
/// Ensures edges are not processed duplicates.
void UseCaseDiagramBuilder::ProcessEdge(ScAddr edge,ScAddr relation,ScAddr package){
    if(usedEdges->find(edge)==usedEdges->end()){
        ScIterator5Ptr it5=context->CreateIterator5(ScType::NodeStructure, ScType::PosArc, edge, ScType::PosArc, package);
        while (it5->Next()) {
            if (it5->Get(0)!=package&&
                context->CheckConnector(Keynodes::concept_er_package,it5->Get(0), ScType::PosArc)) {
                return;
            }
        }

        std::tuple<ScAddr,ScAddr> p=context->GetConnectorIncidentElements(edge);
        m_logger->Debug("Process edge by node: "+context->GetElementSystemIdentifier(get<0>(p))+
            " to node:"+context->GetElementSystemIdentifier(get<1>(p)));
            std::string addition;
        if(relation==Keynodes::nrel_extend_use_case){
            addition+=nameByStruct[get<0>(p)]+" ..> "+nameByStruct[get<1>(p)]+" : <<extend>>\n";
        }else if(relation==Keynodes::nrel_include_use_case){
            addition+=nameByStruct[get<0>(p)]+" ..> "+nameByStruct[get<1>(p)]+" : <<include>>\n";
        }else if(relation==Keynodes::nrel_generalization_use_case){
            m_logger->Debug("generalization use case:");
            ScIterator3Ptr it3=context->CreateIterator3(ScType::ConstNodeClass, ScType::PosArc, get<0>(p));
            bool check=false;
            while(it3->Next()){
                ScIterator5Ptr it5=context->CreateIterator5(Keynodes::concept_actor, ScType::CommonArc, 
                    it3->Get(0),ScType::PosArc, package);
                    if(it5->Next()){
                        check=true;
                            addition+=context->GetElementSystemIdentifier(get<0>(p))+" --|> "+
                            context->GetElementSystemIdentifier(get<1>(p))+"\n";
                            break;
                        }
            
        }
           if(!check){
            addition+=nameByStruct[get<0>(p)]+" --|> "+nameByStruct[get<1>(p)]+"\n";
           } 
        }else if(get<0>(p)==Keynodes::concept_actor &&relation==ScKeynodes::nrel_inclusion) {
            ScIterator3Ptr it3=context->CreateIterator3(get<1>(p), ScType::PosArc, ScType::ConstNode);
            while(it3->Next())
                entitiesInCurrentPackage+="actor "+context->GetElementSystemIdentifier(it3->Get(2))+"\n";
        }else{           
            if(actorsToActions.find(get<0>(p))==actorsToActions.end()||
            actorsToActions[get<0>(p)].find(relation)==actorsToActions[get<0>(p)].end()){
                ScIterator3Ptr it3=context->CreateIterator3(ScType::ConstNodeClass, ScType::PosArc, get<0>(p));
                    while(it3->Next()){
                        ScIterator5Ptr it5=context->CreateIterator5(Keynodes::concept_actor, ScType::CommonArc, 
                            it3->Get(0),ScType::PosArc, ScKeynodes::nrel_inclusion);
                        if(it5->Next()){
                            addition+=context->GetElementSystemIdentifier(get<0>(p))+" --> "+trim(context->GetElementSystemIdentifier(relation))+"\n";
                            actorsToActions[get<0>(p)].insert(relation);
                            m_logger->Debug("insert relation:"+context->GetElementSystemIdentifier(relation)+
                            " to action:"+context->GetElementSystemIdentifier(get<0>(p)));
                            break;
                        }
                    }
            }
            
        }
        usedEdges->insert(edge);
        relations+=addition;
    }
}

/// Intentionally empty. Edge processing is handled via adjacent node traversal 
/// to ensure semantic relations are captured correctly.
void UseCaseDiagramBuilder::ProcessEdgesByNode(ScAddr Node,ScAddr package)
{
    
}

/// Iterates through common arcs to find nodes semantically connected to the current Node.
/// Triggers recursive processing for discovered nodes and their connecting edges.
void UseCaseDiagramBuilder::ProcessAdjacentNodes(ScAddr Node,ScAddr package)
{
        ScIterator5Ptr it5=context->CreateIterator5(Node, ScType::CommonArc, 
            ScType::Node, ScType::PosArc, ScType::NodeNonRole);
        while(it5->Next()){

            if(context->CheckConnector(package, 
                        it5->Get(1), ScType::PosArc)){
                m_logger->Debug("processing :"+context->GetElementSystemIdentifier(Node)+" to "+
                context->GetElementSystemIdentifier(it5->Get(2)));
                ProcessNode(it5->Get(2),package);
                ProcessEdge(it5->Get(1),it5->Get(4),package);
            }
        }
        
    
 }

/// Scans the root diagram structure to identify first-level packages.
/// Initializes the internal level counter and builds the initial PlantUML preamble.
std::shared_ptr<ScAddrSet> UseCaseDiagramBuilder::GetAllPackages(ScAddr diagram)
{   
    level++;
    if(structuresByLevel.find(level)==structuresByLevel.end()){
        structuresByLevel.emplace(
            level,
            std::unordered_set<std::tuple<ScAddr,ScAddr,ScAddr>, ScAddrTripleHash, ScAddrTripleEq>()
        );
            }
    ScIterator5Ptr it5struct=context->CreateIterator5(Keynodes::concept_er_package, ScType::ConstPermPosArc,
        ScType::NodeStructure, ScType::ConstPermPosArc,diagram);
    ScIterator5Ptr it5;
    std::shared_ptr<ScAddrSet> packg=std::make_shared<ScAddrSet>();
    std::string addition="package "+trim(context->GetElementSystemIdentifier(diagram))+"{\n";
    m_logger->Debug("trying to capture packages for:"+context->GetElementSystemIdentifier(diagram)+" type:"+std::string(context->GetElementType(diagram)));
    while (it5struct->Next()) {
        it5=context->CreateIterator5(ScType::NodeStructure, ScType::PosArc, 
            it5struct->Get(2),ScType::PosArc,it5struct->Get(4));
        bool check=false;
        while(it5->Next()){

            if(it5->Get(0)!=diagram&& context->CheckConnector(Keynodes::concept_er_package, 
                it5->Get(0), ScType::PosArc)){
                check=true;
                break;
            }

        }
        if(!check){
           
            addition+="package "+trim(context->GetElementSystemIdentifier(it5struct->Get(2)))+"{\n}\n";
        }
    }
    packg->insert(diagram);
    addition+="}\n";
    preamble+=addition;
    m_logger->Debug(" packages size for:"+context->GetElementSystemIdentifier(diagram)+
    " size:"+to_string(packg->size()));

    return packg;
}

/// Returns the accumulated set of processed nodes.
std::shared_ptr<ScAddrSet> UseCaseDiagramBuilder::GetUsedNodes(ScAddr addr)
{
    return usedNodes;
}

/// Concatenates the preamble, package entities, current package buffer, and relations
/// into the final @startuml ... @enduml string.
std::string UseCaseDiagramBuilder::GetResultString()
{
    return "@startuml\n"+preamble+entities+entitiesInCurrentPackage+relations+"\n@enduml";
}

/// Checks if a package contains nested `concept_er_package` structures.
/// If it is a leaf package, it maps the structure contents to the current hierarchy level.
/// Returns true if recursion is needed, false otherwise.
bool UseCaseDiagramBuilder::PackageCheck(ScAddr package,ScAddr parent) {
    if(!context->CheckConnector(Keynodes::concept_er_package, package,ScType::PosArc)){
        ScIterator5Ptr it5=context->CreateIterator5(ScType::NodeStructure, ScType::PosArc, 
            package,ScType::PosArc,parent);
            bool check=false;
            while(it5->Next()){
        
                if(it5->Get(0)!=parent&& context->CheckConnector(Keynodes::concept_er_package, 
                    it5->Get(0), ScType::PosArc)&&
                    context->CheckConnector(parent, 
                        it5->Get(0), ScType::PosArc)){
                    check=true;
                    break;
                }
        
            }
            if(!check){
                std::tuple<ScAddr> elements;
                m_logger->Debug("Structure "+context->GetElementSystemIdentifier(package)+" on level "+to_string(level));
                it5=context->CreateIterator5(ScType::ConstNode, ScType::CommonArc, ScType::Node, ScType::ConstPermPosArc, package);
                while (it5->Next()) {
                    ScIterator3Ptr it3=context->CreateIterator3(ScType::ConstNodeNonRole, ScType::ConstPermPosArc,it5->Get(1) );
                    while(it3->Next()){
                        structuresByLevel[level].insert(std::make_tuple(it5->Get(0),it5->Get(2),it3->Get(0)));
                        addrMap[package]=std::make_tuple(it5->Get(0),it5->Get(2),it3->Get(0));
                        return false;
                    }
                } 
            }
        return false;
    }
     m_logger->Debug("trying to capture packages in package check for "+context->GetElementSystemIdentifier(parent));

    ScIterator5Ptr it5=context->CreateIterator5(ScType::NodeStructure, ScType::PosArc, 
        package,ScType::PosArc,parent);
    bool check=false;
    while(it5->Next()){

        if(it5->Get(0)!=parent&& context->CheckConnector(Keynodes::concept_er_package, 
            it5->Get(0), ScType::PosArc)){
            check=true;
            break;
        }

    }
    if(!check)
        m_logger->Debug("successfully checked:"+context->GetElementSystemIdentifier(package));
    return !check;


}