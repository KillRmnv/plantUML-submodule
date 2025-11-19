/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "plantUMLModule.hpp"

#include "agents/ErDiagramAgent.hpp"
#include "agents/StateDiagramAgent.hpp"
#include "agents/UseCaseDiagramAgent.hpp"

SC_MODULE_REGISTER(PlantUMLModule)
    ->Agent<ErDiagramAgent>()
    ->Agent<StateDiagramAgent>()
    ->Agent<UseCaseDiagramAgent>();
