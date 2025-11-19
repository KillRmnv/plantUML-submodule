#pragma once 
#include <string>
#include <utility> 

 #include "ParticularDiagramBuilder.hpp"
 #include "DiagramBuilder.hpp"
using namespace std;
class StateDiagramBuilder : public ParticularDiagramBuilder
{


public:
    StateDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override;
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;
    void ProcessUnusedEdges(ScAddr package, std::shared_ptr<ScAddrSet> edges) override;
    std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;
    std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;
    std::string GetResultString() override;
    void ProcessPackage(ScAddr package) override;

private:
    std::vector<std::pair<ScAddr, int>> FindEntryPoints(ScAddr action,ScAddr package);
    ScAddrVector CaptureTuple(ScAddr structure);
    std::vector<ScAddrVector> FindSequence(std::vector<std::pair<ScAddr, int>>  entry,ScAddr package);
    std::vector<std::vector<int>> FormEqualPrioritiesSequences(std::vector<ScAddrVector> sequences,std::vector<std::pair<ScAddr, int>> entries);
    std::vector<ScAddrVector> Combinations(std::vector<ScAddrVector> sequences);
    std::string AddEntitiesAndTransitions(ScAddrVector comb,ScAddr Node,std::string condition);
    std::string CreateEdgeBetweenActions(std::unordered_map<int, std::vector<std::string>> combByInt,ScAddrVector NextAction,ScAddr BaseAction);
    void ProcessEdge(ScAddr edge,ScAddr package);
    std::string GetNodeName(ScAddr node,bool destination);
    std::pair<std::string,std::string> ProcessCondition(ScAddr Condition,ScAddr Node);
    int conditionCounter=0;
    ScAddrToValueUnorderedMap<std::string> nodes;
    ScAddrToValueUnorderedMap<std::pair<std::string,std::string>> conditionMap;
    std::shared_ptr<ScAddrSet> packages;
    std::string entities;
    std::string relations;
    ScAddrToValueUnorderedMap<int> priorities;
    struct LocalScTypeHash {
        size_t operator()(ScType const & t) const noexcept {
            return std::hash<sc_type>{}(*t);
        }
    };
    
    std::unordered_map<ScType, std::string, LocalScTypeHash> descriptionByType;

};