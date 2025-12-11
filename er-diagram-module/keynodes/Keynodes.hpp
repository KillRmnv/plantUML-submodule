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
  static inline ScKeynode const concept_entity{"concept_entity", ScType::ConstNodeClass};
  static inline ScKeynode const concept_attribute{"concept_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_optional_attribute{"concept_optional_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_key_attribute{"concept_key_attribute", ScType::ConstNodeClass};
  static inline ScKeynode const concept_one_to_one{"concept_one_to_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_one_to_many{"concept_one_to_many", ScType::ConstNodeClass};
  static inline ScKeynode const concept_many_to_one{"concept_many_to_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_many_to_many{"concept_many_to_many", ScType::ConstNodeClass};
  static inline ScKeynode const concept_weak_entity{"concept_weak_entity", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_identifying_relationship{"nrel_identifying_relationship", ScType::ConstNodeNonRole};
  static inline ScKeynode const concept_one_or_many{"concept_one_or_many", ScType::ConstNodeClass};
  static inline ScKeynode const concept_second_domain{"concept_second_domain", ScType::ConstNodeClass};

  static inline ScKeynode const concept_first_domain{"concept_first_domain", ScType::ConstNodeClass};
  static inline ScKeynode const concept_zero_or_one{"concept_zero_or_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_zero_or_many{"concept_zero_or_many", ScType::ConstNodeClass};
  static inline ScKeynode const concept_one{"concept_one", ScType::ConstNodeClass};
  static inline ScKeynode const concept_many{"concept_many", ScType::ConstNodeClass};

  
};
