# 🐸 FrogIT – Projekt-Handout

## Projektname: FrogIT

**FrogIT** ist eine Desktop-Anwendung zur Entspannung, mit der Benutzer individuelle Ambient-Soundkulissen erstellen und animierte Frösche auf ihrem Bildschirm beobachten können. Die Anwendung kombiniert anpassbare Audio-Umgebungen mit visuellem Feedback in Form von springenden, interaktiven Fröschen.

**Repository:** [github.com/RyhoBtw/FrogIT](https://github.com/RyhoBtw/FrogIT)  
**Zeitraum:** Oktober 2025 – Dezember 2025  
**Team:** 4 Personen

---

## 1. Statistik

### 1.1 Stunden pro Person & Hauptbeiträge

| Person | Rolle | Geschätzte Stunden | Hauptbeiträge |
|--------|-------|-------------------:|---------------|
| **Moritz** | Organisation, Kommunikation, Development | ~45 h | Projektorganisation, Issue-Management, GitHub Projects Setup, Sprint-Planung, Qualitätsbaum & Architekturentscheidungen, Blog-Kommentare, Aktivitätsdiagramm |
| **Luca** | Technische Recherche, Hauptentwickler | ~65 h | Gesamte C++-Implementierung (50 Commits), SFML/ImGui-Integration, UI-Entwicklung, ResourceManager, Frog-Klasse, CI/CD-Pipeline, Cross-Platform-Builds, Framework-Recherche, Klassendiagramm |
| **Jonas** | Blog Posts, Kanban Board, Development | ~35 h | 8 Blog Posts, ADRs (3 Stück), Sequenzdiagramme, Kanban-Board-Pflege, Diagramm-Uploads, SRS-Mitarbeit |
| **Hilda** | Technische Recherche, Development | ~30 h | Sprite- & Frosch-Designs, Sequenzdiagramme, Klassen-Implementierung (nach Klassendiagramm), Blog Posts & Kommentare, Recherche |
| **Gesamt** | | **~175 h** | |

### 1.2 Stunden pro Disziplin

| Disziplin | Geschätzte Stunden | Anteil |
|-----------|-------------------:|-------:|
| Anforderungen (SRS, User Stories, Use Cases) | ~20 h | 11 % |
| Analyse & Entwurf (UML-Diagramme, Klassendiagramm, Sequenz-/Aktivitätsdiagramme) | ~25 h | 14 % |
| Implementierung (C++-Code, SFML/ImGui, ResourceManager, UI) | ~55 h | 32 % |
| Test & Qualitätssicherung (Catch2-Tests, Fuzz-Tests, Sanitizer, CodeQL) | ~15 h | 9 % |
| CI/CD & Infrastruktur (GitHub Actions, Cross-Platform-Builds, Caching) | ~25 h | 14 % |
| Dokumentation (Blog Posts, ADRs, Architekturprinzipien, Qualitätsbaum) | ~25 h | 14 % |
| Projektmanagement (Scrum, Sprint-Planung, Organisation) | ~10 h | 6 % |

### 1.3 Stunden pro Phase

| Phase | Zeitraum | Geschätzte Stunden | Aktivitäten |
|-------|----------|-------------------:|-------------|
| **Anforderungsdefinition** | Okt 2025 (Sprint 1–2) | ~25 h | SRS erstellen, Use-Case-Diagramm, Mockups, User Stories, Framework-Recherche |
| **Planung & Entwurf** | Okt–Nov 2025 (Sprint 3–4) | ~35 h | Klassen-/Sequenz-/Aktivitätsdiagramme, Architekturentwurf, C++-Template-Setup, Build-System |
| **Implementierung & Demo 1** | Nov 2025 (Sprint 5–6) | ~55 h | SFML/ImGui-Integration, UI-Entwicklung, ResourceManager, Frog-Rendering, Hello World, Cross-Platform |
| **Implementierung & Demo 2** | Nov–Dez 2025 (Sprint 7–8) | ~35 h | Frog-Sprites als OS-Fenster, Transparenz, Desktop-Overlay, CI/CD-Ausbau |
| **Architektur & Abschluss** | Dez 2025 (Sprint 9–10) | ~25 h | ASRs, ADRs, Qualitätsbaum, Architekturprinzipien, finale Dokumentation |

---

## 2. Gesamtanwendungsfalldiagramm

Das Use-Case-Diagramm befindet sich unter: [`docs/use_case_diagram.jpeg`](use_case_diagram.jpeg)

```
┌─────────────────────────────────────────────────────────────────┐
│                          FrogIT System                          │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  ★ Soundkulisse erstellen & mischen                     │   │
│  │    (Sounds auswählen, Lautstärke regeln, Play/Pause)    │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  ★ Frosch-Animation beobachten                          │   │
│  │    (Frösche springen über den Desktop)                   │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │    Interaktive Frösche                                   │   │
│  │    (Klick → lustiger Spruch / Quaken)                    │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │    Frosch-Trolling                                       │   │
│  │    (Mauszeiger fangen, Explosion bei Klick)              │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │    Soundscape-Presets speichern/laden                     │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │    Frosch-Geschwindigkeit einstellen                     │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │    Letzte Auswahl speichern & wiederherstellen           │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
         │
    ┌────┴────┐
    │ Benutzer│
    └─────────┘

★ = Fokus der Entwicklung (Demo)
```

**Fokus bei der Entwicklung (Demo):**
- ★ **Soundkulisse erstellen & mischen** – UI mit Slidern für Lautstärke und Frequenz (Demo 1)
- ★ **Frosch-Animation beobachten** – Frösche als transparente OS-Fenster auf dem Desktop (Demo 2)

---

## 3. Architekturstil, Architekturentscheidungen & Hauptargumente

### 3.1 Architekturstil

**Komponentenbasierte Architektur mit Komposition statt Vererbung (Composition over Inheritance)**

Die Anwendung folgt einem modularen, komponentenbasierten Ansatz:

```
┌─────────────┐     ┌────────────────┐     ┌─────────────────┐
│   FrogApp    │────▶│ FrogSettingsUI │────▶│   Soundscape    │
│  (Hauptapp)  │     │   (GUI/ImGui)  │     │  (Audio-Mixer)  │
└──────┬───────┘     └────────────────┘     └────────┬────────┘
       │                                             │
       ▼                                             ▼
┌─────────────┐                              ┌─────────────┐
│    Frog     │                              │    Sound    │
│  (Entität)  │                              │  (Audioclip)│
└──────┬───────┘                              └──────┬──────┘
       │                                             │
       ▼                                             ▼
┌──────────────────────────────────────────────────────────┐
│                   ResourceManager                        │
│        (Texturen, Sounds, Animationen – Lazy Loading)    │
└──────────────────────────────────────────────────────────┘
```

### 3.2 Architekturentscheidungen (ADRs)

#### ADR 0001 – Dedizierter Audio-Thread für Low-Latency-Playback
- **Entscheidung:** Multi-Threading mit dediziertem Audio-Thread
- **Argument:** Audio und UI auf getrennten Threads verhindern Stottern und UI-Lag; Ziel: ≤ 50 ms Reaktionszeit
- **Trade-off:** Höhere Komplexität durch Synchronisation, mehr Testing für Race Conditions erforderlich

#### ADR 0002 – SFML für Audio & Dear ImGui für die UI
- **Entscheidung:** SFML + Dear ImGui als Technologie-Stack
- **Argument:** Minimaler Boilerplate, leichtgewichtig, einsteigerfreundlich, Open Source, passt zum Projektumfang
- **Trade-off:** SFML garantiert nicht die niedrigste Audio-Latenz; ImGui ist kein natives Desktop-UI

#### ADR 0003 – JSON als Preset-Format
- **Entscheidung:** JSON für das Speichern von Soundscape-Presets
- **Argument:** Einfach, menschenlesbar, weit verbreitet, gut unterstützt durch C++-Bibliotheken (nlohmann/json)
- **Trade-off:** Größer und etwas langsamer als Binärformate

### 3.3 Architekturprinzipien

| Prinzip | Beschreibung |
|---------|-------------|
| **Composition over Inheritance** | Klare, einfache Lebenszeiten; flexible Kombination von Komponenten |
| **Single Responsibility Principle** | Jede Klasse hat eine klar definierte Verantwortung |
| **Modularität** | Getrennte Asset-Ordner (`/assets/sounds/`, `/assets/textures/`, `/assets/animation/`) |
| **Lazy Loading** | Ressourcen werden erst bei Bedarf geladen und gecacht (ResourceManager) |
| **Cross-Platform** | Plattformspezifische Implementierungen (Win32, X11, Objective-C) hinter einheitlicher API |

### 3.4 Qualitätsprioritäten (ASRs)

| Priorität | Qualitätsmerkmal | Ziel |
|-----------|-----------------|------|
| **Hoch** | Performance – Low-Latency Audio | ≤ 50 ms Reaktionszeit, keine Buffer-Unterläufe |
| **Hoch** | Wartbarkeit & Erweiterbarkeit | Neue Ressource = eine Datei hinzufügen |
| **Mittel** | Zuverlässigkeit | Graceful Handling bei fehlenden Dateien |
| **Mittel** | Usability | Nicht-intrusives Overlay, Toggle ≤ 200 ms |
| **Niedrig** | Sicherheit/Datenschutz | Nur lokale Speicherung, kein Netzwerkverkehr |

---

## 4. Verwendete Softwaretools, Plattformen & Techniken

### 4.1 Entwicklung

| Kategorie | Tool/Technologie | Zweck |
|-----------|-----------------|-------|
| **Sprache** | C++ (Standard C++23) | Hauptprogrammiersprache |
| **Audio & Grafik** | SFML 3.0.1 | Multimedia-Framework für Audio-Playback und Rendering |
| **GUI** | Dear ImGui v1.91.1 + ImGui-SFML v3.0 | Immediate-Mode GUI-Framework |
| **Formatierung** | fmt (fmtlib) 11.1.4 | Typ-sichere String-Formatierung |

### 4.2 Build & Infrastruktur

| Kategorie | Tool/Technologie | Zweck |
|-----------|-----------------|-------|
| **Build-System** | CMake 3.21+ | Cross-Platform-Build-Konfiguration |
| **Build-Generator** | Ninja / Visual Studio 2022 | Schnelle Builds |
| **Paket-Manager** | CPM (CMake Package Manager) + FetchContent | Automatische Dependency-Auflösung |
| **Compiler** | GCC 14, Clang 19.1.1, MSVC (VS 2022) | Multi-Compiler-Support |
| **Caching** | ccache | Beschleunigte Rebuilds |

### 4.3 Qualitätssicherung

| Kategorie | Tool/Technologie | Zweck |
|-----------|-----------------|-------|
| **Unit-Testing** | Catch2 3.8.1 | Testframework für C++ |
| **Fuzz-Testing** | libFuzzer | Zufallseingabe-Tests mit Sanitizern |
| **Statische Analyse** | cppcheck, clang-tidy v19 | Code-Qualität & Fehlersuche |
| **Code-Formatierung** | clang-format v19 | Einheitlicher Coding-Style |
| **Sanitizer** | AddressSanitizer, UBSan, TSan | Speicher-/Threading-/UB-Fehler |
| **Security-Scanning** | CodeQL | Semantische Sicherheitsanalyse |
| **Code-Coverage** | gcovr / llvm-cov / OpenCppCoverage | Testabdeckung messen |

### 4.4 CI/CD & DevOps

| Kategorie | Tool/Technologie | Zweck |
|-----------|-----------------|-------|
| **CI/CD** | GitHub Actions | Automatisierte Builds, Tests, Releases |
| **Sekundäre CI** | GitLab CI | Parallele Build-Pipeline (GCC + Clang) |
| **Dependency-Updates** | Dependabot | Automatische Aktualisierung von Abhängigkeiten |
| **Packaging** | CPack | Plattformspezifische Release-Artefakte |

### 4.5 Projektmanagement & Dokumentation

| Kategorie | Tool/Technologie | Zweck |
|-----------|-----------------|-------|
| **Versionskontrolle** | Git / GitHub | Code-Verwaltung & Zusammenarbeit |
| **Projektmanagement** | GitHub Projects (vorher YouTrack) | Scrum-Board, Sprint-Planung |
| **Scrum** | 1-Wochen-Sprints | Agile Entwicklungsmethodik |
| **Blog** | GitHub Discussions | Team-Blog mit Fortschrittsberichten |
| **Diagramme** | draw.io | UML-Diagramme (Aktivität, Sequenz, Klassen) |
| **Dev-Environment** | VS Code DevContainers | Einheitliche Entwicklungsumgebung |

### 4.6 Zielplattformen

| Plattform | Compiler | Status |
|-----------|----------|--------|
| **Windows** | MSVC (Visual Studio 2022) | ✅ Unterstützt |
| **Linux** | GCC 14 / Clang 19 | ✅ Unterstützt |
| **macOS** | Apple Clang | ✅ Unterstützt |

---

*Erstellt am 29. März 2026 – FrogIT-Team*
