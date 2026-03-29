---
theme: default
---

# 🐸 FrogIT

### Entspannung neu definiert – mit Sound und Fröschen

**Team:** Moritz · Luca · Jonas · Hilda

---

## Agenda

1. 🎯 Projektziel & Vision — *Moritz*
2. 🏗️ Architekturentscheidungen — *Jonas*
3. 🧩 Designprinzipien — *Hilda*
4. 🔧 Tech-Stack & Plattformen — *Luca*
5. 🖥️ Live-Demo — *Luca*
6. 📊 Projektmanagement & Erkenntnisse — *Moritz*
7. ❓ Fragen

---

<!-- Sprecher: Moritz (~5 Min.) -->

## 1. Projektziel & Vision

---

### Was ist FrogIT?

Eine **Desktop-Anwendung zur Entspannung**, die zwei Sinne anspricht:

🔊 **Audio** – Individuelle Ambient-Soundkulissen erstellen
(Regen, Wind, Frösche, Feuer-Knistern, …)

🐸 **Visuell** – Animierte Frösche springen über den Desktop

> *"Ein Ort zum Abschalten – beim Einschlafen, Lernen oder Arbeiten."*

---

### Kernfeatures

| Feature | Beschreibung |
|---------|-------------|
| **Sound-Mixer** | Mehrere Sounds mischen, Lautstärke individuell regeln |
| **Frosch-Animation** | Frösche bewegen sich über den gesamten Desktop |
| **Interaktive Frösche** | Klick → lustiger Spruch oder Quaken |
| **Frosch-Trolling** | Frösche fangen den Mauszeiger oder explodieren |
| **Presets** | Soundkulissen speichern und laden |

---

### Use-Case-Diagramm

![[use_case_diagram.jpeg]]

**Demo-Fokus:**
- ⭐ Soundkulisse erstellen & mischen (Demo 1)
- ⭐ Frosch-Animation auf dem Desktop (Demo 2)

---

### Zielgruppe & Motivation

- 🎓 Studierende beim Lernen
- 💤 Entspannung vor dem Einschlafen
- 💻 Hintergrund-Atmosphäre beim Arbeiten

**Alleinstellungsmerkmal:**
Kombination aus anpassbarem Audio **und** visuellem Feedback
→ Multi-sensorisches Erlebnis

---

<!-- Sprecher: Jonas (~5 Min.) -->

## 2. Architekturentscheidungen

---

### Qualitätsbaum – Priorisierung

```
                    Qualität
                       │
         ┌─────────┬───┴───┬──────────┬──────────┐
     Usability  Perform. Zuverl.  Wartbark.  Sicherh.
         │         │        │        │          │
        Q1,Q2    ★Q3      Q4      ★Q5         Q6
```

**★ Höchste Priorität:**
- **Q3:** Low-Latency Audio (≤ 50 ms Reaktionszeit)
- **Q5:** Wartbarkeit & Erweiterbarkeit

---

### ADR 0001 – Dedizierter Audio-Thread

**Problem:** Audio + UI auf einem Thread → Stottern & Lag

**Entscheidung:** Multi-Threading mit dediziertem Audio-Thread

| ✅ Vorteile | ❌ Nachteile |
|------------|-------------|
| Stabiles, latenzfreies Audio | Höhere Code-Komplexität |
| UI bleibt reaktionsfähig | Synchronisation nötig |
| Einfache Integration mit SFML | Mehr Testing (Race Conditions) |

**Ziel:** ≤ 50 ms End-to-End-Reaktionszeit

---

### ADR 0002 – SFML + Dear ImGui

**Problem:** Welche Bibliotheken für Audio, Grafik und UI?

**Optionen:**

| Option | Bewertung |
|--------|-----------|
| **SFML + ImGui** ✅ | Leichtgewichtig, schnell erlernbar |
| SDL2 + ImGui | Mehr Boilerplate, komplexere API |
| Qt | Zu umfangreich für den Projektrahmen |
| Custom | Zu hoher Aufwand |

**Argument:** Minimaler Boilerplate, Open Source, passt zum Projektumfang

---

### ADR 0003 – JSON für Presets

**Problem:** In welchem Format sollen Soundkulissen gespeichert werden?

**Optionen:** JSON · XML · Binär · YAML · TOML

**Entscheidung:** JSON

| ✅ Vorteile | ❌ Nachteile |
|------------|-------------|
| Menschenlesbar & editierbar | Größer als Binärformat |
| Breite C++-Unterstützung | Schwächere Typisierung |
| Einfaches Debugging | |

→ Nutzt `nlohmann/json`-Bibliothek

---

<!-- Sprecher: Hilda (~5 Min.) -->

## 3. Designprinzipien

---

### Composition over Inheritance

Statt komplexer Vererbungshierarchien nutzen wir **Komposition**:

```
FrogApp ──▶ FrogSettingsUI ──▶ Soundscape ──▶ Sound[]
   │
   ▼
  Frog ──▶ Position
       ──▶ Resource
```

**Vorteile:**
- Klare, einfache Lebenszeiten der Objekte
- Flexible Kombination von Komponenten
- Leichter testbar und erweiterbar

---

### Single Responsibility Principle (SRP)

Jede Klasse hat **eine** klar definierte Aufgabe:

| Klasse | Verantwortung |
|--------|---------------|
| `FrogApp` | Fenster-Management, Render-Loop |
| `Frog` | Frosch-Entität (Position, Animation) |
| `Soundscape` | Audio-Mischung & Steuerung |
| `Sound` | Ein einzelner Audioclip |
| `ResourceManager` | Laden & Cachen aller Assets |
| `FrogSettingsUI` | Benutzeroberfläche (Slider, Profile) |

---

### Modularität & Lazy Loading

**Modulare Asset-Struktur:**
```
assets/
├── textures/    → Frosch-Sprites, UI-Frames
├── sounds/      → Audio-Dateien
└── animation/   → Animationsdaten
```

**ResourceManager – Lazy Loading:**
- Ressourcen werden **erst bei Bedarf** geladen
- Gecacht in `unordered_map` für Wiederverwendung
- Mehrere Frösche teilen sich **eine** Textur im Speicher

→ Neue Ressource hinzufügen = **eine Datei** in den Ordner legen

---

### Klassendiagramm

![[Class_FrogApp.png]]

Aufgebaut nach dem **Kompositionsprinzip** mit klarer Trennung:
- Jede Klasse ist eigenständig und austauschbar
- Resources werden über einen Enum-Typ kategorisiert (`SOUND`, `ANIMATION`, `TEXTURE`)

---

### Verhaltensmodellierung

**Frosch-Animation:**
![[Behavior_AnimateFrog.png]]

**Frosch-Interaktion (Klick):**
![[Behavior_FrogOnClick.png]]

---

<!-- Sprecher: Luca (~5 Min.) -->

## 4. Tech-Stack & Plattformen

---

### Kerntechnologien

| Schicht | Technologie | Version |
|---------|------------|---------|
| **Sprache** | C++ | C++23 |
| **Audio & Grafik** | SFML | 3.0.1 |
| **GUI** | Dear ImGui + ImGui-SFML | v1.91.1 / v3.0 |
| **Formatierung** | fmtlib | 11.1.4 |
| **Build-System** | CMake + Ninja | 3.21+ |
| **Testing** | Catch2 | 3.8.1 |
| **Paket-Mgmt** | CPM + FetchContent | — |

---

### Qualitätssicherung & CI/CD

```
         Pull Request / Push
                │
    ┌───────────┼───────────┐
    ▼           ▼           ▼
clang-format   CI Build   CodeQL
(Auto-Fix)     Pipeline   Security
                │
    ┌───────┬───┼───┬───────┐
    ▼       ▼   ▼   ▼       ▼
  Linux   macOS Win  Tests  Coverage
  GCC14   Clang MSVC Catch2 gcovr
  Clang19
```

**Tools:** AddressSanitizer · UBSan · cppcheck · clang-tidy · libFuzzer

---

### Cross-Platform-Support

| Plattform | Compiler | Besonderheiten |
|-----------|----------|----------------|
| **Windows** | MSVC (VS 2022) | Win32 API für Fenster-Transparenz |
| **Linux** | GCC 14 / Clang 19 | X11 für Desktop-Overlay |
| **macOS** | Apple Clang | Objective-C Bridge für OS-Features |

- Plattformspezifischer Code hinter **einheitlicher API**
- Automatisierte Builds für **alle 3 Plattformen** in der CI

---

### CI/CD-Pipeline im Detail

| Workflow | Trigger | Funktion |
|----------|---------|----------|
| `ci.yml` | PR, Push, Tags | Multi-Platform Build & Test (72+ Konfigurationen) |
| `auto-clang-format.yml` | PR | Automatische Code-Formatierung |
| `codeql.yml` | PR, Push, wöchentlich | Security-Scanning |
| `dependabot.yml` | Automatisch | Dependency-Updates |

**Release:** CPack generiert plattformspezifische Artefakte bei Tags

---

## 5. Live-Demo

### Aktueller Status

- ✅ Settings-UI mit ImGui (Slider für Lautstärke & Frequenz)
- ✅ Transparentes Frosch-Fenster auf dem Desktop
- ✅ ResourceManager für Texturen
- ✅ Custom Window-Frame (Drag & Drop, Minimize, Close)
- ✅ Cross-Platform-Builds
- 🔄 Audio-Playback (in Arbeit)
- 🔄 Frosch-Animationen (in Arbeit)

> **→ Live-Vorführung der Anwendung**

---

<!-- Sprecher: Moritz (~5 Min.) -->

## 6. Projektmanagement & Erkenntnisse

---

### Scrum-Setup

| Aspekt | Details |
|--------|---------|
| **Methodik** | Scrum (agil) |
| **Sprint-Länge** | 1 Woche |
| **Rituale** | Planning, Weeklies, Reviews, Retros |
| **Tool** | GitHub Projects (vorher YouTrack) |
| **Blog** | GitHub Discussions (8 Blog Posts) |
| **Kommunikation** | Regelmäßige Meetings |

---

### Teamaufteilung & Beiträge

| Mitglied | Rolle | Commits | Schwerpunkte |
|----------|-------|--------:|-------------|
| **Moritz** | Orga & Kommunikation | — | Sprint-Planung, Issues, Qualitätsbaum, Architektur-Docs |
| **Luca** | Hauptentwickler | 50 | C++-Implementierung, SFML/ImGui, CI/CD, Cross-Platform |
| **Jonas** | Dokumentation | 7 | Blog Posts, ADRs, Sequenzdiagramme, Kanban |
| **Hilda** | Design & Recherche | — | Sprites, Frosch-Designs, Sequenzdiagramme, Klassen |

---

### Projektzeitlinie

```
Okt 2025          Nov 2025          Dez 2025
│                 │                 │
▼                 ▼                 ▼
┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
│Anforder-│ │Planung &│ │Implemen-│ │Implemen-│ │Architek-│
│ungen &  │ │Entwurf  │ │tierung  │ │tierung  │ │tur &    │
│SRS      │ │Diagramme│ │Demo 1   │ │Demo 2   │ │Abschluss│
└─────────┘ └─────────┘ └─────────┘ └─────────┘ └─────────┘
Sprint 1-2   Sprint 3-4  Sprint 5-6  Sprint 7-8  Sprint 9-10
```

---

### Fakten zum Projekt

| Metrik | Wert |
|--------|------|
| **Projektlaufzeit** | ~10 Wochen (10 Sprints) |
| **Geschätzte Gesamtstunden** | ~175 h |
| **Commits** | 57 |
| **GitHub Issues** | 34 |
| **Blog Posts** | 8 |
| **ADRs** | 3 |
| **CI/CD-Konfigurationen** | 72+ (OS × Compiler × Build-Typ) |
| **Unterstützte Plattformen** | 3 (Windows, Linux, macOS) |

---

### Lessons Learned

**Was gut lief:** ✅
- Frühe Investition in CI/CD hat sich ausgezahlt
- Cross-Platform von Anfang an → weniger Überraschungen
- Klare Rollenverteilung im Team
- Wechsel von YouTrack zu GitHub Projects → alles an einem Ort

**Was wir anders machen würden:** 🔄
- Früher mit der Feature-Implementierung beginnen
- Mehr parallele Arbeit an Features
- Audio-Integration früher priorisieren

---

### Gewonnene Erkenntnisse

> 🐸 *"Wie echte Frösche gedeiht FrogIT in einem sauberen Ökosystem."*

1. **Infrastruktur first** – CI/CD-Pipeline spart langfristig Zeit
2. **Composition > Inheritance** – Einfacherer, flexiblerer Code
3. **Cross-Platform ist machbar** – aber plattformspezifischer Code ist unvermeidlich
4. **Dokumentation parallel** – ADRs und Blog Posts helfen beim Reflektieren
5. **Agil bleiben** – Wöchentliche Sprints erzwingen regelmäßigen Fortschritt

---

## Vielen Dank! 🐸

### Fragen?

**Links:**
- 🔗 Repository: [github.com/RyhoBtw/FrogIT](https://github.com/RyhoBtw/FrogIT)
- 📖 Blog: [GitHub Discussions](https://github.com/RyhoBtw/FrogIT/discussions)
- 📋 SRS: [docs/SoftwareRequirementsSpecification.md](https://github.com/RyhoBtw/FrogIT/blob/main/docs/SoftwareRequirementsSpecification.md)
