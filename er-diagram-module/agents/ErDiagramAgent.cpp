/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ErDiagramAgent.hpp"
#include "common/PlantUmlPngGenerator.hpp"
#include "builder/ErDiagramBuilder.hpp" 


#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/Keynodes.hpp"


ErDiagramAgent::ErDiagramAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/ErDiagramAgent.log", utils::ScLogLevel::Debug);
}

ScResult ErDiagramAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto const [input] = action.GetArguments<1>(); 

  if (!input.IsValid())
  {
    m_logger.Error("Arguments are not valid. Input structure address is required.");
     return action.FinishUnsuccessfully();
  }
  
  DiagramBuilder builder;
  std::shared_ptr<ErDiagramBuilder> erBuilder = std::make_shared<ErDiagramBuilder>(&m_context, &m_logger);
  
  builder.generateStructure(erBuilder, input); 
  
  std::string result = erBuilder->GetResultString();
  
  ScAddr solutionNode = m_context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr textLink = m_context.GenerateNode(ScType::ConstNodeLink);
  
  if(m_context.SetLinkContent(textLink, result))
  {
    m_context.GenerateConnector(ScType::ConstPosArc, solutionNode, textLink);
    m_logger.Debug("Set link text with PlantUML code.");
    
    PlantUmlPngGenerator generator(&m_context, &m_logger);
    generator.png_generator(result, solutionNode); 
  }

  action.FormResult(solutionNode);
  return action.FinishSuccessfully();
}

ScAddr ErDiagramAgent::GetActionClass() const
{
  return Keynodes::action_generate_er_diagram;
}