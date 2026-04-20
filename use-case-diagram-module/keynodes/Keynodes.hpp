/// @file Keynodes.hpp
/// @brief OSTIS semantic network keynodes for use case diagram generation.
/// 
/// Defines semantic relations, roles, and classes for use case diagrams,
/// including actors, use cases, and their relationships (extend, include, generalization).

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>

/// @class Keynodes
/// @brief Registry of semantic identifiers for use case diagram structures.
/// 
/// Provides compile-time resolved references to semantic concepts and relations
/// used in use case specification within the SC-memory knowledge base.
class Keynodes : public ScKeynodes
{
public:
  /// Action that initiates use case diagram generation process.
  /// Triggered when user requests conversion of use case structure to UML diagram.
  static inline ScKeynode const action_generate_use_case_diagram{
      "action_generate_use_case_diagram", ScType::ConstNodeClass};

  /// Concept representing an actor (user or external system interacting with system).
  /// Used to identify and render actor entities in diagram.
  static inline ScKeynode const concept_actor{"concept_actor", ScType::ConstNodeClass};

  /// Concept for use case package/container.
  /// Groups related use cases and actors within diagram scope.
  static inline ScKeynode const concept_er_package{"concept_er_package", ScType::ConstNodeClass};

  /// Relation indicating use case extension.
  /// Semantics: UseCase A extends UseCase B (optional extension point).
  /// Rendered as: A ..> B : <>
  static inline ScKeynode const nrel_extend_use_case{
      "nrel_extend_use_case", ScType::ConstNodeNonRole};

  /// Relation indicating use case inclusion.
  /// Semantics: UseCase A includes (calls) UseCase B (mandatory).
  /// Rendered as: A ..> B : <>
  static inline ScKeynode const nrel_include_use_case{
      "nrel_include_use_case", ScType::ConstNodeNonRole};

  /// Relation indicating use case generalization.
  /// Semantics: UseCase A is specialized form of UseCase B.
  /// Rendered as: A --|> B
  static inline ScKeynode const nrel_generalization_use_case{
      "nrel_generalization_use_case", ScType::ConstNodeNonRole};
};