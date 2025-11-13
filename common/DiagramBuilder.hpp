#pragma once
#include <sc-memory/sc_addr.hpp>
#include "ParticularDiagramBuilder.hpp"
 class DiagramBuilder{
    public:
    ScAddrSet generateStructure(ParticularDiagramBuilder* builder,ScAddr diagram);
};