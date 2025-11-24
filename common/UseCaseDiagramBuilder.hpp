#pragma once 
#include <sc-memory/sc_addr.hpp>
#include <string>


#include "ParticularDiagramBuilder.hpp"
#include "DiagramBuilder.hpp"
using namespace std;
class UseCaseDiagramBuilder : public ParticularDiagramBuilder
{


public:
    UseCaseDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override;
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;
    std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;
    std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;
    std::string GetResultString() override;
    void ProcessPackage(ScAddr package) override;
    bool PackageCheck(ScAddr package,ScAddr parent) override;

private:
    void ProcessEdge(ScAddr edge,ScAddr relation,ScAddr package);
    std::string trim(const std::string &s);
    std::string trim_spaces(std::string str);

    std::string entitiesInCurrentPackage;
    std::string entities;
    std::string relations;
    std::string preamble;
    ScAddrToValueUnorderedMap<std::string> nameByNode;
    ScAddrToValueUnorderedMap<std::string> nameByStruct;
    ScAddrToValueUnorderedMap<ScAddrSet> actorsToActions;



};