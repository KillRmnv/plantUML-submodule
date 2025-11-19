#pragma once
#include "ParticularDiagramBuilder.hpp"
 class DiagramBuilder{
    public:
    std::shared_ptr<ScAddrSet> generateStructure(std::shared_ptr<ParticularDiagramBuilder> builder,ScAddr diagram);
};


