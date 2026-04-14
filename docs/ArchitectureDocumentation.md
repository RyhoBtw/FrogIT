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

# 3 Context and Scope {#section-context-and-scope}

## 3.1 Business Context {#_business_context}

| Actor             | Interaction                                              |
| ----------------- | -------------------------------------------------------- |
| User              | Creates and customizes soundscapes, interacts with frogs |
| Local File System | Stores and loads presets and audio resources             |
**Explanation**:
The user interacts directly with FrogIT to create and control soundscapes.
All data (e.g. presets, audio files) is stored locally on the system.
There are no external services or network dependencies.

## 3.2 Technical Context {#_technical_context}

| Component              | Interface                         |
| ---------------------- | --------------------------------- |
| User Interface (ImGui) | User input (mouse, keyboard)      |
| Audio System (SFML)    | Playback of sounds files          |
| File System            | Read/write presets and resources  |
| Operating System       | Window management, input handling |
### Mapping Input / Output to Channels

| Input / Output               | Channel                  |
| ---------------------------- | ------------------------ |
| User input (clicks, sliders) | GUI (Dear ImGUI)         |
| Audio output                 | Speakers via SFML        |
| Visual Output (frogs, UI)    | Screen                   |
| Preset data                  | Local file system (JSON) |


# 4 Solution Strategy {#section-solution-strategy}
The architecture of FrogIT follows a modular and layered approach to ensure maintainability, extensibility and performance

## Key Design Decisions
- **Layered Architecture**
	Separation into UI, application logic and resource handling to keep responsibilities clearly structured.
- **Modular Design**
	Components such as sound management, animation and UI are implemented independently to allow easy extension.
- **Dedicated Audio Handling**
	Audio processing is separated from the UI to ensure smooth, low-latency playback.
- **Observer Pattern**
	Used to connect audio events with frog animations while keeping components loosely coupled.
- **Asynchronous Resource Handling**
	Resources such as audio files are loaded without the application.

## Technology Choices
- C++ as main programming language
- SFML for audio playback and basic rendering
- Dear ImGui for the graphical user interface
- CMake for build configuration
- GitHub Actions for CI/CD

## Quality Focus
The solution strategy is mainly driven by the following quality goals:
- **Performance**: real-time audio without interruptions
- **Maintainability**: clean structure and separation of concerns
- **Usability**: simple and intuitive interaction
- **Reliability**: stable handling of resources and playback

# 5 Building Block View {#section-building-block-view}

## Level 1: Whitebox Overall System

The system as seen from the outside, decomposed into its top-level building blocks. External actors and systems are shown at the boundary.

![component1](comp1.png)  

### Black Box Descriptions

| Building Block | Responsibility |
|---|---|
| **FrogApp** | Application orchestrator — window lifecycle, frog collection, event loop, rendering |
| **Frog** | Individual frog entity — animation, sprite state, user interaction |
| **ResourceManager** | Singleton asset cache — preloads and caches textures, images, sounds |
| **SoundScape** | Audio controller — manages sound playback (implementation pending) |
| **FrogSettingsUI** | User configuration UI (placeholder for future) |
| **Constants** | Centralised config values (framerate, dimensions, scaling, etc.) |

### External Systems

| System | Description |
|---|---|
| **Desktop User** | Interacts with frogs via mouse clicks on the desktop overlay |
| **SFML Graphics** | Windowing, sprite, and texture rendering library |
| **SFML Audio** | Sound and sound-buffer playback library |
| **OS Desktop** | Host operating system providing the transparent overlay surface |

## Level 2: Whitebox FrogApp & Frog

Zooms into the two most important Level 1 building blocks.

![component2](comp2.png)  

### Black Box Descriptions — FrogApp Internals

| Building Block | Responsibility |
|---|---|
| **Window Manager** | Creates and configures the transparent render window with desktop integration |
| **Frog Collection** | Owns vector of active Frog entities via `unique_ptr` |
| **Event Processor** | Handles OS events (close, mouse click) and routes them to frogs |
| **Render Pipeline** | Updates all frogs each frame, renders sprites to window |
| **Speech Bubble Mgr** | Manages overlay window for frog dialogue display |

### Black Box Descriptions — Frog Internals

| Building Block | Responsibility |
|---|---|
| **Animation State** | Manages hop sequences, timing, and arc calculation |
| **Sprite Manager** | Handles position, scale, and sprite rendering |
| **Speech State** | Controls speech bubble visibility and text content |

## Level 3: Whitebox Animation State & Event Processor

Zooms into the most complex Level 2 blocks.

![component3](comp3.png)  

### Black Box Descriptions

| Building Block | Responsibility |
|---|---|
| **Hop Calculator** | Computes parabolic arc position using elapsed time |
| **Position Updater** | Applies delta-time based position updates |
| **Frame Selector** | Switches between open/closed mouth textures during animation |
| **Click Routing** | Performs hit-test on all frogs and dispatches click to matching entity |

Here's the complete Chapter 6. Building block names match Chapter 5 throughout.

# 6 Runtime View {#section-runtime-view}

This section shows how the building blocks from the [Building Block View](#section-building-block-view) collaborate at runtime. Scenarios are chosen for architectural relevance, not completeness.

## 6.1 Application Startup

How FrogIT initialises itself from launch to first rendered frame.

![sequence1](sequence1.png)

**Notable aspects:**

- **ResourceManager** loads all assets **once** at startup — frogs only hold references, never duplicate textures.
- The render window is created with OS-level transparency flags so the frog sits directly on the desktop.
- Frog instances are heap-allocated via `unique_ptr` and owned by the **Frog Collection** inside FrogApp.

## 6.2 Main Game Loop (Single Frame)

The core loop that runs every frame (~60 fps). This is the heartbeat of the application.

![sequence2](sequence2.png)

**Notable aspects:**

- The loop follows a strict **Event → Update → Render** order each frame.
- **Delta-time** is used for all movement so animation speed is independent of framerate.
- The Render Pipeline iterates the Frog Collection twice: once for update, once for draw.

## 6.3 User Clicks a Frog

What happens when the desktop user clicks on a frog — from OS event to speech bubble.

![sequence3](sequence3.png)

**Notable aspects:**

- **Click Routing** performs a hit-test against all frogs in the collection; the **first** matching frog handles the event.
- The **Speech Bubble Mgr** creates a separate small overlay window positioned near the frog — it is not drawn inside the main render window.
- Sound playback is fire-and-forget via **SoundScape**.

## 6.4 Frog Hop Animation

How a single hop is calculated and rendered across multiple frames.

![sequence4](sequence4.png)

**Notable aspects:**

- The hop arc uses a **parabolic interpolation**: $y = \text{lerp}(y_0, y_1, t) - h \cdot 4t(1-t)$, giving a smooth jump curve.
- **Frame Selector** swaps the mouth texture at the midpoint of the hop — creating the illusion of a croak during the jump.
- After a hop completes, the Animation State picks a random idle delay before scheduling the next hop.

**Exception:** If the calculated landing position is off-screen, the hop target is clamped to the nearest screen edge.

# 7 Deployment View {#section-deployment-view}

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
