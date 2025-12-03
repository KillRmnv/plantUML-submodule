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

  static inline ScKeynode const rrel_entry{"rrel_entry", ScType::ConstNodeRole};

  static inline ScKeynode const concept_actor{"concept_actor", ScType::ConstNodeClass};

  static inline ScKeynode const concept_er_package{"concept_er_package", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_extend_use_case{"nrel_extend_use_case", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_include_use_case{"nrel_include_use_case", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_generalization_use_case{"nrel_generalization_use_case", ScType::ConstNodeNonRole};

  static inline ScKeynode const concept_entity{"concept_entity", ScType::ConstNodeClass};
  static inline ScKeynode const concept_attribute{"concept_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_attrconcept_optional_attributeibute{"concept_optional_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_key_attribute{"concept_key_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_one_to_one{"concept_one_to_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_one_to_many{"concept_one_to_many", ScType::ConstNodeClass};
  static inline ScKeynode const concept_many_to_one{"concept_many_to_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_many_to_many{"concept_many_to_many", ScType::ConstNodeClass};


};
