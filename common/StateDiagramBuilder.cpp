#include <string>
#include <utility>

#include "StateDiagramBuilder.hpp"
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_type.hpp>
#include "keynodes/Keynodes.hpp"

StateDiagramBuilder::StateDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : ParticularDiagramBuilder( context, logger)
{
    priorities[Keynodes::nrel_priority_path]=4;
    priorities[Keynodes::nrel_then]=3;
    priorities[Keynodes::nrel_else]=2;
    priorities[Keynodes::nrel_goto]=1;
    descriptionByType = {

    
    { ScType::ConstConnector, "connects" },
    { ScType::VarConnector, "connects" },


    { ScType::ConstPermPosArc, "is an element of" },
    { ScType::VarCommonArc, "is an element of" },

    { ScType::ConstMembershipArc, "is an membership of" },
    { ScType::VarMembershipArc, "is an membership of" },


    { ScType::ConstPermArc, "is an element of" },
    { ScType::VarPermArc, "is an element of" },

    { ScType::ConstTempArc, "is an temporary element of" },
    { ScType::VarTempArc, "is an temporary element of" },

    // --- Positive arcs ---
    { ScType::ConstPosArc, "is an element of" },
    { ScType::VarPosArc, "is an element of" },

    { ScType::PermPosArc, "is an permanent element of" },
    { ScType::TempPosArc, "is an temporary element of" },

    { ScType::ConstPermPosArc, "is an permanent element of" },
    { ScType::ConstTempPosArc, "is an temporary element of" },

    { ScType::VarPermPosArc, "Variable permanent positive arc" },
    { ScType::VarTempPosArc, "Variable temporary positive arc" },
    { ScType::VarActualTempPosArc, "Variable actual temporary positive arc" },
    { ScType::VarInactualTempPosArc, "Variable inactual temporary positive arc" },

    // --- Negative arcs ---
    { ScType::ConstNegArc, "Constant negative arc" },
    { ScType::VarNegArc, "Variable negative arc" },

    { ScType::PermNegArc, "Permanent negative arc" },
    { ScType::TempNegArc, "Temporary negative arc" },
    { ScType::ActualTempNegArc, "Actual temporary negative arc" },
    { ScType::InactualTempNegArc, "Inactual temporary negative arc" },

    { ScType::ConstPermNegArc, "Constant permanent negative arc" },
    { ScType::ConstTempNegArc, "Constant temporary negative arc" },
    { ScType::ConstActualTempNegArc, "Constant actual temporary negative arc" },
    { ScType::ConstInactualTempNegArc, "Constant inactual temporary negative arc" },

    { ScType::VarPermNegArc, "Variable permanent negative arc" },
    { ScType::VarTempNegArc, "Variable temporary negative arc" },
    { ScType::VarActualTempNegArc, "Variable actual temporary negative arc" },
    { ScType::VarInactualTempNegArc, "Variable inactual temporary negative arc" },

    // --- Fuzzy arcs ---
    { ScType::ConstFuzArc, "Constant fuzzy arc" },
    { ScType::VarFuzArc, "Variable fuzzy arc" }
    

    };

}
std::string StateDiagramBuilder::AddEntitiesAndTransitions(ScAddrVector comb,ScAddr Node,std::string condition)
{
    std::string combStr;
    ScIterator3Ptr it3=context->CreateIterator3(Node, ScType::CommonArc, comb[0]);
    combStr+=nodes[comb[0]]+"\n";
    if(it3->Next()){
        relations+=condition+" --> "+conditionMap[it3->Get(1)].first;
        combStr+=conditionMap[it3->Get(1)].first +" --> "+nodes[comb[0]]+":"+conditionMap[it3->Get(1)].second;
        condition=conditionMap[it3->Get(1)].first;
    }
    
    for(int i=1;i<comb.size();i++){
        it3=context->CreateIterator3(ScType::Node, ScType::CommonArc, comb[i]);
        combStr+=nodes[comb[i]]+"\n";
        if(it3->Next()){
            relations+=condition+" --> "+conditionMap[it3->Get(1)].first;
            combStr+=conditionMap[it3->Get(1)].first+" --> "+nodes[comb[i]]+":"+conditionMap[it3->Get(1)].second;
            condition=conditionMap[it3->Get(1)].first;
    }
 
}
return combStr;
}

std::string trim(const std::string &s)
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
std::string StateDiagramBuilder::GetNodeName(ScAddr node,bool destination){
    std::string result;
    ScType type=context->GetElementType(node);
    if(type==ScType::NodeTuple){
        ScIterator3Ptr it3=context->CreateIterator3(node, ScType::Arc, ScType::Node);
        while(it3->Next()){
            result+=GetNodeName(it3->Get(2),destination)+ " and ";
        }
        result.substr(0,result.size()-4);
        if(!destination)
        result+="form ";
    }else if(type==ScType::NodeClass){
        result="class "+trim(context->GetElementSystemIdentifier(node));
    }else if(type==ScType::NodeLink){
        result="content "+context->GetElementSystemIdentifier(node);
    }else{
        result=context->GetElementSystemIdentifier(node);
    }
    return result;
}
//TODO:check SysIdentif first
std::pair<std::string,std::string> StateDiagramBuilder::ProcessCondition(ScAddr Condition,ScAddr Node){
    std::string sysIdentifier=context->GetElementSystemIdentifier(Condition);
    std::pair<std::string,std::string> result;
    result.second="";
    result.first=nodes[Node];

    if(!sysIdentifier.empty()){
        entities+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
        result.first="choice"+to_string(conditionCounter);
        conditionCounter++;
        result.second=sysIdentifier;
        return result;
    }
    ScIterator3Ptr it3=context->CreateIterator3(Condition, ScType::ConstPermPosArc, ScType::CommonArc);
    ScIterator5Ptr it5internal;
    ScIterator3Ptr it3internal;
    std::string description;
    while(it3->Next()){
        std::tuple<ScAddr, ScAddr> source=context->GetConnectorIncidentElements(it3->Get(2));
        it5internal=context->CreateIterator5(get<0>(source), ScType::CommonArc,
        get<1>(source), ScType::CommonArc, ScType::NodeNonRole);
        while(it5internal->Next()){
            description+=GetNodeName(it5internal->Get(0), false)+" "+
            trim(context->GetElementSystemIdentifier(it5internal->Get(4)))+" "+
            GetNodeName(it5internal->Get(2), true)+"\n";
        }
    }
    it3=context->CreateIterator3(Condition, ScType::ConstPermPosArc, ScType::CommonArc);
    while(it3->Next()){

        it3internal=context->CreateIterator3(ScType::NodeClass, it3->Get(2), ScType::Node);
        while(it3internal->Next()){
            description+=GetNodeName(it3internal->Get(2), true)+" "+ 
            descriptionByType[context->GetElementType(it3internal->Get(1))]+" "+
            trim(context->GetElementSystemIdentifier(it3internal->Get(0)))+"\n";
        }

    }
    if(!description.empty()){
        entities+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
        result.first="choice"+to_string(conditionCounter);
        conditionCounter++;
        result.second=description;
    }

    return result;

}

void StateDiagramBuilder::ProcessPackage(ScAddr package) {
    ScIterator3Ptr it3=context->CreateIterator3(package, ScType::ConstPermPosArc,ScType::NodeTuple);
    ScIterator5Ptr it5internal;
    while (it3->Next()) {
        it5internal=context->CreateIterator5(ScType::Node, ScType::CommonArc, it3->Get(2),ScType::PosArc,ScType::NodeNonRole);
        while (it5internal->Next()) {
            entities="state "+context->GetElementSystemIdentifier(it5internal->Get(0))+"{\n"+entities+"}\n";
            return;
        }
    }
}

std::vector<ScAddrVector> StateDiagramBuilder::FindSequence(
    std::vector<std::pair<ScAddr, int>> entries,
    ScAddr package)
{
    std::vector<ScAddrVector> result;

    // // ---------------------------
    // // 1. Собрать все дуги в граф
    // // ---------------------------

    std::unordered_map<ScAddr, ScAddrVector, ScAddrHashFunc> graph;

    for(auto entry:entries){
        std::tuple<ScAddr,ScAddr> p=context->GetConnectorIncidentElements(entry.first);
        m_logger->Debug("node graph "+context->GetElementSystemIdentifier(get<0>(p)));

        ScAddr source = get<1>(p);
        ScIterator5Ptr it5=context->CreateIterator5(source, ScType::CommonArc,ScType::Node, ScType::PosArc, Keynodes::nrel_basic_sequence);
        while(it5->Next()){
            if(context->CheckConnector(ScKeynodes::action,it5->Get(2), 
                ScType::PermPosArc)&&context->CheckConnector(package,it5->Get(2), 
                    ScType::PermPosArc))
            graph[source].push_back(it5->Get(2));
        }
        
    }
    
    m_logger->Debug("form graph");

    // ---------------------------
    // 2. DFS для поиска всех путей
    // ---------------------------

    std::function<void(ScAddr, ScAddrVector&, std::unordered_set<ScAddr, ScAddrHashFunc>&)> dfs;

    dfs = [&](ScAddr node, ScAddrVector &path, 
              std::unordered_set<ScAddr, ScAddrHashFunc> &visited)
    {
        visited.insert(node);
        path.push_back(node);

        // если у узла нет исходящих дуг — конец пути
        if (!graph.count(node)) {
            result.push_back(path);
            visited.erase(node);
            path.pop_back();
            return;
        }
        m_logger->Debug("dfs for adjacent nodes:"+context->GetElementSystemIdentifier(node)+":");
        for (ScAddr next : graph[node]) {
            m_logger->Debug(context->GetElementSystemIdentifier(next));
            if (!visited.count(next)) {
                dfs(next, path, visited);
            }
        }

        // конец поиска из этого узла
        visited.erase(node);
        path.pop_back();
    };

    // ---------------------------
    // 3. Запускаем DFS из каждого entry
    // ---------------------------
    for (auto &e : entries) {
        std::tuple<ScAddr,ScAddr> p=context->GetConnectorIncidentElements(e.first);

        m_logger->Debug("dfs for "+context->GetElementSystemIdentifier(get<0>(p)));

        ScAddr start = get<1>(p);

        ScAddrVector path;
        std::unordered_set<ScAddr, ScAddrHashFunc> visited;

        dfs(start, path, visited);
    }
    m_logger->Debug("dfs finished");


    return result;
}
void StateDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package)
{
    if(usedNodes->find(Node)==usedNodes->end() && 
    context->CheckConnector(ScKeynodes::action, Node, ScType::PermPosArc)&&
    nodes.find(Node)==nodes.end()){
        ScIterator5Ptr it5=context->CreateIterator5(Node, ScType::CommonArc,ScType::NodeTuple,ScType::PosArc,
             Keynodes::nrel_decomposition_of_action);
        while(it5->Next()){
            if(context->CheckConnector(package, it5->Get(2), ScType::PermPosArc)){
                return;
            }
        }

        nodes[Node]=context->GetElementSystemIdentifier(Node); 
        usedNodes->insert(Node);
        entities+="state "+context->GetElementSystemIdentifier(Node)+"{\n}\n";
    }
   
}

std::vector<std::pair<ScAddr, int>> StateDiagramBuilder::FindEntryPoints(ScAddr action,ScAddr package)
{
    ScIterator5Ptr it5 = context->CreateIterator5(
        action,
        ScType::CommonArc,
        ScType::Node,
        ScType::PosArc,
        ScType::NodeNonRole
    );

    std::vector<std::pair<ScAddr, int>> result;

    while (it5->Next()) 
    {
        if(context->CheckConnector(package, it5->Get(1), ScType::PermPosArc)&&
        context->CheckConnector(ScKeynodes::action, it5->Get(2), ScType::PermPosArc)){
        ScAddr entry = it5->Get(1);
        ScAddr rel   = it5->Get(4);

        result.emplace_back(entry, priorities[rel]);
        }
    }

    std::sort(result.begin(), result.end(),
              [](auto &a, auto &b) {
                  return a.second > b.second; 
              });

    return result;
}

std::vector<std::vector<int>> StateDiagramBuilder::FormEqualPrioritiesSequences(
    std::vector<ScAddrVector> sequences,
    std::vector<std::pair<ScAddr, int>> entries)
{
    // 1. Преобразуем entries в удобный map: ScAddr -> priority
    std::unordered_map<ScAddr, int, ScAddrHashFunc> priorityByAddr;
    priorityByAddr.reserve(entries.size());

    for (auto &p : entries) {
        priorityByAddr[p.first] = p.second;
    }

    // 2. Группировка: priority -> vector of sequence indices
    // map по убыванию ключа (приоритета)
    std::map<int, std::vector<int>, std::greater<int>> groups;

    for (int i = 0; i < static_cast<int>(sequences.size()); i++) {
        const auto &seq = sequences[i];

        if (seq.empty())
            continue;

        ScAddr first = seq[0];

        int pr = 0;
        if (priorityByAddr.count(first))
            pr = priorityByAddr[first];

        groups[pr].push_back(i);
    }

    // 3. Переносим группы в результат
    std::vector<std::vector<int>> result;
    result.reserve(groups.size());

    for (auto &g : groups) {
        result.push_back(g.second);
    }

    // // 4. Сортировка групп по приоритету первого элемента каждой группы
    // //    (он уже отсортирован map-ом по убыванию ключа, но проверим ещё раз)
    // std::sort(result.begin(), result.end(),
    //     [&](const std::vector<int> &a, const std::vector<int> &b)
    //     {
    //         if (a.empty() || b.empty())
    //             return a.size() > b.size(); 

    //         // первый элемент каждой группы → индекс последовательности
    //         int idxA = a[0];
    //         int idxB = b[0];

    //         // первый ScAddr в соответствующей последовательности
    //         ScAddr addrA = sequences[idxA][0];
    //         ScAddr addrB = sequences[idxB][0];

    //         int prA = priorityByAddr.count(addrA) ? priorityByAddr[addrA] : 0;
    //         int prB = priorityByAddr.count(addrB) ? priorityByAddr[addrB] : 0;

    //         return prA > prB;
    //     }
    // );

    return result;
}

void StateDiagramBuilder::ProcessEdge(ScAddr edge,ScAddr Node){
    m_logger->Debug("Process edge by node: "+context->GetElementSystemIdentifier(Node));
    std::pair<std::string,std::string> condition;
    condition.second="";
    condition.first=nodes[Node];
    conditionMap[Node]=condition;
        ScIterator3Ptr it3=context->
        CreateIterator3(edge, ScType::CommonArc, ScType::NodeStructure);
        if(it3->Next() ){
            condition=ProcessCondition(it3->Get(2),Node);            
             if(condition.first!=nodes[Node])
             relations+=condition.first+ " --> "+nodes[Node] +" :satisfy"+condition.second+"\n";
        }
}
void StateDiagramBuilder::ProcessEdgesByNode(ScAddr Node,ScAddr package)
{
    if(context->CheckConnector(ScKeynodes::action,Node, 
        ScType::PermPosArc)){
            ScIterator3Ptr it3 =context->CreateIterator3(ScType::Node, ScType::CommonArc, Node);
            m_logger->Debug("ProcessEdgesByNode:"+context->GetElementSystemIdentifier(Node));
            while(it3->Next()&& 
                context->CheckConnector(package, 
                it3->Get(1), ScType::PermPosArc)&&
                usedEdges->find(it3->Get(1))==usedEdges->end()&&
                context->CheckConnector(ScKeynodes::action, 
                    it3->Get(0), ScType::PermPosArc))
                {
                ProcessEdge(it3->Get(1),Node);
                usedEdges->insert(it3->Get(1)); 
                }
             
    }
}

void StateDiagramBuilder::ProcessAdjacentNodes(ScAddr Node,ScAddr package)
{
    if(context->CheckConnector(ScKeynodes::action, Node,ScType::PermPosArc)){
        ScIterator5Ptr it5=context->CreateIterator5(Node, ScType::CommonArc, ScType::NodeTuple,ScType::PosArc,Keynodes::nrel_decomposition_of_action);
        while(it5->Next()){
            if(context->CheckConnector(package, it5->Get(2), ScType::ConstPosArc)){
                return;
            }
        }
         it5=context->CreateIterator5(Node, ScType::CommonArc, 
            ScType::Node, ScType::PosArc, ScType::NodeNonRole);
            m_logger->Debug("processing :"+context->GetElementSystemIdentifier(Node));
        while(it5->Next() && context->CheckConnector(package, 
            it5->Get(2), ScType::PosArc)&& context->CheckConnector(ScKeynodes::action, 
                it5->Get(2), ScType::PosArc)){
            m_logger->Debug("processing edges to node:"+context->GetElementSystemIdentifier(it5->Get(2)));
            ProcessNode(it5->Get(2));
            m_logger->Debug("processed node:"+nodes[it5->Get(2)]);
            ProcessEdgesByNode(it5->Get(2),  package);
            m_logger->Debug("conditionMap:"+conditionMap[it5->Get(2)].first);
        }
        m_logger->Debug("trying to find entries:"+context->GetElementSystemIdentifier(Node));
        std::vector<std::pair<ScAddr, int>> entries=FindEntryPoints(Node,package);
        for(auto p:entries){
            m_logger->Debug("entry:"+context->GetElementSystemIdentifier(p.first)+" value:"+to_string(p.second));
        }
        std::vector<ScAddrVector> sequences;

        sequences=FindSequence(entries,package);

        for(int i=0;i<sequences.size();i++){
            m_logger->Debug("sequence:"+to_string(i));
            for(int j=0;j<sequences[i].size();j++){
                m_logger->Debug(context->GetElementSystemIdentifier(sequences[i][j]));
            }
        }

        std::vector<std::vector<int>> equalSequences=FormEqualPrioritiesSequences(sequences,entries);

        for(int i=0;i<equalSequences.size();i++){
            m_logger->Debug("equal sequences:"+to_string(i));
            for(int j=0;j<equalSequences[i].size();j++){
                m_logger->Debug(to_string(equalSequences[i][j]));
            }
        }
        std::string condition;
        for (int sequence=0;sequence<equalSequences.size();sequence++){
            m_logger->Debug("processing equal sequence "+to_string(sequence));
            if(equalSequences[sequence].size()>1){
                
                std::vector<ScAddrVector> equalSequence;
                int ind=equalSequences[sequence][0]-1;
                
                if(ind!=-1 ){
                    entities+="state Path"+to_string(conditionCounter) +" <<choice>>\n";
                    condition="Path"+to_string(conditionCounter);
                    conditionCounter++;
                    relations+=nodes[Node]+" --> "+condition;
                }else{
                    entities+="state Path"+to_string(conditionCounter) +" <<choice>>\n";
                    relations+=condition+" --> "+"Path"+to_string(conditionCounter);;
                    condition="Path"+to_string(conditionCounter);
                    conditionCounter++;
                }
                
                for(int index=0;index< equalSequences[sequence].size();index++){
                    equalSequence.push_back(sequences[ equalSequences[sequence][index]]);
                }
                for(auto equal:equalSequence){
                    std::string combStr="state PossiblePath"+to_string(conditionCounter)+"{\n"+AddEntitiesAndTransitions( equal,Node,condition)+"\n}\n";
                    relations+=condition +" --> "+"PossiblePath"+to_string(conditionCounter)+"\n";//TODO"instead of possible path use first node in comb with 
                    conditionCounter++;
                    entities+=combStr;    
                }
                
            }else{
                if(equalSequences[sequence][0]-1==-1 ){
                    m_logger->Debug("equalSequences[sequence][0]:"+to_string(equalSequences[sequence][0])+"\n"+
                    context->GetElementSystemIdentifier(sequences[equalSequences[sequence][0]][0]));

                    relations+=nodes[Node]+" --> "+conditionMap[sequences[equalSequences[sequence][0]][0]].first+" :"+
                    conditionMap[sequences[equalSequences[sequence][0]][0]].second;
                    condition=conditionMap[sequences[equalSequences[sequence][0]][0]].first;
                }else{
                    m_logger->Debug("equalSequences[sequence][0]:"+to_string(equalSequences[sequence][0])+"\n"+
                    context->GetElementSystemIdentifier(sequences[equalSequences[sequence][0]][0]));
                    
                    relations+=condition+" --> "+conditionMap[sequences[equalSequences[sequence][0]][0]].first+" :"+
                    conditionMap[sequences[equalSequences[sequence][0]][0]].second;
                    condition=conditionMap[sequences[equalSequences[sequence][0]][0]].first;
                }
            }
        }
        m_logger->Debug("result for:"+context->GetElementSystemIdentifier(Node)+":"+entities+relations);
    }
}

void StateDiagramBuilder::ProcessUnusedEdges(ScAddr package, std::shared_ptr<ScAddrSet> edges)
{

}
ScAddrVector StateDiagramBuilder::CaptureTuple(ScAddr structure){
    ScAddr tuple;
    ScAddrVector tuples;
    ScIterator3Ptr it3=context->CreateIterator3(structure, ScType::ConstPermPosArc,ScType::NodeTuple);
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
    ScIterator3Ptr it3struct=context->CreateIterator3(diagram, ScType::ConstPermPosArc,ScType::ConstNodeStructure);
    ScAddrToValueUnorderedMap<ScAddrSet> actionsMap;
    ScIterator3Ptr it3;
    m_logger->Debug("trying to capture packages for:"+context->GetElementSystemIdentifier(diagram)+" type:"+std::string(context->GetElementType(diagram)));
    //сбор всех действий в одном пакете если структурa
    while (it3struct->Next()) {
        ScAddrSet actions;
        ScAddrVector tuple=CaptureTuple(it3struct->Get(2));
        if(tuple.empty())
            m_logger->Error("no tuple in struct:"+context->GetElementSystemIdentifier(it3struct->Get(2)));
        ScAddr action=context->ResolveElementSystemIdentifier("action");
         it3=context->CreateIterator3(it3struct->Get(2), ScType::ConstPermPosArc, ScType::Node);
        
            while(it3->Next()){
                if(context->CheckConnector(action, it3->Get(2),ScType::PosArc) &&
                    !context->CheckConnector( it3->Get(2),tuple[0], ScType::CommonArc)){
                        m_logger->Debug("trying to capture package for action"+context->
                            GetElementSystemIdentifier(it3->Get(2)));
                    actions.insert(it3->Get(2));     
                }
            }
            
        actionsMap[it3struct->Get(2)] = actions;
    }


    //проверка вхождений действий из одного пакета в другой
    it3struct=context->CreateIterator3(diagram, ScType::ConstPermPosArc,ScType::NodeStructure);
    while(it3struct->Next()){
        for(auto pair: actionsMap){
            if(pair.first!=it3struct->Get(2)){
                for(auto item:   actionsMap[pair.first]){
                    if(context->CheckConnector(it3struct->Get(2),item,ScType::PosArc)){
                        packages->insert(it3struct->Get(2));
                        packages->insert(pair.first);
                        break;
                    }
                }
            }
        }
    }
    this->packages=packages;
    return packages;
}
std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetUsedNodes(ScAddr addr)
{
    return usedNodes;
}
std::string StateDiagramBuilder::GetResultString()
{
    return entities+relations;
}