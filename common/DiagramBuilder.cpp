
#include <string>
#include "DiagramBuilder.hpp"

std::shared_ptr<ScAddrSet> DiagramBuilder::generateStructure(std::shared_ptr<ParticularDiagramBuilder> builder,ScAddr diagram){

std::shared_ptr<ScAddrSet> packages=builder->GetAllPackages( diagram);

    builder->GetLogger()->Debug("amount of packages captured in "+
        builder->GetContext()->GetElementSystemIdentifier(diagram)+":"+ std::to_string(packages->size()));

    std::shared_ptr<ScAddrSet> unusedAddrs=std::make_shared<ScAddrSet>();

    for(auto package:*packages){

        builder->GetLogger()->Debug("start processing package:"+builder->GetContext()->GetElementSystemIdentifier(package));

        ScIterator3Ptr it=builder->GetContext()->CreateIterator3(package, ScType::PosArc,
             ScType::NodeStructure);
        while(it->Next()){
            if(builder->PackageCheck(it->Get(2),package)){
                builder->GetLogger()->Debug("internal package start processing:"+builder->GetContext()->GetElementSystemIdentifier(it->Get(2)));
                auto result=generateStructure(builder,it->Get(2));
            } 
        }    
        it=builder->GetContext()->CreateIterator3(package, ScType::PosArc,
            ScType::Node); 
        while (it->Next()) {         
            builder->ProcessNode(it->Get(2),package);
            builder->ProcessEdgesByNode(it->Get(2),package);
            builder->ProcessAdjacentNodes(it->Get(2),package);
        }

        builder->GetLogger()->Debug("end processing package:"+builder->GetContext()->GetElementSystemIdentifier(package));
        builder->ProcessPackage(package); 
    }

    builder->GetLogger()->Debug("processing diagram:"+builder->GetContext()->GetElementSystemIdentifier(diagram));

    ScIterator3Ptr it=builder->GetContext()->CreateIterator3(diagram, ScType::PosArc,
        ScType::Node);

    while (it->Next()) {       
            builder->ProcessNode(it->Get(2),diagram);
            builder->ProcessEdgesByNode(it->Get(2),diagram);
            builder->ProcessAdjacentNodes(it->Get(2),diagram);
        }
    builder->ProcessPackage(diagram);
    builder->GetLogger()->Debug(builder->GetResultString());
    return unusedAddrs;
};