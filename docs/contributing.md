# Contributing — Vision Studio

> Code style, naming conventions, PR process, and architectural rules.

---

## Quick Reference

| Rule | Convention |
|------|-----------|
| C++ Standard | **C++17** |
| Namespace | `namespace vision { }` |
| Header guards | `#pragma once` |
| File naming | `PascalCase.h` / `PascalCase.cpp` |
| Class naming | `PascalCase` |
| Method naming | `camelCase` |
| Member variables | `m_camelCase` |
| Smart pointers | `std::unique_ptr`, `std::shared_ptr` — no raw `new`/`delete` |

---

## Coding Standards

### Modern C++17

- Use `std::unique_ptr` / `std::shared_ptr` and RAII. **No raw `new`/`delete`**.
- Prefer `[[nodiscard]]`, `constexpr`, `std::optional`.
- **No C-style casts** — use `static_cast`, `reinterpret_cast`, etc.
- Use `auto` where the type is obvious from context.

### Cross-Platform Safety

- **No Win32-only or Mac-only APIs** without `#ifdef _WIN32` / `#ifdef __APPLE__` guards.
- Use Qt / Standard C++ wrappers instead of platform-specific calls.

### Interface Boundary

VisionApp must **never** `#include` VisionCore internals. Only:
- `VisionCore/IVisionEngine.h` (abstract interface)
- `VisionCore/Export.h` (DLL macros)

---

## Directory Structure Rules

| Zone | Path | What Goes Here |
|------|------|----------------|
| Source | `/src/` | Clean C++ source code only |
| Build | `/build/[platform]/` | CMake metadata + intermediates |
| Output | `/bin/[Config]/` | Executables and DLLs |
| Linker | `/lib/[Config]/` | Static/import libraries |
| Docs | `/docs/` | User-facing documentation |
| Plans | `/plans/` | Architecture plans, implementation details |

- **Never** place source files in the root directory.
- **Never** create files outside `/src`, `/build`, `/bin`, `/lib`, `/tests`, `/docs`, `/plans`.
- Follow `include/[ModuleName]/*.h` + `src/*.cpp` convention.

---

## Plan Document Rules

- Every file in `/plans/` must be **≤ 200 lines**.
- If a plan grows beyond 200 lines, split it into focused sub-plans.
- `MASTER_PLAN.md` is the overview index — keep it concise.

---

## Pull Request Process

1. **Branch** from `main` with a descriptive name: `feature/batch-ocr`, `fix/memory-leak`.
2. **Follow** all naming and coding conventions above.
3. **Test** — run GTest unit tests before submitting:
   ```
   ctest --test-dir build/windows-x64 --build-config Debug
   ```
4. **Describe** what changed and why in the PR description.
5. **One feature per PR** — avoid mixing unrelated changes.

---

## Commit Messages

Use imperative mood:
- ✅ `Add grayscale preprocessing to ImageProcessor`
- ✅ `Fix memory leak in VisionEngine shutdown`
- ❌ `Added some stuff`
- ❌ `WIP`

---

## Adding a New Feature

1. **Plan** — write or update a plan in `/plans/` (≤ 200 lines).
2. **Implement** — add code under `/src/[Module]/`.
3. **Test** — add GTest cases in `/tests/`.
4. **Document** — update user-facing docs in `/docs/` if relevant.
5. **PR** — follow the process above.
