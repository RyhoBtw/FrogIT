
# 

**About arc42**

arc42, the template for documentation of software and system
architecture.

Template Version 9.0-EN. (based upon AsciiDoc version), July 2025

Created, maintained and © by Dr. Peter Hruschka, Dr. Gernot Starke and
contributors. See <https://arc42.org>.

# 1 Introduction and Goals {#section-introduction-and-goals}

## 1.1 Requirements Overview {#_requirements_overview}
FrogIT is a desktop application that allows users to create custom relaxing soundscapes by mixing ambient sounds such as frogs and jungle noises.
Additionally, the system provides visual feedback through animated frogs to enhance the relaxation experience.
**Key features**:
- Create and mix soundscapes
- Adjust sound parameters (e.g. volume)
- Animated frog visualization
- Preset saving and loading

## 1.2 Quality Goals {#_quality_goals}
Based on the quality tree, the most imporant quality goals are:
1. **Performance (High Priority)**
	- Low-latency, real-time audio playback without interruptions
2. **Maintainability / Extensibility (High Priority)**
	- Easy to extend with new sounds, animations and UI components
3. **Usability (Medium Priority)**
	- Intuitive control of the sound mixer
	- Non-intrusive visual feedback during usage
4. **Reliability (Medium Priority)**
	- Robust handling of missing or corrupted resources
5. **Security / Privacy (Low Priority)**
	- Local-only storage of user data without external communication

## 1.3 Stakeholders {#_stakeholders}

| Role/Name              | Contact | Expectations                                                         |
| ---------------------- | ------- | -------------------------------------------------------------------- |
| User                   | -       | Easy-to-use application for relaxation with smooth audio and visuals |
| Developers (Team)      | -       | Maintainable, modular and extensible codebase                        |
| Lecturer / Stakeholder | -       | Clean architecture, proper documentation and working prototype       |

# 2 Architecture Constraints {#section-architecture-constraints}
## 2.1 Technical Constraints
The system is developed under the following technical constraints:
- Implementation language: C++
- Use of SFML for audio processing
- Use of Dear ImGui for the user interface
- Cross-platform support: Windows, macOS, Linux
- Build system: CMake
- CI/CD via GitHub Actions
- Local execution only (no cloud/backend components)

## 2.2 Organizational Constraints
The project is developed under the following organizational conditions:
- Team of four developers
- Agile development using Scrum (weekly Sprints)
- Task Management via GitHub Projects and Kanban board
- Version control with GitHub
- Fixed deadlines defined by the course schedule

## 2.3 Conventions
The following conventions are applied:
- Code follows consistent naming conventions (e.g. camelCase / UpperCamelCase)
- Code formatting is enforced using clang-format
- Documentation is written in Markdown
- UML diagrams are created using PlantUML / draw.io

# Context and Scope {#section-context-and-scope}

## Business Context {#_business_context}

**\<Diagram or Table\>**

**\<optionally: Explanation of external domain interfaces\>**

## Technical Context {#_technical_context}

**\<Diagram or Table\>**

**\<optionally: Explanation of technical interfaces\>**

**\<Mapping Input/Output to Channels\>**

# Solution Strategy {#section-solution-strategy}

# Building Block View {#section-building-block-view}

## Whitebox Overall System {#_whitebox_overall_system}

***\<Overview Diagram\>***

Motivation

:   *\<text explanation\>*

Contained Building Blocks

:   *\<Description of contained building block (black boxes)\>*

Important Interfaces

:   *\<Description of important interfaces\>*

### \<Name black box 1\> {#_name_black_box_1}

*\<Purpose/Responsibility\>*

*\<Interface(s)\>*

*\<(Optional) Quality/Performance Characteristics\>*

*\<(Optional) Directory/File Location\>*

*\<(Optional) Fulfilled Requirements\>*

*\<(optional) Open Issues/Problems/Risks\>*

### \<Name black box 2\> {#_name_black_box_2}

*\<black box template\>*

### \<Name black box n\> {#_name_black_box_n}

*\<black box template\>*

### \<Name interface 1\> {#_name_interface_1}

...

### \<Name interface m\> {#_name_interface_m}

## Level 2 {#_level_2}

### White Box *\<building block 1\>* {#_white_box_building_block_1}

*\<white box template\>*

### White Box *\<building block 2\>* {#_white_box_building_block_2}

*\<white box template\>*

...

### White Box *\<building block m\>* {#_white_box_building_block_m}

*\<white box template\>*

## Level 3 {#_level_3}

### White Box \<\_building block x.1\_\> {#_white_box_building_block_x_1}

*\<white box template\>*

### White Box \<\_building block x.2\_\> {#_white_box_building_block_x_2}

*\<white box template\>*

### White Box \<\_building block y.1\_\> {#_white_box_building_block_y_1}

*\<white box template\>*

# Runtime View {#section-runtime-view}

## \<Runtime Scenario 1\> {#_runtime_scenario_1}

- *\<insert runtime diagram or textual description of the scenario\>*
- *\<insert description of the notable aspects of the interactions between the building block instances depicted in this diagram.\>*

## \<Runtime Scenario 2\> {#_runtime_scenario_2}

## ...

## \<Runtime Scenario n\> {#_runtime_scenario_n}

# Deployment View {#section-deployment-view}

## Infrastructure Level 1 {#_infrastructure_level_1}

***\<Overview Diagram\>***

Motivation

:   *\<explanation in text form\>*

Quality and/or Performance Features

:   *\<explanation in text form\>*

Mapping of Building Blocks to Infrastructure

:   *\<description of the mapping\>*

## Infrastructure Level 2 {#_infrastructure_level_2}

### *\<Infrastructure Element 1\>* {#_infrastructure_element_1}

*\<diagram + explanation\>*

### *\<Infrastructure Element 2\>* {#_infrastructure_element_2}

*\<diagram + explanation\>*

...

### *\<Infrastructure Element n\>* {#_infrastructure_element_n}

*\<diagram + explanation\>*

# Cross-cutting Concepts {#section-concepts}

## *\<Concept 1\>* {#_concept_1}

*\<explanation\>*

## *\<Concept 2\>* {#_concept_2}

*\<explanation\>*

...

## *\<Concept n\>* {#_concept_n}

*\<explanation\>*

# Architecture Decisions {#section-design-decisions}

# Quality Requirements {#section-quality-scenarios}

## Quality Requirements Overview {#_quality_requirements_overview}

## Quality Scenarios {#_quality_scenarios}

# Risks and Technical Debts {#section-technical-risks}

# Glossary {#section-glossary}

| Term | Definition |
|---|---|
| *\<Term-1\>* | *\<definition-1\>* |
| *\<Term-2\>* | *\<definition-2\>* |
