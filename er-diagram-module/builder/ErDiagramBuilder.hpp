#ifndef E164795F_4231_4185_B75A_4A5F94B86E68
#define E164795F_4231_4185_B75A_4A5F94B86E68
#pragma once 
#include <string>
#include <utility> 
#include <set>
#include <sc-memory/sc_addr.hpp>

#include "common/ParticularDiagramBuilder.hpp"
#include "common/DiagramBuilder.hpp"
using namespace std;

class ErDiagramBuilder : public ParticularDiagramBuilder{
public:
    explicit ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override {} 
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;
    void ProcessPackage(ScAddr package) override {};

    std::string GetResultString() override;

private:
    std::vector<ScAddr> GetClassMembers(ScAddr class_node);
    std::vector<ScAddr> GetAttributes(ScAddr entity);
    bool IsOptional(ScAddr attr);
    bool IsRequired(ScAddr attr);
    std::vector<ScAddr> GetChildAttributes(ScAddr attr);

    std::string ChenCardinality(ScAddr relNode);
    std::string MakeEntityBlock(ScAddr entity);
    std::string MakeRelationshipBlock(ScAddr relNode); 
   
    std::string entities_;
    std::string relationships_;
    std::string relations_;
    std::set<ScAddr> usedRelationships; 

};


#endif
