#pragma once
#include <sc-memory/sc_addr.hpp>
#include <memory>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent_context.hpp>
#include <sc-memory/sc_memory.hpp>



class ParticularDiagramBuilder{
    std::shared_ptr<ScMemoryContext> context;

    public:
    ParticularDiagramBuilder(){
        context=std::make_shared<ScMemoryContext>();
    }
    virtual void ProcessNode(ScAddr Node);
    virtual void ProcessEdgesByNode(ScAddr Node,ScAddrSet unusedEdges);
    virtual void ProcessAdjacentNodes(ScAddr Node);
    virtual ScAddrSet GetAllPackages(ScAddr diagram);
    virtual ScAddrSet GetUsedNodes(ScAddr diagram);
    virtual std::string GetResultString();
    
    std::shared_ptr<ScMemoryContext> GetContext(){
        return context;
    }
};