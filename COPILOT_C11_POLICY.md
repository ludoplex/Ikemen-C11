# GitHub Copilot C11 Development Policy

## Purpose

This document provides explicit guidance for GitHub Copilot, AI coding assistants, and automated development tools working on the Ikemen C11 refactor. Following these rules ensures AI-generated code aligns with project architecture and does not disrupt the existing Go implementation.

## Primary Rules

### 1. C11 Code Location
**All C11 implementation code MUST go under the `c11/` directory.**

When generating C11 code:
- Place C source files in `c11/src/`
- Place public headers in `c11/include/`
- Place test files in `c11/tests/`
- Place documentation in `c11/docs/`

**Never create C source files outside `c11/` for the refactor work.**

### 2. Do Not Modify Go Sources
**The Go implementation in `src/*.go` is the canonical reference and must NOT be modified for C11 work.**

- Do not alter, remove, or reorganize Go source files
- Do not change Go build scripts or Makefiles for C11 features
- Do not modify `go.mod` or `go.sum` for C11 dependencies
- Exception: Additive CI changes that run C11 builds alongside Go builds are acceptable

### 3. Do Not Modify Shared Assets
**Assets in `data/`, `external/`, and `font/` are shared between engines.**

- Do not change asset files to accommodate C11
- Do not reorganize asset directories
- Do not add C11-specific assets to shared locations
- C11 tests may create temporary assets in `/tmp` or `c11/build/`

### 4. Generate Real, Working Code
**Never generate placeholder or example-only code.**

When implementing C11 features:
- Write complete, compiling implementations
- Include proper error handling
- Add comprehensive tests
- Ensure code passes strict warnings (`-Wall -Wextra -Wpedantic -Werror`)
- Use only C11 standard library and POSIX APIs

### 5. Maintain Asset Compatibility
**C11 code must work with existing MUGEN resources used by the Go engine.**

When parsing or processing assets:
- Follow the same search paths as Go engine
- Parse file formats identically to Go implementation
- Handle edge cases the way Go engine does
- Test with actual MUGEN assets from the repository

## Code Generation Guidelines

### Implementing New C11 Features

When asked to implement a C11 feature:

1. **Study the Go reference first**
   - Look at corresponding Go code in `src/`
   - Understand the behavior and edge cases
   - Note file path handling and search order

2. **Create modular C11 implementation**
   - Write header file in `c11/include/` with full documentation
   - Implement in `c11/src/` with comments referencing Go code
   - Use C11 standard features only
   - Handle errors explicitly

3. **Write comprehensive tests**
   - Create test file in `c11/tests/`
   - Cover normal cases, edge cases, and error conditions
   - Compare behavior with Go engine when possible
   - Integrate with CTest in `c11/CMakeLists.txt`

4. **Update documentation**
   - Add comments explaining design decisions
   - Reference MUGEN behavior being replicated
   - Update `c11/docs/ROADMAP.md` progress tracking

### Example: Adding a New Module

If asked to "implement SFF sprite file parsing in C11":

**DO:**
```
c11/include/sff.h          - Public API for SFF parsing
c11/src/sff.c              - Implementation
c11/tests/test_sff.c       - Comprehensive tests
Update c11/CMakeLists.txt  - Add new targets
Update c11/docs/ROADMAP.md - Mark progress
```

**DON'T:**
```
src/sff_c11.go             - Never modify Go code
data/test.sff              - Don't add test assets to shared dirs
c11/src/example_sff.c      - No example-only code
```

### CMake Build System

When modifying `c11/CMakeLists.txt`:

- Enforce C11 standard: `set(CMAKE_C_STANDARD 11)`
- Disable extensions: `set(CMAKE_C_EXTENSIONS OFF)`
- Enable strict warnings for GCC/Clang and MSVC
- Add new source files to appropriate targets
- Create test executables and add to CTest
- Maintain out-of-tree build compatibility

### Writing Tests

All C11 functionality must have tests:

**Unit Tests:**
- Test individual functions in isolation
- Use simple assertions
- Exit with 0 on success, non-zero on failure
- Print clear pass/fail messages

**Integration Tests:**
- Test component interactions
- May create temporary test directories in `/tmp`
- Clean up test artifacts
- Validate against known-good results

**Compatibility Tests:**
- Process the same assets as Go engine
- Compare outputs when possible
- Report discrepancies as failures

**CTest Integration:**
```cmake
add_executable(test_myfeature tests/test_myfeature.c)
target_link_libraries(test_myfeature PRIVATE ikemen_engine)
add_test(NAME MyFeature COMMAND test_myfeature)
```

## Asset Path Handling

When implementing file I/O or asset loading:

### Search Order (Match Go Engine)
1. Explicit absolute or relative path
2. Relative to definition file's directory
3. Motif (screenpack) directory
4. `data/` directory
5. `external/` directory (for scripts, shaders, etc.)
6. `font/` directory
7. `video/` directory

### Implementation Pattern
```c
/* Check multiple locations following Go engine's search order */
const char* search_paths[] = {
    def_file_dir,    /* Relative to definition file */
    motif_dir,       /* Screenpack directory */
    "data/",         /* Common data */
    NULL
};

for (int i = 0; search_paths[i] != NULL; i++) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", 
             search_paths[i], filename);
    if (file_exists(full_path)) {
        return load_file(full_path);
    }
}
```

## Error Handling

Always handle errors explicitly in C11 code:

**DO:**
```c
int result = ikm_parse_file(path);
if (result != IKM_SUCCESS) {
    fprintf(stderr, "Failed to parse %s: %s\n", 
            path, ikm_get_error());
    return result;
}
```

**DON'T:**
```c
ikm_parse_file(path);  /* Ignoring return value */
```

Return meaningful error codes:
- `IKM_SUCCESS` (0) for success
- Specific error codes for different failure modes
- Provide `ikm_get_last_error()` for detailed messages

## Documentation Standards

### Header File Documentation
```c
/*
 * Brief description of function.
 * 
 * Detailed explanation including:
 * - Expected inputs and constraints
 * - Return value meanings
 * - Side effects
 * - Thread safety considerations
 * - MUGEN compatibility notes
 * 
 * Parameters:
 *   param1 - Description
 *   param2 - Description
 * 
 * Returns:
 *   Success/error codes
 */
int ikm_function(param1, param2);
```

### Source File Comments
```c
/* 
 * Implementation notes:
 * - This follows Go engine's behavior in src/sprite.go:123
 * - MUGEN spec requires X behavior in Y situation
 * - Performance consideration: cache frequently accessed data
 */
```

## CI/CD Integration

When modifying `.github/workflows/c11-ci.yml`:

**Ensure workflow:**
- Triggers on C11 file changes and PRs
- Builds on Ubuntu (minimum), ideally Linux/Windows/macOS
- Runs `cmake` to configure
- Runs `cmake --build .` to compile
- Runs `ctest` to execute tests
- Fails on build errors or test failures

**Do not:**
- Modify existing Go CI workflows unless adding non-breaking C11 checks
- Break existing Go engine builds
- Remove or weaken existing CI checks

## Compatibility Testing

When implementing features, consider:

### Behavioral Equivalence
Does the C11 implementation behave identically to Go for:
- Parsing the same asset files
- Handling malformed input
- Processing edge cases
- Following the same search paths

### Asset Compatibility
Can C11 use:
- Existing character definitions
- Existing stage definitions
- Existing screenpacks
- Existing MUGEN resources without modification

### Regression Prevention
- Test against actual assets from the repository
- Compare outputs with Go engine when possible
- Document known behavioral differences as bugs

## Common Patterns

### Version Query Functions
```c
/* Simple accessors returning constants */
int ikm_engine_major_version(void) {
    return IKM_ENGINE_MAJOR;
}
```

### Resource Loading
```c
/* Search multiple paths, return first success */
Resource* ikm_load_resource(const char* name) {
    for each search path {
        if (try_load(search_path + name)) {
            return resource;
        }
    }
    return NULL;  /* Not found */
}
```

### Error Reporting
```c
/* Thread-local error message buffer */
static char error_buffer[512];

void ikm_set_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(error_buffer, sizeof(error_buffer), fmt, args);
    va_end(args);
}

const char* ikm_get_last_error(void) {
    return error_buffer;
}
```

## Anti-Patterns to Avoid

### DON'T: Modify Go Code for C11
```
❌ src/sprite.go: Add C11 compatibility shim
❌ src/main.go: Add flag to use C11 backend
```

### DON'T: Create Placeholders
```
❌ c11/src/todo.c: /* TODO: Implement this */
❌ c11/include/api.h: /* Placeholder for future API */
```

### DON'T: Assume Availability
```
❌ Using C++ features (this is C11)
❌ Using GNU extensions (use standard C11)
❌ Platform-specific APIs without fallbacks
```

### DON'T: Skip Testing
```
❌ Implementing features without tests
❌ Not integrating tests into CTest
❌ Skipping asset compatibility validation
```

## Questions and Clarifications

When uncertain about how to implement something:

1. **Check the Go reference implementation** in `src/`
2. **Consult MUGEN documentation** for expected behavior
3. **Review existing C11 code** for patterns
4. **Read `c11/docs/ROADMAP.md`** for planned approach
5. **When still unclear**, ask the human developer rather than guessing

## Summary Checklist

Before generating C11 code, confirm:

- [ ] Code goes under `c11/` directory
- [ ] No modifications to Go sources or shared assets
- [ ] Real, compiling implementation (not placeholder)
- [ ] Follows Go engine's behavior for compatibility
- [ ] Includes comprehensive tests integrated with CTest
- [ ] Uses only C11 standard and POSIX APIs
- [ ] Handles errors explicitly with clear messages
- [ ] Documented with comments and updated roadmap
- [ ] Enforces strict warnings in CMake
- [ ] Validated against actual MUGEN assets

## Conclusion

AI coding assistants must respect the C11 refactor architecture: isolated under `c11/`, compatible with Go engine and MUGEN assets, always providing real working code with tests. The Go implementation is the canonical reference that must not be modified. Asset compatibility is paramount—both engines must work with identical resources.

By following these rules, AI-generated contributions will integrate smoothly into the C11 refactor without disrupting the existing, working Go implementation.

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-17  
**For**: GitHub Copilot and AI coding assistants
