
#pragma once 
#include <string>
#include <utility> 
#include <set>
#include <sc-memory/sc_addr.hpp>

#include "../../common/BaseDiagramBuilder.hpp"
#include "../../common/DiagramBuilder.hpp"
using namespace std;

class ErDiagramBuilder : public BaseDiagramBuilder{
public:
    explicit ErDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override {} 
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;

    std::string GetResultString() override;

private:
    std::vector<ScAddr> GetClassMembers(ScAddr class_node);
    std::vector<ScAddr> GetAttributes(ScAddr entity,ScAddr package);
    bool IsOptional(ScAddr attr);
    bool IsRequired(ScAddr attr);
    std::vector<ScAddr> GetChildAttributes(ScAddr attr,ScAddr package);

    std::pair<std::string,std::string> ChenCardinality(ScAddr classNode,bool hasFirst)  ;
      std::string MakeEntityBlock(ScAddr entity,ScAddr package);
    std::string MakeRelationshipBlock(ScAddr relNode,ScAddr package); 
   
    std::string entities_;
    std::string relationships_;
    std::string relations_;
    ScAddrSet usedRelationships; 
    ScAddrToValueUnorderedMap<ScAddr> entityToRelation;

};


