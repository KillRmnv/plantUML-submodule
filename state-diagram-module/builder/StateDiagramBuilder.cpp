#include <cstddef>
#include <memory>
#include <sc-builder/scs_loader.hpp>
#include <string>
#include <unordered_map>
#include <utility>

#include "StateDiagramBuilder.hpp"
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_debug.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_type.hpp>
#include "keynodes/Keynodes.hpp"

StateDiagramBuilder::StateDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
    : PackageDiagramBuilder( context, logger)
{
    /// Инициализация весов отношений для определения порядка обхода переходов.
    /// Чем выше значение, тем выше приоритет ветви при генерации.
    priorities[Keynodes::nrel_priority_path]=4;
    priorities[Keynodes::nrel_then]=3;
    priorities[Keynodes::nrel_else]=2;
    priorities[Keynodes::nrel_goto]=1;
    packages=std::make_shared<ScAddrSet>();
    
}

//TODO:refactor and delete regular states from package
std::string StateDiagramBuilder::AddEntitiesAndTransitions(ScAddrVector comb,ScAddr Node,std::string condition,int num)
{
    std::string combStr;
    auto pair=context->GetConnectorIncidentElements(comb[0]);
    m_logger->Debug("comb size:"+to_string(comb.size()));
    std::string nodeInPosssiblePath=nodes[get<1>(pair)].front()+"_pp"+to_string(num);
    nodes[get<1>(pair)].push_back(nodeInPosssiblePath);
    combStr+="state "+nodeInPosssiblePath+"{\n}\n";
        m_logger->Debug("map:"+conditionMap[comb[0]].first+" nodes:"+nodeInPosssiblePath);
        if(conditionMap[comb[0]].first==nodes[get<1>(pair)].front()){
            combStr+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
            std::string addition=condition+" --> "+"choice"+to_string(conditionCounter);
            if(!conditionMap[comb[0]].second.empty())
                        addition+=":"+conditionMap[comb[0]].second+"\n";
            else
                addition+="\n";
            condition="choice"+to_string(conditionCounter);
            combStr+=condition+" --> "+nodeInPosssiblePath+"\n";
            relations+=addition;
            conditionCounter++;

        }else{

            combStr+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
            std::string addition=condition+" --> "+"choice"+to_string(conditionCounter);
            if(!conditionMap[comb[0]] .second.empty())
                        addition+=":"+conditionMap[comb[0]] .second+"\n";
            else
                addition+="\n";
            condition="choice"+to_string(conditionCounter);
            combStr+=condition+" --> "+nodeInPosssiblePath+"\n";

            relations+=addition;
            conditionCounter++;
        }
    
    // Обработка остальных элементов комбинации
    for(int i=1;i<comb.size();i++){
        pair=context->GetConnectorIncidentElements(comb[i]);
        nodeInPosssiblePath=nodes[get<1>(pair)].front()+"_pp"+to_string(num);
        nodes[get<1>(pair)].push_back(nodeInPosssiblePath);
        combStr+="state "+nodeInPosssiblePath+"{\n}\n";
        m_logger->Debug("map:"+conditionMap[comb[i]] .first+" nodes:"+nodeInPosssiblePath);

        if(conditionMap[comb[i]] .first==nodes[get<1>(pair)].front()){
            combStr+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
            std::string addition=condition+" --> "+"choice"+to_string(conditionCounter);
            if(!conditionMap[comb[i]] .second.empty())
                        addition+=":"+conditionMap[comb[i]] .second+"\n";
            else
                addition+="\n";
            condition="choice"+to_string(conditionCounter);
            combStr+=addition;
            combStr+=condition+" --> "+nodeInPosssiblePath+"\n";

            conditionCounter++;


        }else{
            combStr+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
            std::string addition=condition+" --> "+conditionMap[comb[i]] .first;
            if(!conditionMap[comb[i]] .second.empty())
                        addition+=":"+conditionMap[comb[i]] .second+"\n";
            else
                addition+="\n";
            condition="choice"+to_string(conditionCounter);;
            combStr+=addition;
            combStr+=condition+" --> "+nodeInPosssiblePath+"\n";
            conditionCounter++;
        }
    }
return combStr;
}

/// @details Утилитарная функция для обрезки системных префиксов (nrel_, rrel_ и т.д.)
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

/// @details Генерирует уникальное имя для состояния выбора (choice) на основе счетчика,
/// если обнаружен системный идентификатор условия.
std::pair<std::string,std::string> StateDiagramBuilder::ProcessCondition(ScAddr Condition,ScAddr Node){
    std::string sysIdentifier=context->GetElementSystemIdentifier(Condition);
    std::pair<std::string,std::string> result;
    result.second="";
    result.first=nodes[Node].front();

    if(!sysIdentifier.empty()){
        entitiesInCurrentPackage+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
        result.first="choice"+to_string(conditionCounter);
        conditionCounter++;
        result.second=sysIdentifier+"\n";
        return result;
    }

    entitiesInCurrentPackage+="state choice"+to_string(conditionCounter)+" <<choice>>\n";
    result.first="choice"+to_string(conditionCounter);
    conditionCounter++;
    result.second="";
    
    return result;
}

std::string trim_spaces(std::string str)
{
    // Убедимся, что trim_spaces корректно удаляет пробелы
    str.erase(str.find_last_not_of(' ') + 1);  
    str.erase(0, str.find_first_not_of(' '));  
    return str;
}

/// @details Анализирует сгенерированный строковый буфер PlantUML (entities + relations),
/// выявляет узлы, у которых есть входящие, но нет исходящих связей,
/// и принудительно завершает их переходом в конечное состояние [*].
std::string StateDiagramBuilder::Termination(){
    std::unordered_map<std::string, int> outgoingCounts;
    std::unordered_set<std::string> entitiesWithIncoming;
    std::unordered_set<std::string> allEntities;
    std::string result = "";

    std::stringstream ss(entitiesInCurrentPackage+relations+FormRelations());
    std::string line;

    // Парсинг текущего текста диаграммы для построения графа связности
    while (std::getline(ss, line, '\n')) {
        size_t arrow_pos = line.find("-->");
        if (arrow_pos == std::string::npos) {
            continue; 
        }

        std::string source = line.substr(0, arrow_pos);
        source = trim_spaces(source);
        if (source.empty()) continue; 

        allEntities.insert(source);
        outgoingCounts[source]++; 

        size_t target_start = arrow_pos + 3; 
        size_t target_end = line.find(':', target_start);
        
        if (target_end == std::string::npos) {
            target_end = line.length();
        }

        std::string target = line.substr(target_start, target_end - target_start);
        target = trim_spaces(target);
        if (target.empty()) continue; 

        allEntities.insert(target);
        entitiesWithIncoming.insert(target); 
    }

    for (const std::string& entity : allEntities) {
        
        int out_count = (outgoingCounts.count(entity)) ? outgoingCounts.at(entity) : 0;
        

        bool has_incoming = entitiesWithIncoming.count(entity);
        bool has_no_outgoing = (out_count == 0);
        
        // Если есть вход, но нет выхода -> конец потока управления
        if (has_incoming && has_no_outgoing&&(entitiesInCurrentPackage.find(entity)!=std::string::npos||
        relations.find(entity)!=std::string::npos)) {
            result += entity + " --> [*]" + "\n";
            continue; 
        }
        
        bool is_choice = (entity.find("choice") != std::string::npos); 
        
        bool has_one_outgoing = (out_count == 1);
        
        // Choice-узлы с одним выходом также считаем терминальными для данной ветки
        if (is_choice && has_one_outgoing&&(entitiesInCurrentPackage.find(entity)!=std::string::npos||
        relations.find(entity)!=std::string::npos)) {
            result += entity + " --> [*]" + "\n";
        }
    }
    
    return result;
}
std::string StateDiagramBuilder::FormRelations(){
    std::string result;
    
    for(auto p:relationsByAddr){
        std::string firstEl=nodes[p.first].front();
        size_t s=result.size();
        for(auto node:nodes[p.first]){
            if(node!=firstEl){
                for(auto relation:p.second){
                    std::string rel=relation;
                size_t pos= rel.find(nodes[p.first].front());
                if(pos==0){
                rel.replace(pos,nodes[p.first].front().size(),node);

                result+=rel;
                }
                }
            }
        }
        if(result.size()==s){
            for(auto relation:p.second){            
                result+=relation;
            }
        }else{
            for(auto relation:p.second){
                std::string rel=relation;
            size_t pos= rel.find(nodes[p.first].front());
            if(pos!=0&& rel.find("choice")==0){

                result+=rel;
            }
            }
        }
    }
    return result;
}

/// @details Итерируется по декомпозиции (nrel_decomposition_of_action) пакета,
/// формирует state-блоки и вызывает финализацию (Termination) для текущего уровня вложенности.
void StateDiagramBuilder::ProcessPackage(ScAddr package) {
    ScIterator3Ptr it3=context->CreateIterator3(package, ScType::ConstPermPosArc,ScType::NodeTuple);
    ScIterator5Ptr it5internal;
    while (it3->Next()) {
        it5internal=context->CreateIterator5(ScType::Node, ScType::CommonArc, it3->Get(2),
        ScType::PosArc,Keynodes::nrel_decomposition_of_action);
        while (it5internal->Next()) {

            entities+="state "+context->GetElementSystemIdentifier(it5internal->Get(0))+"{\n"+
            entitiesInCurrentPackage+relations+Termination()+"}\n";
            entitiesInCurrentPackage="";relations="";
            return;
        }
    }
}
std::vector<ScAddrVector> StateDiagramBuilder::FindSequence(
    std::vector<std::pair<ScAddr, int>> entries,
    ScAddr package, ScAddr Node)
{
    std::vector<ScAddrVector> result;

    std::unordered_map<ScAddr, ScAddrVector, ScAddrHashFunc> graph;

    for(auto entry : entries){
        std::tuple<ScAddr,ScAddr> p = context->GetConnectorIncidentElements(entry.first);
        m_logger->Debug("node graph " + context->GetElementSystemIdentifier(std::get<0>(p)));
 
        ScIterator5Ptr it5 = context->CreateIterator5(entry.first, ScType::CommonArc, ScType::CommonArc, 
        ScType::PosArc, Keynodes::nrel_basic_sequence);
        
        while(it5->Next()){
            if(context->CheckConnector(package, it5->Get(2), ScType::PosArc)){
                graph[entry.first].push_back(it5->Get(2));
            }      
        }
    }
    
    m_logger->Debug("form graph finished");

    std::function<void(ScAddr, ScAddrVector&, std::unordered_set<ScAddr, ScAddrHashFunc>&)> dfs;

    dfs = [&](ScAddr node, ScAddrVector &path, 
              std::unordered_set<ScAddr, ScAddrHashFunc> &visited)
    {
        visited.insert(node);
        path.push_back(node);

        if (!graph.count(node) || graph[node].empty()) {
            result.push_back(path);
            
            visited.erase(node);
            path.pop_back();
            return;
        }

        for (ScAddr next : graph[node]) {
            if (!visited.count(next)) {
                dfs(next, path, visited);
            }
        }

        visited.erase(node);
        path.pop_back();
    };

    for (const auto& entry : entries) {
        ScAddr start_node = entry.first;
        
        // std::tuple<ScAddr,ScAddr> p = context->GetConnectorIncidentElements(start_node);
        // m_logger->Debug("dfs start for: " + context->GetElementSystemIdentifier(std::get<0>(p)));
        
        ScAddrVector path;
        std::unordered_set<ScAddr, ScAddrHashFunc> visited;

        dfs(start_node, path, visited);
    }

    m_logger->Debug("dfs finished");

    return result;
}

/// @details Проверяет, является ли узел действием (action) и не принадлежит ли он 
/// декомпозиции другого пакета. Если проверка пройдена, добавляет узел в список processed.
void StateDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package)
{
    if(usedNodes->find(Node)==usedNodes->end() && 
    context->CheckConnector(ScKeynodes::action, Node, ScType::PermPosArc)&&
    nodes.find(Node)==nodes.end()){
        if(context->GetElementSystemIdentifier(Node).empty()){
            ScIterator5Ptr it5=context->CreateIterator5(ScType::NodeClass, ScType::PermPosArc, 
                Node,ScType::PermPosArc, package);
            while(it5->Next()){
                if(it5->Get(0)!=ScKeynodes::action){
                    nodes[it5->Get(0)].push_back(trim(context->GetElementSystemIdentifier(it5->Get(0)))); 
                }
            }
        }else{
            nodes[Node].push_back(context->GetElementSystemIdentifier(Node)); 
        }
        usedNodes->insert(Node);

        ScIterator5Ptr it5=context->CreateIterator5(Node, ScType::CommonArc,ScType::NodeTuple,ScType::PosArc,
             Keynodes::nrel_decomposition_of_action);
        while(it5->Next()){
            for(auto p:*packages){
                if(context->CheckConnector(p, it5->Get(2), ScType::PermPosArc)){
                    return;
                }
                if(!type&&context->CheckConnector(p, Node, ScType::PermPosArc)&& p!=package){
                    throw 1;
                }
            }
            if(context->CheckConnector(package, it5->Get(2), ScType::PermPosArc)){
                return;
            }
        }

        entitiesInCurrentPackage+="state "+context->GetElementSystemIdentifier(Node)+"{\n}\n";
    }
}

/// @details Находит точки входа (start nodes) в логике переходов действий, 
/// учитывая роли (приоритеты) связей. Сортирует результат по приоритету.
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

/// @details Группирует последовательности действий по равным приоритетам
/// для корректного отображения параллельных или альтернативных веток.
std::vector<std::vector<int>> StateDiagramBuilder::FormEqualPrioritiesSequences(
    std::vector<ScAddrVector> sequences,
    std::vector<std::pair<ScAddr, int>> entries)
{
    std::unordered_map<ScAddr, int, ScAddrHashFunc> priorityByAddr;
    priorityByAddr.reserve(entries.size());

    for (auto &p : entries) {
        priorityByAddr[p.first] = p.second;
    }


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

    std::vector<std::vector<int>> result;
    result.reserve(groups.size());

    for (auto &g : groups) {
        result.push_back(g.second);
    }
    return result;
}

/// @details Обрабатывает ребро графа, извлекая привязанные к нему условия (структуры),
/// и формирует текстовое описание перехода с условием (:satisfy ...).
void StateDiagramBuilder::ProcessEdge(ScAddr edge,ScAddr Node){
    m_logger->Debug("Process edge by node: "+context->GetElementSystemIdentifier(Node));
    std::pair<std::string,std::string> condition;
    condition.second="";
    condition.first=nodes[Node].front();
    conditionMap[edge]=condition;
        ScIterator3Ptr it3=context->
        CreateIterator3(edge, ScType::CommonArc, ScType::NodeStructure);
        if(it3->Next()){
            condition=ProcessCondition(it3->Get(2),Node);            
            if(condition.first!=nodes[Node].front()){
                relationsByAddr[Node].push_back(condition.first+ " --> "+nodes[Node].front() +" :satisfy "+condition.second+" ");
                // relations+=;
                conditionMap[edge]=condition;
                m_logger->Debug("Check condition:"+condition.first+ " --> "+nodes[Node].front() +" :satisfy "+condition.second+" ");
            }
        }
}

/// @details Итерируется по всем исходящим дугам узла в рамках пакета.
/// Фильтрует уже обработанные ребра и вызывает ProcessEdge.
void StateDiagramBuilder::ProcessEdgesByNode(ScAddr Node,ScAddr package)
{
    if(context->CheckConnector(ScKeynodes::action,Node, 
        ScType::PermPosArc)){
            ScIterator5Ptr it5 =context->CreateIterator5(ScType::Node, ScType::CommonArc, Node,ScType::PosArc,package);
            m_logger->Debug("ProcessEdgesByNode:"+context->GetElementSystemIdentifier(Node));

            while(it5->Next()){
                m_logger->Debug("source:"+context->GetElementSystemIdentifier(it5->Get(0)));
               if(context->CheckConnector(package, 
                it5->Get(0), ScType::PosArc)&&
                usedEdges->find(it5->Get(1))==usedEdges->end()&&
                context->CheckConnector(ScKeynodes::action, 
                    it5->Get(0), ScType::PosArc))
                {
                if(!context->CheckConnector(package, 
                    it5->Get(1), ScType::PosArc)){
                    m_logger->Debug("why?????");}else{
                ProcessEdge(it5->Get(1),Node);
                usedEdges->insert(it5->Get(1)); 
                m_logger->Debug("conditionMap:"+conditionMap[it5->Get(1)].first+ " value:"+conditionMap[it5->Get(1)].second);
                    }
                    
                }     
            }   
        }
}
//TODO:refactor
/// @details Основной метод обработки смежных узлов.
/// Координирует поиск входов, построение последовательностей (Sequence),
/// группировку по приоритетам и генерацию сложной логики переходов (включая ветвления).
void StateDiagramBuilder::ProcessAdjacentNodes(ScAddr Node,ScAddr package)
{
    if(context->CheckConnector(ScKeynodes::action, Node,ScType::PermPosArc)){
        ScIterator5Ptr it5=context->CreateIterator5(Node, ScType::CommonArc, 
            ScType::NodeTuple,ScType::PosArc,Keynodes::nrel_decomposition_of_action);
        while(it5->Next()){
            if(context->CheckConnector(package, it5->Get(2), ScType::ConstPosArc)){
                return;
            }
        }
        
        // Поиск смежных действий через NodeNonRole
         it5=context->CreateIterator5(Node, ScType::CommonArc, 
            ScType::Node, ScType::PosArc, ScType::NodeNonRole);
            m_logger->Debug("processing :"+context->GetElementSystemIdentifier(Node));
        while(it5->Next()){
            if(context->CheckConnector(package, 
                        it5->Get(2), ScType::PosArc)&& context->CheckConnector(ScKeynodes::action, 
                            it5->Get(2), ScType::PosArc)){

                    m_logger->Debug("processing edges to node:"+context->GetElementSystemIdentifier(it5->Get(2)));
                    ProcessNode(it5->Get(2),package);
                    m_logger->Debug("processed node:"+nodes[it5->Get(2)].front());
                    ProcessEdgesByNode(it5->Get(2),  package);
            }
        }
        m_logger->Debug("trying to find entries:"+context->GetElementSystemIdentifier(Node));
        
        // Поиск и обработка последовательностей выполнения
        std::vector<std::pair<ScAddr, int>> entries=FindEntryPoints(Node,package);
        for(auto p:entries){
            m_logger->Debug("entry:"+context->GetElementSystemIdentifier(p.first)+" value:"+to_string(p.second));
        }
        std::vector<ScAddrVector> sequences;

        sequences=FindSequence(entries,package,Node);

        for(int i=0;i<sequences.size();i++){
            m_logger->Debug("sequence:"+to_string(i));
            for(int j=0;j<sequences[i].size();j++){
                auto pair=context->GetConnectorIncidentElements(sequences[i][j]);
                m_logger->Debug(context->GetElementSystemIdentifier(get<1>(pair)));
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
        // Генерация переходов для каждой группы последовательностей
        for (int sequence=0;sequence<equalSequences.size();sequence++){
            for(int i=0;i<equalSequences[sequence].size();i++){
                m_logger->Debug("processing equal sequence "+to_string(sequence));
                if(sequences[equalSequences[sequence][i]].size()>1||i>0){
                    m_logger->Debug("Start processing large equal sequence:"+to_string(sequences[equalSequences[sequence][i]].size()));
                    std::vector<ScAddrVector> equalSequence;
                    int ind=equalSequences[sequence][i]-1;
                    
                    if(ind==-1 ){
                        entitiesInCurrentPackage+="state Path"+to_string(conditionCounter) +" <<choice>>\n";
                        condition="Path"+to_string(conditionCounter);
                        conditionCounter++;
                        relations+=nodes[Node].front()+" --> "+condition+"\n";
                    }else{
                        entitiesInCurrentPackage+="state Path"+to_string(conditionCounter) +" <<choice>>\n";
                        relations+=condition+" --> "+"Path"+to_string(conditionCounter)+"\n";
                        // condition="Path"+to_string(conditionCounter);
                        // conditionCounter++;
                    }
                    

                    std::string combStr="state PossiblePath"+to_string(conditionCounter)+"{\n"+AddEntitiesAndTransitions( sequences[equalSequences[sequence][i]]
                        ,Node,condition,conditionCounter)+"\n}\n";
                    conditionCounter++;
                    entitiesInCurrentPackage=combStr+entitiesInCurrentPackage;    
                    
                }else{
                    std::string addition;

                    m_logger->Debug("equalSequences[sequence][0]:"+to_string(equalSequences[sequence][0])+"\n"+
                    context->GetElementSystemIdentifier(sequences[equalSequences[sequence][i]][0]));
                    if(equalSequences[sequence][0]-1==-1 ){
                       
                            if(!conditionMap[sequences[equalSequences[sequence][i]][0]].first.empty()){
                                addition=nodes[Node].front()+" --> "+conditionMap[sequences[equalSequences[sequence][i]][0]].first;
                                // nodes[Node]+" --> "+conditionMap[sequences[equalSequences[sequence][i]][0]].first;
                                condition=conditionMap[sequences[equalSequences[sequence][i]][0]].first;
                            }
                            else{
                                addition=nodes[Node].front()+" --> "+nodes[sequences[equalSequences[sequence][i]][0]].front();
                                 
                                condition=nodes[sequences[equalSequences[sequence][i]][0]].front();
                            }
                            if(!conditionMap[sequences[equalSequences[sequence][i]][0]].second.empty())
                                addition+=  " :"+conditionMap[sequences[equalSequences[sequence][i]][0]].second+"\n" ;
                            else
                                addition+="\n";
                        
                        // relations+=addition;
                    }else{
                        if(!conditionMap[sequences[equalSequences[sequence][i]][0]].first.empty()){
                         addition= condition+" --> "+conditionMap[sequences[equalSequences[sequence][i]][0]].first;
                         condition=conditionMap[sequences[equalSequences[sequence][i]][0]].first;
                        }else{
                            addition= condition+" --> "+nodes[sequences[equalSequences[sequence][i]][0]].front();
                            condition=nodes[sequences[equalSequences[sequence][i]][0]].front();
                        }
                        if(!conditionMap[sequences[equalSequences[sequence][i]][0]].second.empty())
                            addition+=  " :"+conditionMap[sequences[equalSequences[sequence][i]][0]].second+"\n" ;
                        else
                        addition+="\n";                
                        // relations+=addition;
                    }
                    relationsByAddr[Node].push_back(addition);
                    m_logger->Debug("Check relations:"+addition);

                }
            }
        }
        m_logger->Debug("result for "+context->GetElementSystemIdentifier(Node)+":\n"+preamble+entities+entitiesInCurrentPackage+relations);
    }
}
std::string StateDiagramBuilder::AddTransitions(ScAddr Node,std::string end,std::string cond){
    std::string result;
    for(auto n:nodes[Node]){
        result+=n+" --> "+end;
        if(!cond.empty()){
            result+=" :"+cond+"\n" ;
        }else{
            result+="\n";
        }
    }
    return result;
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
    m_logger->Debug("capture tuples:"+to_string(tuples.size())+" for "+context->GetElementSystemIdentifier(structure) );
    return tuples;
};
//TODO:refactor
/// @details Собирает все пакеты, участвующие в диаграмме.
/// Определяет иерархию вложенности пакетов на основе пересечения множеств действий.
/// Формирует начальные переходы (entry) для вложенных структур.
std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetAllPackages(ScAddr diagram)
{   
    ScIterator3Ptr it3struct=context->CreateIterator3(diagram, ScType::ConstPermPosArc,ScType::ConstNodeStructure);
    ScAddrToValueUnorderedMap<ScAddrSet> actionsMap;
    ScAddrToValueUnorderedMap<ScAddr> mainActionMap;
    ScAddrSet packages;
    ScIterator3Ptr it3;
    ScAddrSet structures;
    std::shared_ptr<ScAddrSet> packg=std::make_shared<ScAddrSet>();
    std::string entries="";
    std::string newPackages;
    m_logger->Debug("trying to capture packages for:"+context->GetElementSystemIdentifier(diagram)+" type:"+std::string(context->GetElementType(diagram)));
    //сбор всех действий в одном пакете 
    while (it3struct->Next()) {




        ScAddrSet actions;
        ScAddr structure;
        ScAddrVector tuple=CaptureTuple(it3struct->Get(2));
        if(tuple.empty()){
            m_logger->Debug("no tuple in struct:"+context->GetElementSystemIdentifier(it3struct->Get(2)));
            ScIterator5Ptr it5=context->CreateIterator5(it3struct->Get(2), ScType::PosArc,
            ScType::NodeStructure, ScType::PosArc, Keynodes::rrel_entry);
            while(it5->Next()){
                structure=it5->Get(2);
                 tuple=CaptureTuple(it5->Get(2));
                if(tuple.empty())
                    m_logger->Debug("no tuple in struct:"+context->GetElementSystemIdentifier(structure));
                break;
            }
        }else{
            structure=it3struct->Get(2);
            packg->insert(it3struct->Get(2));

        }


        it3=context->CreateIterator3(structure, ScType::ConstPermPosArc, ScType::Node);
        m_logger->Debug("Processing actions for "+context->GetElementSystemIdentifier(structure));
            while(it3->Next()){
                if(context->CheckConnector(ScKeynodes::action, it3->Get(2),ScType::PosArc) &&
                    !context->CheckConnector( it3->Get(2),tuple[0], ScType::CommonArc)){

                    actions.insert(it3->Get(2));     
                }else if(context->CheckConnector(ScKeynodes::action, it3->Get(2),ScType::PosArc) &&
                context->CheckConnector( it3->Get(2),tuple[0], ScType::CommonArc)){

                    mainActionMap[structure]=it3->Get(2);
                }
            }
        
        structures.insert(structure);    
        actionsMap[structure] = actions;




    }
    m_logger->Debug("start processing intersections(size):"+to_string(structures.size()));
    ScIterator5Ptr it5;
    //проверка вхождений действий из одного пакета в другой
    for( auto str:structures){
        std::string state="state "+context->GetElementSystemIdentifier(mainActionMap[str])+"{\n";
        entries+=state;
        for(auto pair: actionsMap){
            if(pair.first!=str&&this->packages->find(pair.first)==this->packages->end()){
                for(auto item:   pair.second){
                    if(context->CheckConnector(str,item,ScType::PosArc)){
                        packages.insert(str);
                        packages.insert(pair.first);
                    }
                }
                if(context->CheckConnector(str,mainActionMap[pair.first],ScType::PosArc)){
                    state+="state "+context->GetElementSystemIdentifier(mainActionMap[pair.first])+"{\n}\n";
                    
                }
            }
        }
        it5=context->CreateIterator5(mainActionMap[str], ScType::CommonArc,
        ScType::NodeTuple, ScType::PosArc, Keynodes::nrel_decomposition_of_action);
                while(it5->Next()&&processedEntries.find(str)==processedEntries.end()){
                    if(context->CheckConnector(str,it5->Get(2),ScType::PosArc)){
                        it5=context->CreateIterator5(it5->Get(2), ScType::PosArc,
                ScType::Node, ScType::PosArc, ScKeynodes::rrel_1);
                        if(it5->Next()){
                            entries+="[*] --> "+context->GetElementSystemIdentifier(it5->Get(2))+"\n";
                            processedEntries.insert(str);
                            break;
                        }
                    }
                }
        it5=context->CreateIterator5(diagram, ScType::PosArc,
                    str, ScType::PosArc, Keynodes::rrel_entry);
        state+="}\n";
        entries+="}\n";
        if(it5->Next()){

            newPackages=state+newPackages;
        }
        else
            newPackages+=state;

    }
    preamble+=newPackages;
    entities+=entries;
    m_logger->Debug("end processing intersections");
    for(auto p:*packg){
        this->packages->insert(p);
    }
    if(structures.size()>0)
        packg->insert(diagram);
    return packg;
}
std::shared_ptr<ScAddrSet> StateDiagramBuilder::GetUsedNodes(ScAddr addr)
{
    return usedNodes;
}
std::string StateDiagramBuilder::GetResultString()
{
    return "@startuml\n"+preamble+entities+entitiesInCurrentPackage+relations+FormRelations()+"\n@enduml";
}

/// @details Проверяет, содержит ли пакет вложенные структуры (через rrel_entry),
/// которые требуют рекурсивной обработки.
bool StateDiagramBuilder::PackageCheck(ScAddr package,ScAddr parent) {
    ScIterator5Ptr it5=context->CreateIterator5(package, ScType::PosArc,ScType::NodeStructure,ScType::PosArc,Keynodes::rrel_entry);
    ScIterator5Ptr it5internal;
    ScIterator3Ptr it3;
    m_logger->Debug("trying to capture packages in package check for "+context->GetElementSystemIdentifier(package));
    while (it5->Next()) {
            if(packages->find(it5->Get(2))==packages->end()){
                it3=context->CreateIterator3(it5->Get(2), ScType::PosArc, ScType::NodeTuple);
                while(it3->Next()){
                    it5internal=context->CreateIterator5(ScType::Node, ScType::CommonArc, it3->Get(2),ScType::PosArc,Keynodes::nrel_decomposition_of_action);
                    while (it5internal->Next()) {
                        
                        return true;
                    }
                }
            }
    }
    return false;
    
}

