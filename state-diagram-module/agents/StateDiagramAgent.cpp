/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include "StateDiagramAgent.hpp"
#include <sc-memory/sc_type.hpp>
#include "builder/StateDiagramBuilder.hpp"
#include "common/PlantUmlPngGenerator.hpp"

// #include "common/StateDiagramBuilder.hpp"
#include "keynodes/Keynodes.hpp"
#include <string>


/// Инициализирует агент, настраивая логгер для записи отладочной информации
/// в файл `logs/StateDiagramAgent.log`.
StateDiagramAgent::StateDiagramAgent()
{
  m_logger =
      utils::ScLogger(utils::ScLogger::ScLogType::File, "logs/StateDiagramAgent.log", utils::ScLogLevel::Debug);
}

/// Основной метод, который выполняет генерацию диаграммы состояний.
ScResult StateDiagramAgent::DoProgram( ScAction & action)
{
    m_logger.Debug("StateDiagramAgent action class:"+m_context.GetElementSystemIdentifier(GetActionClass())+" action:"
    +m_context.GetElementSystemIdentifier(action));


    m_logger.Debug("StateDiagramAgent:start");
    /// Извлекает аргументы действия: входную структуру (`input`) и необязательный тип (`type`).
    auto const [input,type] = action.GetArguments<2>();

    if (!input.IsValid())
    {
      m_logger.Error("Input structure are not valid.");
      return action.FinishUnsuccessfully();
    }

    DiagramBuilder builder;
    std::shared_ptr<StateDiagramBuilder> stateBuilder=make_shared<StateDiagramBuilder>(&m_context,&m_logger);

    /// Проверяет, был ли передан тип `non_atomic_action`, чтобы настроить билдep.
    if(type.IsValid() && type==Keynodes::non_atomic_action){
      stateBuilder->SetType(true);
    }
    
    try{
    /// Запускает обход структуры и генерацию PlantUML через `DiagramBuilder`.
    builder.generateStructure(stateBuilder, input);
    }catch(int e){
      /// Обработка специфической ошибки: код `1` указывает на нарушение бизнес-правила
      /// о переходах между вложенными состояниями.
      if(e==1){
        m_logger.Error("In state diagram actions in substates can not have transitions with actions in other substates");
       return  action.FinishUnsuccessfully();
      }
    }
   std::string result= stateBuilder->GetResultString();

    /// Создает результирующий узел (кортеж) для хранения сгенерированных данных.
    ScAddr solutionNode=m_context.GenerateNode(ScType::ConstNodeTuple);
    ScAddr text=m_context.GenerateNode(ScType::ConstNodeLink);
    
    /// Сохраняет PlantUML-скрипт в узел-ссылку и запускает генерацию PNG.
    if(m_context.SetLinkContent(text, result)){
      m_context.GenerateConnector(ScType::ConstPosArc, solutionNode,text );
      m_logger.Debug("Set link text");
      PlantUmlPngGenerator generator(&m_context,&m_logger);
      generator.png_generator(result, solutionNode);
    }
    
    /// Формирует результат действия, связывая его с узлом решения.
    action.FormResult(solutionNode);
    return action.FinishSuccessfully();
}

/// Возвращает адрес ключевого узла, который используется для активации агента
/// (определено в Keynodes.hpp).
ScAddr StateDiagramAgent::GetActionClass() const
{
  return Keynodes::action_generate_state_diagram;
}