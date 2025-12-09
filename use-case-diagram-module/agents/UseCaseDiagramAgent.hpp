/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef C62835DC_2BAA_4144_A202_4763809FC3AA
#define C62835DC_2BAA_4144_A202_4763809FC3AA


#include <sc-memory/sc_agent.hpp>

/// @class UseCaseDiagramAgent
/// @brief Agent responsible for handling use case diagram generation requests.
/// 
/// This agent reacts to the `action_generate_use_case_diagram` initiation.
/// It validates the input structure, orchestrates the `UseCaseDiagramBuilder` to 
/// generate the PlantUML script, and triggers the PNG generation.
class UseCaseDiagramAgent : public ScActionInitiatedAgent
{
public:
  /// @brief Constructs the agent and configures the logger.
  UseCaseDiagramAgent();

  /// @brief Retrieves the action class keynode that triggers this agent.
  /// @return ScAddr of `action_generate_use_case_diagram`.
  ScAddr GetActionClass() const override;

  /// @brief Main execution method for the agent.
  /// 
  /// 1. Validates input arguments.
  /// 2. Invokes the diagram builder to traverse the sc-memory structure.
  /// 3. Catches and logs domain-specific validation errors (e.g. cross-package transitions).
  /// 4. Saves the resulting PlantUML text and PNG link to the action result.
  /// 
  /// @param event The event object describing the action initiation.
  /// @param action The action object containing arguments (the root diagram structure).
  /// @return ScResult::Ok on success, ScResult::Error on failure.
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;


};


#endif /* C62835DC_2BAA_4144_A202_4763809FC3AA */