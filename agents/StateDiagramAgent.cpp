/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */



#include <sc-agents-common/utils/IteratorUtils.hpp>
#include "StateDiagramAgent.hpp"
#include <sc-memory/sc_type.hpp>
#include "common/StateDiagramBuilder.hpp"

// #include "common/StateDiagramBuilder.hpp"
#include "keynodes/Keynodes.hpp"


StateDiagramAgent::StateDiagramAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/StateDiagramAgent.log", utils::ScLogLevel::Debug);
}

ScResult StateDiagramAgent::DoProgram( ScAction & action)
{
    m_logger.Debug("StateDiagramAgent action class:"+m_context.GetElementSystemIdentifier(GetActionClass())+" action:"
    +m_context.GetElementSystemIdentifier(action));
    


    m_logger.Debug("StateDiagramAgent:start");
    auto const [input] = action.GetArguments<1>();

    if (!input.IsValid())
    {
      m_logger.Error("Arguments are not valid.");
      return action.FinishUnsuccessfully();
    }
    DiagramBuilder builder;
    std::shared_ptr<StateDiagramBuilder> stateBuilder=make_shared<StateDiagramBuilder>(&m_context,&m_logger);
    builder.generateStructure(stateBuilder, input);
    stateBuilder->GetResultString();
    ScAddr solutionNode=m_context.GenerateNode(ScType::ConstNode);

    action.FormResult(solutionNode);
    return action.FinishSuccessfully();
  
}

ScAddr StateDiagramAgent::GetActionClass() const
{
  return Keynodes::action_generate_state_diagram;
}

