# OSTIS Diagram Builders

C++ library for generating UML diagrams from sc-memory structures in OSTIS systems.

## Overview

This project provides diagram generation modules for the OSTIS (Open Semantic Technology for Intelligent Systems) platform. It converts semantic knowledge represented in sc-memory into industry-standard UML diagrams in PlantUML format.

### Supported Diagram Types

- **State Diagrams** - Represents action lifecycles and state transitions with priority-based routing
- **Use Case Diagrams** - Models system interactions between actors and use cases
- **Entity-Relationship Diagrams** - Visualizes data models using Chen notation
- **PNG Export** - Converts PlantUML code to rasterized images with Base64 encoding

## Requirements

- OSTIS sc-machine v0.10 or higher
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Java runtime for PlantUML generation
- cmake >= 3.10
- GraphViz (`sudo apt update && sudo apt install graphviz`)

## Architecture

### Class Hierarchy

```
BaseDiagramBuilder (abstract)
├── ErDiagramBuilder
└── PackageDiagramBuilder (abstract)
    ├── UseCaseDiagramBuilder
    └── StateDiagramBuilder

DiagramBuilder (orchestrator)
PlantUmlPngGenerator (independent)
```

### Core Components

#### BaseDiagramBuilder
Abstract base class defining the diagram builder interface. All specialized builders inherit from this class.

Methods:
- `ProcessNode(ScAddr node, ScAddr container)` - Process individual nodes
- `ProcessEdgesByNode(ScAddr node, ScAddr container)` - Process outgoing edges
- `ProcessAdjacentNodes(ScAddr node, ScAddr container)` - Process adjacent nodes
- `GetResultString()` - Return diagram in target format

#### DiagramBuilder
Orchestrates recursive traversal of knowledge structures. Implements template method pattern for diagram generation.

Methods:
- `generateStructure(std::shared_ptr<BaseDiagramBuilder> builder, ScAddr diagram)` - Generate diagram from structure

#### PackageDiagramBuilder
Extends BaseDiagramBuilder with hierarchical package support. Used by State and Use Case diagram builders.

Methods:
- `ProcessPackage(ScAddr package)` - Finalize package processing
- `PackageCheck(ScAddr package, ScAddr parent)` - Validate nested structures
- `GetAllPackages(ScAddr diagram)` - Retrieve all packages
- `GetUsedNodes(ScAddr diagram)` - Retrieve processed nodes

#### PlantUmlPngGenerator
Converts PlantUML code to PNG images. Manages temporary files, Base64 encoding, and SC-memory integration.

Methods:
- `png_generator(const std::string& plantUML_code, ScAddr outputNode)` - Generate PNG and store in SC-memory

## State Diagram Builder

### Semantic Concepts

| Concept | Type | Description |
|---------|------|-------------|
| action_generate_state_diagram | ConstNodeClass | Action class for diagram generation |
| nrel_decomposition_of_action | ConstNodeNonRole | Maps action to sub-actions |
| nrel_basic_sequence | ConstNodeNonRole | Ordered action sequence |
| nrel_priority_path | ConstNodeNonRole | Primary execution path (priority 4) |
| nrel_then | ConstNodeNonRole | Success transition (priority 3) |
| nrel_else | ConstNodeNonRole | Alternative transition (priority 2) |
| nrel_goto | ConstNodeNonRole | Unconditional transition (priority 1) |
| nrel_condition | ConstNodeNonRole | Transition guard condition |
| rrel_entry | ConstNodeRole | Structure entry point |
| non_atomic_action | ConstNodeClass | Decomposable action |

### Algorithm

State diagram generation uses depth-first search (DFS) to build all possible action sequences. Priority levels determine transition evaluation order:

1. Traverse action decomposition structures recursively
2. Identify entry points and build sequence graph
3. Execute DFS from each true start node
4. Generate PlantUML state declarations with transitions
5. Mark terminal states with connections to end point [*]

### PlantUML Output

```
@startuml
state MainAction {
  [*] --> InitialAction
  InitialAction --> DecisionState : condition
  DecisionState --> SuccessPath : then
  DecisionState --> FailurePath : else
  SuccessPath --> [*]
  FailurePath --> [*]
}
@enduml
```

## Use Case Diagram Builder

### Semantic Concepts

| Concept | Type | Description |
|---------|------|-------------|
| action_generate_use_case_diagram | ConstNodeClass | Action class for diagram generation |
| concept_actor | ConstNodeClass | System actor entity |
| concept_er_package | ConstNodeClass | Container for grouping use cases |
| nrel_extend_use_case | ConstNodeNonRole | Optional extension |
| nrel_include_use_case | ConstNodeNonRole | Mandatory inclusion |
| nrel_generalization_use_case | ConstNodeNonRole | Specialization/inheritance |

### Algorithm

1. Collect all packages and track hierarchy levels
2. For each package, extract use cases and actors
3. Process relations: extend, include, generalization
4. Generate actor-use case associations
5. Build package hierarchy with nested structures

### PlantUML Output

```
@startuml
package System {
  actor User
  usecase "Login" as UC1
  usecase "Logout" as UC2
}
User --> UC1
UC1 ..> UC2 : include
@enduml
```

## Entity-Relationship Diagram Builder

### Semantic Concepts

| Concept | Type | Description |
|---------|------|-------------|
| action_generate_er_diagram | ConstNodeClass | Action class for diagram generation |
| concept_entity | ConstNodeClass | Regular entity |
| concept_weak_entity | ConstNodeClass | Dependent entity |
| concept_attribute | ConstNodeClass | Entity attribute |
| concept_derived_attribute | ConstNodeClass | Computed attribute |
| concept_key_attribute | ConstNodeClass | Primary key attribute |
| concept_multivalued_attribute | ConstNodeClass | Repeating attribute |
| concept_identifying_relationship | ConstNodeClass | Weak entity relationship |
| concept_strong_relation | ConstNodeClass | Total participation |
| concept_one | ConstNodeClass | Cardinality: exactly 1 |
| concept_many | ConstNodeClass | Cardinality: many |
| concept_one_or_many | ConstNodeClass | Cardinality: 1..N |
| concept_zero_or_one | ConstNodeClass | Cardinality: 0..1 |
| concept_zero_or_many | ConstNodeClass | Cardinality: 0..N |
| concept_first_domain | ConstNodeClass | Role: first entity in pair |

### Algorithm

1. Iterate entity nodes and extract all attributes
2. Process attributes recursively for nested structures
3. Find adjacent entities connected via relationships
4. Generate cardinality notation using Chen method
5. Assemble entity and relationship blocks with connections

### PlantUML Output

```
@startchen
entity Customer {
  customerID <> (key)
  name
  email
}

entity Order {
  orderID <> (key)
  date
}

relationship Places {
}

Customer ==(1,N)== Places -- (1,1) -- Order
@endchen
```

## Usage

### State Diagram Generation

```cpp
#include "builders/state/StateDiagramBuilder.hpp"
#include "builders/common/DiagramBuilder.hpp"

auto builder = std::make_shared<StateDiagramBuilder>(&context, &logger);
DiagramBuilder diagramBuilder;
diagramBuilder.generateStructure(builder, actionNode);
std::string plantUmlCode = builder->GetResultString();
```

### Use Case Diagram Generation

```cpp
#include "builders/usecase/UseCaseDiagramBuilder.hpp"
#include "builders/common/DiagramBuilder.hpp"

auto builder = std::make_shared<UseCaseDiagramBuilder>(&context, &logger);
DiagramBuilder diagramBuilder;
diagramBuilder.generateStructure(builder, systemNode);
std::string plantUmlCode = builder->GetResultString();
```

### ER Diagram Generation

```cpp
#include "builders/er/ErDiagramBuilder.hpp"
#include "builders/common/DiagramBuilder.hpp"

auto builder = std::make_shared<ErDiagramBuilder>(&context, &logger);
DiagramBuilder diagramBuilder;
diagramBuilder.generateStructure(builder, erNode);
std::string plantUmlCode = builder->GetResultString();
```

### PNG Generation

```cpp
#include "builders/generators/PlantUmlPngGenerator.hpp"

PlantUmlPngGenerator generator(&context, &logger);
ScAddr pngLink = generator.png_generator(plantUmlCode, outputNode);
// Result: PNG image stored as Base64 link in SC-memory
```

## Integration with Agents

Diagram generation is exposed through OSTIS agents for automated processing:

- `StateDiagramAgent` - Responds to `action_generate_state_diagram`
- `UseCaseDiagramAgent` - Responds to `action_generate_use_case_diagram`
- `ErDiagramAgent` - Responds to `action_generate_er_diagram`

Each agent follows the same pattern:
1. Extract input structure from action arguments
2. Create appropriate builder instance
3. Generate PlantUML code via DiagramBuilder
4. Create solution node with PlantUML link
5. Generate PNG image and link to solution
6. Return solution node as result

## Implementation Details

### State Diagram - Algorithm Specifics

State diagram builder uses DFS to construct all action sequences from entry points. The algorithm:

1. Finds all entry points marked with `rrel_entry`
2. Builds directed graph using `nrel_basic_sequence` relations
3. Identifies true start nodes (entry points not reached by other paths)
4. Executes DFS to find all possible paths
5. Generates choice states for conditional branches
6. Determines terminal states using outgoing edge analysis

Priority system for transitions:
- Priority 4: `nrel_priority_path` (primary)
- Priority 3: `nrel_then` (success)
- Priority 2: `nrel_else` (alternative)
- Priority 1: `nrel_goto` (unconditional)

### Use Case Diagram - Algorithm Specifics

Use case diagram builder uses iterative traversal with hierarchy level tracking:

1. Collects all packages using `concept_er_package` filter
2. Tracks hierarchy levels in `structuresByLevel` map
3. Processes each package and extracts use cases
4. Finds adjacent use cases through semantic relations
5. Determines relation types (extend, include, generalization)
6. Generates cardinality-annotated connections

Special handling: `ScAddrTripleHash` and `ScAddrTripleEq` enable efficient tuple-based lookups in unordered_map for tracking relationships.

### ER Diagram - Algorithm Specifics

ER diagram builder processes entities, attributes, and relationships:

1. Iterates entity nodes marked with `concept_entity`
2. Extracts attributes and processes recursively for nesting
3. Finds adjacent entities through common arcs
4. Determines cardinality using `ChenCardinality()` method
5. Applies Chen notation rules:
   - `=` for strong participation (total)
   - `-` for weak participation (partial)
   - Cardinality values: 1, N, (0,1), (0,N), (1,N)
6. Marks weak entities and identifying relationships with notation

### PNG Generator - Implementation Details

PNG generation pipeline:
1. Write PlantUML code to temporary .puml file
2. Execute `java -jar plantuml.jar -tpng` command
3. Read generated PNG file in binary mode
4. Encode PNG data to Base64 string
5. Create SC-memory link (ConstNodeLink) with Base64 content
6. Associate link with output node via ConstPermPosArc
7. Mark format as PNG using `nrel_format`/`format_png`
8. Delete temporary files

Cross-platform compatibility:
- Windows: `localtime_s()` for thread-safe time operations
- Unix: `localtime_r()` for thread-safe time operations

## Testing

Unit tests cover:
- Node processing for all diagram types
- Relation extraction and validation
- PlantUML syntax generation
- PNG generation and Base64 encoding
- Error handling for invalid structures

## Performance Considerations

- State diagram generation: O(n*m) where n=nodes, m=transitions (DFS traversal)
- Use case diagram generation: O(n+m) where n=packages, m=relations (iterative)
- ER diagram generation: O(n+a) where n=entities, a=attributes (recursive attributes)
- PNG generation: I/O bound on external PlantUML command execution

## Error Handling

All builders implement comprehensive error checking:
- Validate node addresses before processing
- Check connector types before following edges
- Skip nodes with empty system identifiers
- Prevent processing of duplicate elements using tracking sets
- Log debug information for failed structure validation

## References

- [OSTIS Platform](http://ostis.net)
- [SC-machine Documentation](https://ostis-ai.github.io/sc-machine/)
- [PlantUML Documentation](http://plantuml.com)
- [Entity-Relationship Model](https://plantuml.com/ru/er-diagram)
