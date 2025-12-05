#pragma once
#include <string>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

class BaseDiagramBuilder {
protected:
    std::shared_ptr<ScAddrSet> usedNodes; 
    std::shared_ptr<ScAddrSet> usedEdges;
    bool type = false;

    ScMemoryContext* context;
    utils::ScLogger* m_logger;

public:
    explicit BaseDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger);

    virtual ~BaseDiagramBuilder();

    // --- ядро логики диаграммы ---
    virtual void ProcessNode(ScAddr node, ScAddr container) = 0;
    virtual void ProcessEdgesByNode(ScAddr node, ScAddr container) = 0;
    virtual void ProcessAdjacentNodes(ScAddr node, ScAddr container) = 0;
    virtual std::string GetResultString() = 0;

    ScMemoryContext* GetContext() ;
    utils::ScLogger* GetLogger() ;

    void SetType(bool type) ;

    virtual bool SupportsPackages() const;
};
