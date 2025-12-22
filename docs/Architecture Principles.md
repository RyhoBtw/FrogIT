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

