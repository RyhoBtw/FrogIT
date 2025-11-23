## Focus
We concentrate on two high-priority scenarios:
1. **Low-latency audio playback** (Scenario A)
2. **Maintainability and extensibility** (Scenario D)
## Strategy for Scenario A — Low-latency audio playback
### Goals
- Keep UI responsive while audio plays.
- Apply user controls (volume, enable/disable tracks) with perceptible immediacy (<50ms).
### Architectural Tactics
- **Separate audio thread(s):** Audio playback and mixing run on a dedicated real-time-friendly thread separate from the UI thread.
- **Prioritized thread scheduling:** Where possible, give the audio thread higher scheduling priority.
- **Audio library selection:** Use an audio library with low-latency support (SFML/underlying OS audio backends).
### Expected trade-offs
- Complexity in concurrent code.
- Care with resource loading: load heavy resources asynchronously before playback.
## Strategy for Scenario D — Maintainability & Extensibility
### Goals
- Make it easy to add new sound types, resources, and animations with minimal code changes.
- Keep testing and CI straightforward.
### Architectural Tactics
- **Define single-file resources:** Have every resource be implemented in a single file and managed by the `ResourceManager`
- **Modular folder structure:** `/assets/soudns/`, `/assets/animation/`, `/assets/textures/`,  to keep resourcetypes separate.
- **Automated CI checks:** compile, unit tests, and static analysis on PRs.
- **Code review policy** and well-defined ADRs for major changes.
### Expected trade-offs
- Slight initial overhead to design interfaces and factories, but large long-term gain in extensibility.

---

# **ADR 0001 — Use a Dedicated Audio Thread for Low-Latency Playback**
## Context and Problem Statement

FrogIT must play ambient sounds in real time while the user interface remains responsive.  
If audio and UI run on the same thread, audio stutters and UI lag occur.  
**Question:** How can we ensure low-latency audio playback even when the UI is under load?
## Considered Options
- Single-threaded model (UI + audio together)
- Multi-threading with a dedicated audio thread
- External audio backend with real-time prioritization
- Event-based audio processing without its own thread
## Decision
Chosen option: **“Multi-threading with a dedicated audio thread”**, because it is the only option that reliably ensures real-time audio playback while keeping the UI smooth, and it integrates easily with SFML.
## Status
Accepted
## Consequences
- Positive: **Stable, low-latency audio playback**, even under UI load
- Positive: **UI remains responsive**
- Negative: **Higher complexity** due to synchronization and threading
- Negative: Requires **more testing** for race conditions

# **ADR 0002 — Use SFML for Audio and Dear ImGui for the UI**
## Context and Problem Statement
FrogIT needs audio playback, rendering, and UI components. The tools must be lightweight, C++ friendly, and fast to integrate.  
**Question:** Which libraries best support fast development, good performance, and maintainability?
## Considered Options
- SFML + Dear ImGui
- SDL2 + ImGui
- Qt (full framework)
- Custom UI + custom audio backend
## Decision
Chosen option: **“SFML + Dear ImGui”**, because this combination minimizes boilerplate, is easy to learn, is lightweight, open source, and fits the project scope.
## Status
Accepted
## Consequences
- Positive: **Fast prototyping**, minimal complexity
- Positive: **SFML is easy to use** and beginner friendly
- Negative: SFML does **not guarantee the lowest possible audio latency**
- Negative: ImGui is **not a native desktop UI**, it uses an immediate-mode model

# **ADR 0003 — Use JSON as the Format for Saving Presets**
## Context and Problem Statement
Users want to save and load sound profiles. The format must be structured, editable, and stable.  
**Question:** Which format is best for storing soundscape presets?
## Considered Options
- JSON
- XML
- Binary format
- YAML
- TOML
## Decision
Chosen option: **“JSON”**, because it is simple, human-readable, widely supported, and ideal for configuration data using libraries like nlohmann/json.
## Status
Accepted
## Consequences
- Positive: **Easy debugging** and manual editing
- Positive: Many stable C++ JSON parsers exist
- Negative: JSON is **larger** and slightly **slower** than binary formats
- Negative: JSON has **weaker typing** compared to XML or TOML

