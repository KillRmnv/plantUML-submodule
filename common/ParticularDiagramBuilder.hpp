#pragma once
#include <string>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

class ParticularDiagramBuilder {
protected:
std::shared_ptr<ScAddrSet> usedNodes; 
std::shared_ptr<ScAddrSet> usedEdges;
    ScMemoryContext * context;
    utils::ScLogger * m_logger;

public:
    explicit ParticularDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger)
        : context(context)
        , m_logger(logger)
    {
        usedNodes=std::make_shared<ScAddrSet>(); 
        usedEdges=std::make_shared<ScAddrSet>();
    }

    virtual ~ParticularDiagramBuilder() ;
    virtual void ProcessNode(ScAddr Node,ScAddr package);
    virtual void ProcessEdgesByNode(ScAddr Node, ScAddr package);
    virtual void ProcessAdjacentNodes(ScAddr Node, ScAddr package);
    virtual std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram);
    virtual std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram);
    virtual std::string GetResultString();
    virtual void ProcessPackage(ScAddr package);

    virtual bool PackageCheck(ScAddr package,ScAddr parent);

    ScMemoryContext* GetContext() { return context; }
    utils::ScLogger* GetLogger() { return m_logger; }
};
