# FrogIT – Test Plan (updated)

## 1. Introduction

This document summarizes the current testing process and testing progress of the FrogIT project.
The goal of the testing activities is to ensure stability, maintainability, and reliability of the application while development continues.

The focus of testing lies mainly on:

* Core application logic
* Audio functionality
* Resource handling
* Build and platform stability

GUI-related functionality is currently tested manually.

---

# 2. Test Strategy

Our testing strategy combines automated and manual testing approaches.

## Automated Testing

### Unit Tests

* Implemented using **Catch2**
* Integrated into **CTest**
* Covers runtime and compile-time logic

### Static Analysis

The following tools are integrated into the CI pipeline:

* clang-tidy
* cppcheck
* CodeQL

These tools help detect:

* Code quality issues
* Potential bugs
* Security vulnerabilities

### Coverage Analysis

Coverage tooling is integrated using:

* gcov
* llvm-cov
* OpenCppCoverage

### Planned Tests

The following test types are prepared but not fully implemented yet:

* Fuzz tests using LLVM libFuzzer
* Smoke tests for startup and initialization checks

---

## Manual Testing

GUI functionality implemented with Dear ImGui is tested manually, including:

* Sliders
* Buttons
* Frog interactions
* Window behavior

---

# 3. Test Plan

## Test Execution

Tests are automatically executed through the GitHub Actions CI pipeline:

* On every commit
* On every pull request

The pipeline currently performs:

* Build checks
* Unit test execution
* Static analysis
* Coverage generation

---

## Resources

### Frameworks & Tools

* CTest
* Catch2
* GitHub Actions
* clang-tidy
* cppcheck
* CodeQL

### Platforms

Tests are executed on:

* Windows
* Linux
* macOS

---

# 4. Test Cases

## Example Test Cases

| Test Case                     | Description                            | Status |
| ----------------------------- | -------------------------------------- | ------ |
| Audio playback initialization | Verify audio system startup            | Passed |
| Resource loading              | Verify loading of textures/audio files | Passed |
| Sound mixing logic            | Verify multiple sounds can be mixed    | Passed |
| Build pipeline                | Verify project builds on all platforms | Passed |
| GUI interaction               | Manual verification of UI controls     | Passed |

Additional test cases are implemented directly in the repository as Catch2 test files.

---

# 5. Test Results

The current test setup successfully validates the core functionality of the project.

## Current Results

* Unit tests execute successfully in CI
* Static analysis tools run without critical issues
* Cross-platform builds are stable
* Coverage generation works correctly

## Known Issues

* GUI testing is still mostly manual
* Fuzz tests are not fully implemented yet
* Coverage is still below the final target goal

---

# 6. Metrics

| Metric                | Current State         |
| --------------------- | --------------------- |
| Target test coverage  | 60–70%                |
| Current coverage      | Increasing            |
| Supported platforms   | Windows, Linux, macOS |
| Automated CI runs     | Every commit / PR     |
| Static analysis tools | 3 integrated          |

---

# 7. Recommendations

The following improvements are planned for future iterations:

* Expand unit test coverage
* Implement fuzz tests completely
* Add smoke tests for startup validation
* Improve automated GUI testing where feasible
* Increase coverage of edge cases and error handling

---

# 8. Conclusion

A solid testing infrastructure has been established for FrogIT.
Automated testing, static analysis, and CI integration already provide reliable feedback during development.

Although some planned testing features are still in progress, the current setup already improves software quality significantly and helps detect regressions early.
