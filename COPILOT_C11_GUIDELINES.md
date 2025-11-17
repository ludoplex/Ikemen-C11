# AI Assistant Guidelines for Ikemen C11 Refactor

## ⚠️ CRITICAL RULES FOR AI ASSISTANTS ⚠️

This document provides guidelines for GitHub Copilot and other AI coding assistants working on the Ikemen C11 refactor project.

### Rule #1: DO NOT MODIFY EXISTING GO CODE

**NEVER edit, delete, or modify ANY files in the `src/` directory or any other Go source files.**

The `src/` directory contains the original Ikemen GO engine. It must remain untouched during the C11 refactor.

### Rule #2: ALL C11 CODE GOES IN THE `c11/` DIRECTORY

**ALL new C11 code MUST be placed under the `c11/` top-level directory.**

```
✓ CORRECT:
  c11/src/engine.c
  c11/include/engine.h
  c11/tests/test_engine.c

✗ WRONG:
  src/engine.c
  include/engine.h
  engine.c
```

### Rule #3: FOLLOW THE ARCHITECTURAL GUIDELINES

Read and follow the guidelines in `C11-ROADMAP.md` for:
- Directory structure
- Coding standards
- Development phases
- Build system conventions
- Testing requirements

## Directory Structure Reference

```
Ikemen-C11/
├── src/                    # ⛔ DO NOT TOUCH - Original Go engine
├── c11/                    # ✅ ALL C11 WORK GOES HERE
│   ├── src/                # C11 implementation files (.c)
│   ├── include/            # Public header files (.h)
│   ├── tests/              # Test files
│   ├── cmake/              # CMake modules
│   ├── docs/               # C11-specific documentation
│   ├── CMakeLists.txt      # Root CMake configuration
│   └── build/              # Build artifacts (gitignored)
├── C11-ROADMAP.md          # Development roadmap and guidelines
├── COPILOT_C11_GUIDELINES.md  # This file
└── README.md               # Project overview
```

## When Adding New Features

### Before Writing Code:

1. **Check the phase**: Refer to `C11-ROADMAP.md` to understand the current development phase
2. **Choose the right directory**: Determine the correct subdirectory in `c11/`
3. **Check dependencies**: Ensure prerequisite systems are implemented
4. **Read existing code**: Look for similar implementations in `c11/src/`

### While Writing Code:

1. **Use C11 standard**: Strictly follow ISO C11
2. **Follow naming conventions**: 
   - Types: `PascalCase`
   - Functions: `snake_case`
   - Macros: `UPPER_SNAKE_CASE`
3. **Add documentation**: Use Doxygen-style comments
4. **Handle errors properly**: Return error codes, don't use assertions for runtime errors
5. **Write tests**: Add corresponding tests in `c11/tests/`

### Code Style Checklist:

- [ ] Indentation: 4 spaces (no tabs)
- [ ] Line length: ≤ 100 characters
- [ ] All warnings enabled and passing
- [ ] No memory leaks (test with sanitizers)
- [ ] Includes proper error handling
- [ ] Has unit tests
- [ ] Has Doxygen documentation

## Common Scenarios

### Scenario 1: User asks to "fix a bug in the engine"

**✓ CORRECT Response:**
```
I'll help! First, I need to clarify - are you referring to:
1. The existing Go engine in src/? (I can diagnose but should not modify)
2. The new C11 engine in c11/? (I can fix)
```

**✗ WRONG Response:**
Immediately editing files in `src/` without clarification.

### Scenario 2: User asks to "add a new rendering feature"

**✓ CORRECT Response:**
```
I'll implement this in the C11 engine under c11/src/render.c 
and c11/include/render.h, following the architecture in C11-ROADMAP.md.
```

**✗ WRONG Response:**
Adding the feature to `src/render.go` or creating files outside `c11/`.

### Scenario 3: User asks to "improve performance"

**✓ CORRECT Response:**
```
For the C11 engine, I can optimize code in c11/. For the Go engine,
I can suggest changes but won't modify src/ directly.
```

**✗ WRONG Response:**
Modifying Go files in `src/` without permission.

### Scenario 4: User asks to "port feature X from Go to C11"

**✓ CORRECT Response:**
```
I'll analyze src/X.go to understand the feature, then implement
an equivalent in c11/src/X.c without modifying the original Go code.
```

**✗ WRONG Response:**
Copying Go code verbatim or mixing Go and C files.

## Build System Guidelines

### CMake Files

- Main configuration: `c11/CMakeLists.txt`
- Subdirectory configs: `c11/src/CMakeLists.txt`, `c11/tests/CMakeLists.txt`
- Custom modules: `c11/cmake/*.cmake`

### Building the C11 Engine

```bash
cd c11
mkdir -p build
cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd c11/build
ctest --output-on-failure
```

## Testing Guidelines

### Test Structure

```c
// c11/tests/test_example.c
#include <assert.h>
#include <stdio.h>
#include "example.h"

int test_example_function(void) {
    int result = example_function(42);
    assert(result == 84);
    return 0;
}

int main(void) {
    printf("Running test_example_function...\n");
    if (test_example_function() != 0) {
        return 1;
    }
    printf("All tests passed!\n");
    return 0;
}
```

### Test Requirements

- Every public function should have tests
- Tests must compile without warnings
- Tests must pass before merging
- Use assertions for test validation
- Return 0 on success, non-zero on failure

## CI/CD Integration

The C11 build is automatically tested via GitHub Actions when:
- Files in `c11/` are modified
- `.github/workflows/c11-ci.yml` is modified
- CMake files are changed

## Getting Help

### Documentation Resources

1. `C11-ROADMAP.md` - Overall plan and coding guidelines
2. `c11/docs/` - Subsystem-specific documentation
3. `BUILDING.md` - Original build instructions (for reference)
4. `CONTRIBUTING.md` - General contribution guidelines

### Questions to Ask Users

When unsure, ask:
- "Should this be in the C11 engine or the Go engine?"
- "Which subsystem does this belong to?"
- "Is this following the current development phase?"
- "Does this require modifying existing Go code?" (If yes, get explicit permission)

## Anti-Patterns to Avoid

### ❌ DON'T DO THIS:

```c
// Creating C files in the wrong place
// ❌ BAD: src/new_feature.c
// ❌ BAD: new_feature.c
// ✅ GOOD: c11/src/new_feature.c
```

```c
// Mixing Go and C
// ❌ BAD: Modifying src/*.go to call C code
// ✅ GOOD: Implementing C version separately in c11/
```

```c
// Deleting Go files
// ❌ BAD: rm src/old_system.go
// ✅ GOOD: Leave Go files alone, implement in c11/src/new_system.c
```

```c
// Ignoring build system
// ❌ BAD: Adding source files without updating CMakeLists.txt
// ✅ GOOD: Update c11/src/CMakeLists.txt when adding sources
```

## Summary Checklist

Before suggesting or making any changes:

- [ ] I am NOT modifying files in `src/` or any Go source files
- [ ] I am placing all C11 code under the `c11/` directory
- [ ] I have read the relevant sections of `C11-ROADMAP.md`
- [ ] I am following C11 coding standards
- [ ] I am using the correct naming conventions
- [ ] I am adding tests for new functionality
- [ ] I am updating CMakeLists.txt if adding new source files
- [ ] I am adding documentation for new APIs

## In Case of Doubt

**When in doubt, ASK THE USER before:**
- Modifying anything outside `c11/`
- Deleting any files
- Changing build configurations
- Making architectural decisions

**It's better to ask than to violate the non-destructive development principle!**

---

Remember: The C11 refactor is a parallel development effort. The Go engine continues to work independently while we build the C11 version from scratch in the `c11/` directory.

Last Updated: 2025-11-17
