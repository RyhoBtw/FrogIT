# FrogIT – Test Plan (updated)

## Test Types

### Unit Tests (implemented, expanding)
- Using Catch2 with CTest integration  
- Includes seperation of runtime and compile-time tests

### Fuzz Tests (optional, planned)
- Enabled via `FrogIT_BUILD_FUZZ_TESTS`  
- Based on LLVM libFuzzer (with ASan/TSan)  
- Infrastructure exists, implementation in progress  

### Smoke Tests (planned)
- Can be implemented with existing CTest setup  
- Example: start application, basic initialization checks  

### Manual GUI Tests
- UI (Dear ImGui) tested manually  

### Static Analysis (implemented)
- clang-tidy, cppcheck (linting), CodeQL (security analysis)

## Test Coverage
Target: ~60–70% (core logic)  
- Coverage tooling is set up in CI  
- Actual coverage still increasing as tests are added  

## Automated Tools
- CTest (CMake) → test execution  
- Catch2 → unit / constexpr tests  
- gcov / llvm-cov / OpenCppCoverage → coverage  
- clang-tidy / cppcheck / CodeQL → analysis  
- GitHub Actions → CI pipeline  

## Traceability
- Tests run in CI for every commit / PR  
- Results visible in CI logs and reports  
- Ensures early detection of regressions  
