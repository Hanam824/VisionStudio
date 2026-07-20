---
name: commit
description: "Use whenever creating a git commit in this repo (user says \"commit\", \"commit these changes\", \"create a commit\", or /commit is typed). Enforces Conventional Commits (https://www.conventionalcommits.org/en/v1.0.0/) for the commit message. Not needed for PR titles/descriptions unless asked."
---

# Commit — Conventional Commits

Every commit message in this repo MUST follow [Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/):

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

## Types

| Type | When to use |
|------|-------------|
| `feat` | A new feature (user- or API-visible) |
| `fix` | A bug fix |
| `docs` | Documentation only (`/docs`, `/plans`, `README`, `CLAUDE.md`, comments-as-docs) |
| `style` | Formatting/whitespace only, no logic change |
| `refactor` | Code change that neither fixes a bug nor adds a feature |
| `perf` | Performance improvement |
| `test` | Adding or correcting tests |
| `build` | Build system, CMake, vcpkg manifest, presets |
| `ci` | CI pipeline (`.github/workflows/**`) |
| `chore` | Everything else (tooling, `.gitignore`, misc maintenance) |
| `revert` | Reverts a previous commit |

## Rules

- `<description>` is imperative, lower-case, no trailing period: `fix: correct grayscale threshold default`.
- `[optional scope]` is a parenthesized noun for the affected area when it clarifies things, e.g. `feat(VisionCore): ...`, `fix(VisionApp): ...`, `docs(plans): ...`. Omit if the type alone is clear (most `docs`/`chore` commits don't need one).
- Breaking changes: append `!` after the type/scope (`feat(VisionCore)!: change IVisionEngine ABI`) AND/OR add a `BREAKING CHANGE: <description>` footer. Rare in this codebase — flag to the user before using it.
- Body (optional) explains *why*, wrapped in a HEREDOC so formatting survives, same as the base commit workflow.
- End the body with the standard footer:
  ```
  Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
  ```

## Workflow

1. `git status`, `git diff` (staged + unstaged), and `git log --oneline -5` in parallel to see what changed and confirm the repo already uses this convention (recent history should already be `type: description`).
2. Pick the single most accurate `type` for the whole commit. If the staged changes span multiple unrelated types (e.g. a `feat` plus a `chore`), say so and suggest splitting rather than picking one arbitrarily.
3. Stage only the relevant files by name (never `-A`/`.`).
4. Commit via heredoc:
   ```bash
   git commit -m "$(cat <<'EOF'
   <type>[(scope)]: <description>

   [optional body]

   Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
   EOF
   )"
   ```
5. `git status` to confirm success.

## Examples from this repo's history

- `docs: update Qt5 references to Qt6 and add CLAUDE.md`
- `fix: reorder menu bar to File, Process, View, Help`
- `feat: add cross-platform VS Code debugging for Windows, Linux, and macOS`

Only create commits when the user asks; only amend an existing commit if explicitly requested (see main git safety rules).
