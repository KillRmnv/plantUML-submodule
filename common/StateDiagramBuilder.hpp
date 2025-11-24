#pragma once 
#include <string>
#include <utility> 

 #include "ParticularDiagramBuilder.hpp"
 #include "DiagramBuilder.hpp"
#include <vector>
using namespace std;
class StateDiagramBuilder : public ParticularDiagramBuilder
{


public:
    StateDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override;
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;
    std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;
    std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;
    std::string GetResultString() override;
    void ProcessPackage(ScAddr package) override;


private:
    std::vector<std::pair<ScAddr, int>> FindEntryPoints(ScAddr action,ScAddr package);
    ScAddrVector CaptureTuple(ScAddr structure);
    std::vector<ScAddrVector> FindSequence(std::vector<std::pair<ScAddr, int>>  entry,ScAddr package,ScAddr Node);
    std::vector<std::vector<int>> FormEqualPrioritiesSequences(std::vector<ScAddrVector> sequences,std::vector<std::pair<ScAddr, int>> entries);
    std::vector<ScAddrVector> Combinations(std::vector<ScAddrVector> sequences);
    std::string AddEntitiesAndTransitions(ScAddrVector comb,ScAddr Node,std::string condition);
    std::string CreateEdgeBetweenActions(std::unordered_map<int, std::vector<std::string>> combByInt,ScAddrVector NextAction,ScAddr BaseAction);
    void ProcessEdge(ScAddr edge,ScAddr package);
    std::pair<std::string,std::string> ProcessCondition(ScAddr Condition,ScAddr Node);
    bool PackageCheck(ScAddr package,ScAddr parent) override;
    std::string Termination();
    int conditionCounter=0;
    ScAddrToValueUnorderedMap<std::string> nodes;
    ScAddrToValueUnorderedMap<std::pair<std::string,std::string>> conditionMap;
    std::shared_ptr<ScAddrSet> packages;
    ScAddrSet processedEntries;
    std::string entities;
    std::string entitiesInCurrentPackage;
    std::string relations;
    std::string preamble;
    ScAddrToValueUnorderedMap<int> priorities;

};