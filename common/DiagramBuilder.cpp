#include "DiagramBuilder.hpp"
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_type.hpp>
ScAddrSet DiagramBuilder::generateStructure(ParticularDiagramBuilder* builder,ScAddr diagram){
    auto packages=builder->GetAllPackages( diagram);
    ScAddrSet unusedAddrs;
    for(auto package:packages){
        ScIterator3Ptr it=builder->GetContext()->CreateIterator3(diagram, ScType::CommonArc,
             ScType::Node);
        while (it->Next()) { 
            if(builder->GetContext()->GetElementType(it->Get(2))==ScType::NodeStructure){
            auto result=generateStructure(builder,it->Get(2));
            unusedAddrs.insert(result.begin(),result.end());
            } 
            builder->ProcessNode(it->Get(2));
            builder->ProcessEdgesByNode(it->Get(2),unusedAddrs);
            builder->ProcessAdjacentNodes(it->Get(2));
        }
        builder->ProcessEdgesByNode(package,unusedAddrs);
    }
    return unusedAddrs;
};