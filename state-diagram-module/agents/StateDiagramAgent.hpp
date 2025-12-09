/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma  once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent_context.hpp>

/// @class StateDiagramAgent
/// @brief Агент, который инициирует генерацию UML диаграмм состояний (State Machine Diagrams).
///
/// Обрабатывает запросы, связанные с `action_generate_state_diagram`, и делегирует
/// построение структуры диаграммы классу `StateDiagramBuilder`.
class StateDiagramAgent : public ScActionInitiatedAgent
{
public:
  /// @brief Конструктор агента, инициализирующий логгер.
  StateDiagramAgent();

  /// @brief Возвращает класс действия, который активирует агента.
  /// @return ScAddr ключевого узла, соответствующего `action_generate_state_diagram`.
  ScAddr GetActionClass() const override;

  /// @brief Основная логика агента для выполнения программы.
  ///
  /// Выполняет: проверку входных данных, настройку `StateDiagramBuilder`,
  /// запуск генерации структуры, обработку ошибок бизнес-логики и сохранение
  /// результата (PlantUML и PNG) в sc-memory.
  ///
  /// @param action Действие, инициировавшее выполнение, содержащее входные аргументы.
  /// @return ScResult::Ok, если выполнение прошло успешно.
  ScResult DoProgram( ScAction & action) override;

  /// @brief Предоставляет доступ к контексту памяти агента.
  /// @return Константная ссылка на контекст агента.
  const ScAgentContext & GetContext() const {
    return m_context;
}


};