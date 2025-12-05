#include <string>
#include "DiagramBuilder.hpp"
#include "BaseDiagramBuilder.hpp"
#include "PackageDiagramBuilder.hpp"

void DiagramBuilder::generateStructure(std::shared_ptr<BaseDiagramBuilder> builder, ScAddr diagram)
{

    if (builder->SupportsPackages())
    {
        auto* pkgBuilder = dynamic_cast<PackageDiagramBuilder*>(builder.get());
        if (!pkgBuilder)
            throw std::runtime_error("Builder claims to support packages but is not PackageDiagramBuilder");

        auto packages = pkgBuilder->GetAllPackages(diagram);

        builder->GetLogger()->Debug(
            "amount of packages captured in " +
            builder->GetContext()->GetElementSystemIdentifier(diagram) + ":" +
            std::to_string(packages->size())
        );

        for (auto package : *packages)
        {
            builder->GetLogger()->Debug(
                "start processing package:" +
                builder->GetContext()->GetElementSystemIdentifier(package)
            );

            ScIterator3Ptr it = builder->GetContext()->CreateIterator3(
                package, ScType::PosArc, ScType::NodeStructure);

            while (it->Next())
            {
                if (pkgBuilder->PackageCheck(it->Get(2), package))
                {
                    builder->GetLogger()->Debug(
                        "internal package start processing:" +
                        builder->GetContext()->GetElementSystemIdentifier(it->Get(2))
                    );

                    generateStructure(builder, it->Get(2)); 
                }
            }

            it = builder->GetContext()->CreateIterator3(
                package, ScType::PosArc, ScType::Node);

            while (it->Next())
            {
                try
                {
                    builder->ProcessNode(it->Get(2), package);
                }
                catch (int e)
                {
                    throw;
                }

                builder->ProcessEdgesByNode(it->Get(2), package);
                builder->ProcessAdjacentNodes(it->Get(2), package);
            }

            builder->GetLogger()->Debug(
                "end processing package:" +
                builder->GetContext()->GetElementSystemIdentifier(package)
            );

            // пакет завершён
            pkgBuilder->ProcessPackage(package);
        }
    }

    builder->GetLogger()->Debug(
        "processing diagram:" +
        builder->GetContext()->GetElementSystemIdentifier(diagram)
    );

    ScIterator3Ptr it = builder->GetContext()->CreateIterator3(
        diagram, ScType::PosArc, ScType::Node);

    while (it->Next())
    {
        try
        {
            builder->ProcessNode(it->Get(2), diagram);
        }
        catch (int e)
        {
            throw;
        }

        builder->ProcessEdgesByNode(it->Get(2), diagram);
        builder->ProcessAdjacentNodes(it->Get(2), diagram);
    }


    if (builder->SupportsPackages())
    {
        dynamic_cast<PackageDiagramBuilder*>(builder.get())->ProcessPackage(diagram);
    }

    builder->GetLogger()->Debug(builder->GetResultString());
}
