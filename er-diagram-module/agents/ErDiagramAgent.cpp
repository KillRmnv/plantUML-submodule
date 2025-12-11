/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ErDiagramAgent.hpp"
#include "common/PlantUmlPngGenerator.hpp"
#include "common/ErDiagramBuilder.hpp" 


#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/Keynodes.hpp"

// Constructor: Initialize agent logger with file output at Debug level
ErDiagramAgent::ErDiagramAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/ErDiagramAgent.log", utils::ScLogLevel::Debug);
}

/// Extract input from action arguments, generate diagram, create result node
ScResult ErDiagramAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // Extract input argument: should be ER structure node
  auto const [input] = action.GetArguments<1>(); 

  if (!input.IsValid())
  {
    m_logger.Error("Arguments are not valid. Input structure address is required.");
     return action.FinishUnsuccessfully();
  }
  
  // Initialize diagram generation pipeline
  DiagramBuilder builder;
  std::shared_ptr<ErDiagramBuilder> erBuilder = std::make_shared<ErDiagramBuilder>(&m_context, &m_logger);
  
  // Generate PlantUML code from ER structure
  builder.generateStructure(erBuilder, input); 
  
  // Get final PlantUML diagram string
  std::string result = erBuilder->GetResultString();
  
  // Create solution node to hold results
  ScAddr solutionNode = m_context.GenerateNode(ScType::ConstNodeStructure);
  ScAddr textLink = m_context.GenerateNode(ScType::ConstNodeLink);
  
  // Store PlantUML code in sc-link and connect to solution
  if(m_context.SetLinkContent(textLink, result))
  {
    m_context.GenerateConnector(ScType::ConstPosArc, solutionNode, textLink);
    m_logger.Debug("Set link text with PlantUML code.");
    
    // Convert PlantUML → PNG → Base64 and link to solution
    PlantUmlPngGenerator generator(&m_context, &m_logger);
    generator.png_generator(result, solutionNode); 
  }

  // Return solution node as action result
  action.FormResult(solutionNode);
  return action.FinishSuccessfully();
}

/// Return the action class that triggers this agent
ScAddr ErDiagramAgent::GetActionClass() const
{
  return Keynodes::action_generate_er_diagram;
}