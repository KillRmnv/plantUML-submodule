/// @file Keynodes.hpp
/// @brief OSTIS semantic network keynodes for state diagram generation.
/// 
/// Defines semantic relations, roles, and classes used in action decomposition
/// and state transition specifications.

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>

/// @class Keynodes
/// @brief Registry of semantic identifiers for action control flow structures.
/// 
/// Provides compile-time resolved references to semantic relations and roles
/// in the SC-memory knowledge base. Used for pattern matching during diagram
/// extraction and validation.
class Keynodes : public ScKeynodes {
public:
  /// Action that initiates state diagram generation process
  static inline ScKeynode const action_generate_state_diagram{
      "action_generate_state_diagram", ScType::ConstNodeClass};

  /// Relation linking action to its decomposition structure
  /// Maps main action to composed sub-actions and transitions
  static inline ScKeynode const nrel_decomposition_of_action{
      "nrel_decomposition_of_action"};

  /// Relation indicating successful transition (if-then)
  /// Priority: 3 (high)
  static inline ScKeynode const nrel_then{"nrel_then"};

  /// Relation indicating alternative transition (if-else)
  /// Priority: 2 (medium)
  static inline ScKeynode const nrel_else{"nrel_else"};

  /// Relation indicating unconditional goto transition
  /// Priority: 1 (low)
  static inline ScKeynode const nrel_goto{"nrel_goto"};

  /// Relation attaching guard condition to transition
  /// Condition evaluation determines transition feasibility
  static inline ScKeynode const nrel_condition{"nrel_condition"};

  /// Relation marking high-priority transition path
  /// Priority: 4 (highest)
  static inline ScKeynode const nrel_priority_path{"nrel_priority_path"};

  /// Relation defining ordered action sequence
  /// Specifies basic_sequence edge between consecutive actions
  static inline ScKeynode const nrel_basic_sequence{"nrel_basic_sequence"};

  /// Role indicating entry point action in structure
  /// Marks the initial state of decomposed action
  static inline ScKeynode const rrel_entry{"rrel_entry", ScType::ConstNodeRole};

  /// Class for non-atomic (decomposable) actions
  /// Indicates action contains sub-action composition
  static inline ScKeynode const non_atomic_action{"non_atomic_action"};
};