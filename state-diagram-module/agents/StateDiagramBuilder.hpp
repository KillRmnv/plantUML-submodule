#pragma once 
#include <sc-memory/sc_addr.hpp>
#include <string>
#include <vector>
#include "../../common/DiagramBuilder.hpp"
#include "../../common/ParticularDiagramBuilder.hpp"

using namespace std;
class StateDiagramBuilder : public ParticularDiagramBuilder
{

public:
    StateDiagramBuilder();

    void ProcessNode(ScAddr Node) override;
    void ProcessEdgesByNode(ScAddr Node) override;
    void ProcessAdjacentNodes(ScAddr Node) override;
    void ProcessUnusedEdges(ScAddr package, std::shared_ptr<ScAddrSet> edges) override;
    std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;
    std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;
    std::string GetResultString() override;
private:
ScAddrVector CaptureTuple(ScAddr structure);
std::shared_ptr<ScAddrSet> ProcessPackages();

void ProcessEdge(ScAddr edge,std::shared_ptr<ScAddrToValueUnorderedMap<ScAddr>> sequnce);

std::string ProcessCondition(ScAddr structure);
};