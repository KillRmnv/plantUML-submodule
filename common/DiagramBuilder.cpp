#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_type.hpp>
#include <memory>
#include <string>
#include "DiagramBuilder.hpp"

std::shared_ptr<ScAddrSet> DiagramBuilder::generateStructure(ParticularDiagramBuilder* builder,ScAddr diagram){

std::shared_ptr<ScAddrSet> packages=builder->GetAllPackages( diagram);

   builder->GetLogger()->Debug("amount of packages captured in "+
        builder->GetContext()->GetElementSystemIdentifier(diagram)+":"+ std::to_string(packages->size()));

    //содержит адреса не использованных  дуг(дуги между пакетами или элементами в разных пакетах,
    // в т.ч. вложенных в друг друга)
    std::shared_ptr<ScAddrSet> unusedAddrs=std::make_shared<ScAddrSet>();

    for(auto package:*packages){

        builder->GetLogger()->Debug("processing package:"+builder->GetContext()->GetElementSystemIdentifier(package));

        ScIterator3Ptr it=builder->GetContext()->CreateIterator3(package, ScType::CommonArc,
             ScType::Node);
             
        while (it->Next()) { 

            //Обработка пакетов в пакетах
            if(builder->GetContext()->GetElementType(it->Get(2))==ScType::NodeStructure){

                auto result=generateStructure(builder,it->Get(2));
            //добавление неиспользованных дуг
                unusedAddrs->insert(result->begin(),result->end());
            } 

            //обработка sc элементов на 1 уровне
            builder->ProcessNode(it->Get(2));
            builder->ProcessEdgesByNode(it->Get(2));
            builder->ProcessAdjacentNodes(it->Get(2));
        }

        builder->GetLogger()->Debug("processing unused edges in package:"+builder->GetContext()->GetElementSystemIdentifier(package)+
    ". amount of unused:"+std::to_string(unusedAddrs->size()));
        //обработка дуг из внутренних пакетов на уровни выше
        builder->ProcessUnusedEdges(package,unusedAddrs);
    }

    builder->GetLogger()->Debug("processing package:"+builder->GetContext()->GetElementSystemIdentifier(diagram));

    //обработка sc элементов на самом верхнем уровне
    ScIterator3Ptr it=builder->GetContext()->CreateIterator3(diagram, ScType::CommonArc,
        ScType::Node);

        while (it->Next()) { 
            
            builder->ProcessNode(it->Get(2));
            builder->ProcessEdgesByNode(it->Get(2));
            builder->ProcessAdjacentNodes(it->Get(2));
        }
        //обработка дуг из внутренних пакетов на уровни выше
        builder->GetLogger()->Debug("processing unused edges in package:"+
            builder->GetContext()->GetElementSystemIdentifier(diagram)+
    ". amount of unused:"+std::to_string(unusedAddrs->size()));

        builder->ProcessUnusedEdges(diagram,unusedAddrs);
        builder->SavePackage();
        
    return unusedAddrs;
};