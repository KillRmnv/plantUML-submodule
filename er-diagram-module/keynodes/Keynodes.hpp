/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

 #pragma once

 #include <sc-memory/sc_addr.hpp>
 #include <sc-memory/sc_keynodes.hpp>
 
 /// @class Keynodes
 /// @brief Semantic registry for Entity-Relationship diagram concepts.
 /// 
 /// Defines all semantic identifiers used in ER diagram generation:
 /// - Entity types (strong, weak)
 /// - Attribute types (derived, key, multivalued)
 /// - Relationship types (identifying, strong)
 /// - Cardinality constraints (one, many, zero-or-one, etc.)
 class Keynodes : public ScKeynodes
 {
 public:
   /// Action class that triggers ER diagram generation process
   static inline ScKeynode const action_generate_er_diagram{"action_generate_er_diagram", ScType::ConstNodeClass};
 
   /// Core entity concept
   static inline ScKeynode const concept_entity{"concept_entity", ScType::ConstNodeClass};
 
   /// Regular attribute belonging to an entity
   static inline ScKeynode const concept_attribute{"concept_attribute", ScType::ConstNodeClass};
 
   /// Derived attribute (calculated from other attributes)
   static inline ScKeynode const concept_derived_attribute{"concept_derived_attribute", ScType::ConstNodeClass};
 
   /// Key attribute (primary key, must be unique)
   static inline ScKeynode const concept_key_attribute{"concept_key_attribute", ScType::ConstNodeClass};
 
   // Multivalued attribute (can have multiple values for same entity)
   static inline ScKeynode const concept_multivalued_attribute{"concept_multivalued_attribute", ScType::ConstNodeClass};
 
   /// Weak entity (depends on strong entity for identification)
   static inline ScKeynode const concept_weak_entity{"concept_weak_entity", ScType::ConstNodeClass};
 
   /// Identifying relationship (connects weak entity to strong entity)
   static inline ScKeynode const concept_identifying_relationship{"concept_identifying_relationship", ScType::ConstNodeClass};
 
   /// Cardinality: exactly one (1:1, N:1)
   static inline ScKeynode const concept_one{"concept_one", ScType::ConstNodeClass};
 
   /// Cardinality: many (1:N, N:N)
   static inline ScKeynode const concept_many{"concept_many", ScType::ConstNodeClass};
 
   // Cardinality: one or many (1..N)
   static inline ScKeynode const concept_one_or_many{"concept_one_or_many", ScType::ConstNodeClass};
 
   /// Role indicator: first entity in relationship pair
   static inline ScKeynode const concept_first_domain{"concept_first_domain", ScType::ConstNodeClass};
 
   /// Cardinality: zero or one (0..1, optional participation)
   static inline ScKeynode const concept_zero_or_one{"concept_zero_or_one", ScType::ConstNodeClass};
 
   /// Cardinality: zero or many (0..N, optional participation)
   static inline ScKeynode const concept_zero_or_many{"concept_zero_or_many", ScType::ConstNodeClass};
 
   /// Strong relationship (total participation of both entities)
   static inline ScKeynode const concept_strong_relation{"concept_strong_relation", ScType::ConstNodeClass};
   static inline ScKeynode const concept_second_domain{"concept_second_domain", ScType::ConstNodeClass};
 
 };