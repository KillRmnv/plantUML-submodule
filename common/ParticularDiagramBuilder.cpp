#include "ParticularDiagramBuilder.hpp"

ParticularDiagramBuilder::~ParticularDiagramBuilder() = default;

  bool ParticularDiagramBuilder::PackageCheck(ScAddr package,ScAddr parent) {
    return false;
  }

void ParticularDiagramBuilder::ProcessNode(ScAddr Node,ScAddr package) {}
void ParticularDiagramBuilder::ProcessPackage(ScAddr package) {}

void ParticularDiagramBuilder::ProcessEdgesByNode(ScAddr Node, ScAddr package) {}
void ParticularDiagramBuilder::ProcessAdjacentNodes(ScAddr Node, ScAddr package) {}

std::shared_ptr<ScAddrSet> ParticularDiagramBuilder::GetAllPackages(ScAddr diagram)
{
    return std::make_shared<ScAddrSet>();
}

std::shared_ptr<ScAddrSet> ParticularDiagramBuilder::GetUsedNodes(ScAddr diagram)
{
    return std::make_shared<ScAddrSet>();
}

std::string ParticularDiagramBuilder::GetResultString()
{
    return {};
}
