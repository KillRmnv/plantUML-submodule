#include "PackageDiagramBuilder.hpp"

PackageDiagramBuilder::PackageDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger)
    : BaseDiagramBuilder(context, logger)
{
}

PackageDiagramBuilder::~PackageDiagramBuilder() = default;

bool PackageDiagramBuilder::SupportsPackages() const
{
    return true;
}

void PackageDiagramBuilder::ProcessPackage(ScAddr package)
{
}

bool PackageDiagramBuilder::PackageCheck(ScAddr package, ScAddr parent)
{
    return false;
}

std::shared_ptr<ScAddrSet> PackageDiagramBuilder::GetAllPackages(ScAddr diagram)
{
    return std::make_shared<ScAddrSet>();
}

std::shared_ptr<ScAddrSet> PackageDiagramBuilder::GetUsedNodes(ScAddr diagram)
{
    return std::make_shared<ScAddrSet>();
}
