#include "BaseDiagramBuilder.hpp"

BaseDiagramBuilder::BaseDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger)
    : context(context)
    , m_logger(logger)
{
    usedNodes = std::make_shared<ScAddrSet>();
    usedEdges = std::make_shared<ScAddrSet>();
}

BaseDiagramBuilder::~BaseDiagramBuilder() = default;

void BaseDiagramBuilder::ProcessNode(ScAddr node, ScAddr container)
{
    // пусто
}

void BaseDiagramBuilder::ProcessEdgesByNode(ScAddr node, ScAddr container)
{
    // пусто
}

void BaseDiagramBuilder::ProcessAdjacentNodes(ScAddr node, ScAddr container)
{
    // пусто
}

std::string BaseDiagramBuilder::GetResultString()
{
    return {};
}

ScMemoryContext* BaseDiagramBuilder::GetContext()
{
    return context;
}

utils::ScLogger* BaseDiagramBuilder::GetLogger()
{
    return m_logger;
}

void BaseDiagramBuilder::SetType(bool t)
{
    type = t;
}

bool BaseDiagramBuilder::SupportsPackages() const
{
    return false;
}
