/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "UseCaseDiagramAgent.hpp"


#include <sc-agents-common/utils/IteratorUtils.hpp>
#include "builder/UseCaseDiagramBuilder.hpp"
#include "common/PlantUmlPngGenerator.hpp"
#include "keynodes/Keynodes.hpp"


UseCaseDiagramAgent::UseCaseDiagramAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/UseCaseDiagramAgent.log", utils::ScLogLevel::Debug);
}

ScResult UseCaseDiagramAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto const [input] = action.GetArguments<1>();


  if (!input.IsValid())
  {
    m_logger.Error("Arguments are not valid.");
     return action.FinishUnsuccessfully();
  }
    DiagramBuilder builder;
    std::shared_ptr<UseCaseDiagramBuilder> useCaseBuilder=std::make_shared<UseCaseDiagramBuilder>(&m_context,&m_logger);
    
    try{
      builder.generateStructure(useCaseBuilder, input);
      }catch(int e){
        if(e==1){
          m_logger.Error("In the use case diagram, actions should have transitions only within a single package.");
         return  action.FinishUnsuccessfully();
        }
      }
    ScAddr solutionNode=m_context.GenerateNode(ScType::ConstNodeTuple);
    std::string result=useCaseBuilder->GetResultString();
    ScAddr text=m_context.GenerateNode(ScType::ConstNodeLink);
  
    if(m_context.SetLinkContent(text, result)){
      m_context.GenerateConnector(ScType::ConstPosArc, solutionNode,text );
      m_logger.Debug("Set link text");
      PlantUmlPngGenerator generator(&m_context,&m_logger);
      generator.png_generator(result, solutionNode);
    }

  action.FormResult(solutionNode);
  return action.FinishSuccessfully();
}

ScAddr UseCaseDiagramAgent::GetActionClass() const
{
  return Keynodes::action_generate_use_case_diagram;
}

