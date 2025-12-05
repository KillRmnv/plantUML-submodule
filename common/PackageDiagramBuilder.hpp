#pragma once

#include "BaseDiagramBuilder.hpp"

class PackageDiagramBuilder : public BaseDiagramBuilder {
public:
    explicit PackageDiagramBuilder(ScMemoryContext* context, utils::ScLogger* logger);

    ~PackageDiagramBuilder() override;

    bool SupportsPackages() const override final;

    // Методы для работы с пакетами
    virtual void ProcessPackage(ScAddr package);
    virtual bool PackageCheck(ScAddr package, ScAddr parent);

    virtual std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram);
    virtual std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram);
};


