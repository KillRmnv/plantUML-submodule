/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>

class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_generate_er_diagram{"action_generate_er_diagram", ScType::ConstNodeClass};

  static inline ScKeynode const action_generate_state_diagram{"action_generate_state_diagram", ScType::ConstNodeClass};
  
  static inline ScKeynode const nrel_decomposition_of_action{"nrel_decomposition_of_action"};

  static inline ScKeynode const nrel_then{"nrel_then"};

  static inline ScKeynode const nrel_else{"nrel_else"};

  static inline ScKeynode const nrel_goto{"nrel_goto"};

  static inline ScKeynode const nrel_condition{"nrel_condition"};

  static inline ScKeynode const nrel_priority_path{"nrel_priority_path"};

  static inline ScKeynode const nrel_basic_sequence{"nrel_basic_sequence"};

  static inline ScKeynode const action_generate_use_case_diagram{"action_generate_use_case_diagram", ScType::ConstNodeClass};

};
