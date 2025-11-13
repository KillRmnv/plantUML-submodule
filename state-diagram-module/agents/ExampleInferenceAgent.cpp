/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ExampleInferenceAgent.hpp"


#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/Keynodes.hpp"


ExampleInferenceAgent::ExampleInferenceAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/ExampleInferenceAgent.log", utils::ScLogLevel::Debug);
}

ScResult ExampleInferenceAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto const [targetStructure, formulasSet, arguments, inputStructure] = action.GetArguments<4>();


  if (!arguments.IsValid())
  {
    m_logger.Error("Arguments are not valid.");
    // return action.FinishUnsuccessfully();
  }
  ScAddr solutionNode=m_context.GenerateNode(ScType::ConstNode);

  action.FormResult(solutionNode);
  return action.FinishSuccessfully();
}

ScAddr ExampleInferenceAgent::GetActionClass() const
{
  return Keynodes::action_generate_state_diagram;
}

