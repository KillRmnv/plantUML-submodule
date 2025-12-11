/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_agent.hpp>

/// @class ErDiagramAgent
/// @brief OSTIS agent that generates ER diagrams on demand.
/// 
/// Responds to action_generate_er_diagram action invocation.
/// Reads ER structure from input, generates PlantUML code, converts to PNG,
/// and returns result containing both PlantUML text and PNG image links.
class ErDiagramAgent : public ScActionInitiatedAgent
{
public:
  /// @brief Constructs ER diagram agent with file-based logging.
  /// Sets up logger output to "logs/ErDiagramAgent.log" at Debug level.
  ErDiagramAgent();

  /// @brief Returns the action class this agent responds to.
  /// @return ScAddr of action_generate_er_diagram action class
  ScAddr GetActionClass() const override;

  /// @brief Executes ER diagram generation program.
  /// 
  /// Algorithm:
  /// 1. Extract input ER structure from action arguments
  /// 2. Create ErDiagramBuilder and initialize DiagramBuilder orchestrator
  /// 3. Generate PlantUML text via generateStructure()
  /// 4. Create solution node with PlantUML content link
  /// 5. Call PNG generator to convert PlantUML → PNG → Base64
  /// 6. Return solution node as action result
  /// 
  /// @param event The action initiation event containing parameters
  /// @param action The action object for argument access and result formation
  /// @return ScResult status code (success/failure)
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;


};
