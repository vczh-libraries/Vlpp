# Cross-Platform Instruction Research Report (Linux-Focused)

## Scope and Method

This report scans instruction-oriented files and runnable helper scripts under the repository, with emphasis on content reachable from `AGENTS.md` and `.github/copilot-instructions.md`, recursively through referenced guideline/prompt/script files.

Per request, the Knowledge Base documents are excluded from analysis.

## Files Reviewed

- Root and routing instructions:
  - `AGENTS.md`
  - `.github/copilot-instructions.md`
  - `Project.md`
  - `.github/prompts/code.prompt.md`
- Build / run / debug guideline docs:
  - `.github/Guidelines/Building.md`
  - `.github/Guidelines/Running-UnitTest.md`
  - `.github/Guidelines/Running-CLI.md`
  - `.github/Guidelines/Debugging.md`
  - `Test/Linux/README.md`
- Script implementations:
  - `.github/Scripts/copilotBuild.ps1`
  - `.github/Scripts/copilotExecute.ps1`
  - `.github/Scripts/copilotDebug_Start.ps1`
  - `.github/Scripts/copilotDebug_RunCommand.ps1`
  - `.github/Scripts/copilotDebug_Stop.ps1`
  - `.github/Ubuntu/build.sh`

---


## Prompt Files Scan (`.github/prompts`)

All top-level prompt files in `.github/prompts` were scanned:

- `0-scrum.prompt.md`
- `1-design.prompt.md`
- `2-planning.prompt.md`
- `3-summarizing.prompt.md`
- `4-execution.prompt.md`
- `5-verifying.prompt.md`
- `ask.prompt.md`
- `code.prompt.md`
- `investigate.prompt.md`
- `kb.prompt.md`
- `kb-sync.prompt.md`
- `refine.prompt.md`
- `review.prompt.md`

### Prompt-specific Cross-Platform Findings

Common pattern across prompts:
- Most prompts rely on `copilot-instructions.md` for build/run/debug behavior, so Windows-vs-Linux behavior is inherited indirectly.
- Prompt docs themselves are mostly workflow/control documents, but many explicitly require running `.ps1` scripts for preparation/review/backup flows.

Windows-only or Windows-leaning references inside prompts:
- `code.prompt.md`, `4-execution.prompt.md`, `5-verifying.prompt.md` require build/test execution following existing script-based flow, which is currently Windows-centric in linked guideline docs.
- `0-scrum.prompt.md`, `1-design.prompt.md`, `review.prompt.md`, `refine.prompt.md` explicitly call PowerShell scripts such as `copilotPrepare.ps1`, `copilotPrepareReview.ps1`, and backup flows through `copilotPrepare.ps1 -Backup/-Earliest`.
- Multiple prompts assume `.ps1` orchestration availability without an explicit Linux-native equivalent in the prompt itself.

Linux-relevant nuance for prompt rewrite:
- Keep task-log maintenance scripts (`copilotPrepare*.ps1`) as Windows-only unless Linux equivalents are introduced.
- For execution-oriented prompts (`code` / `execute` / `verify`), add explicit Linux branches that route to `.github/Ubuntu/build.sh`, `vmake`/`vbuild`, direct binary execution, and LLDB guidance.
- Do not imply PowerShell script behavior parity on Linux unless corresponding Linux scripts are actually provided.

---

## What Is Windows-Specific and Should Stay Windows-Only

### 1) Build flow based on Visual Studio Developer Command Prompt + MSBuild wrappers

**Windows-only signals:**
- Hard requirement to call `copilotBuild.ps1`.
- `copilotBuild.ps1` requires `VLPP_VSDEVCMD_PATH` (path to `VsDevCmd.bat`).
- Uses `cmd.exe`/`ComSpec`, `.sln`, and MSBuild command-line composition.
- Uses `-Platform Win32|x64` and `-Configuration Debug|Release` with Visual Studio conventions.

**Linux relevance:**
- This path should not be treated as Linux instructions.
- Linux already has `vmake`/`vbuild` flow via `.github/Ubuntu/build.sh`.

**Nuance to preserve in docs:**
- Keep Windows logging behavior details (`Build.log`, `Build.log.unfinished`) documented in Windows section only.
- Do not imply Linux build writes equivalent logs unless Linux scripts are enhanced to do so.

### 2) Test execution via `copilotExecute.ps1`

**Windows-only signals:**
- Wrapper resolves `.exe` artifacts.
- Uses `cmd.exe` and `.vcxproj.user` debugger args conventions.
- Unit test mode emits `Execute.log` and `Execute.log.unfinished`.

**Linux relevance:**
- Linux execution is direct binary invocation (`./Bin/UnitTest /C`), currently no built-in log file lifecycle equivalent.

**Nuance to preserve in docs:**
- Explicitly state: `copilotExecute.ps1` log behavior is a Windows-only convenience.
- If parity is desired, Linux would need explicit tee/logging wrapper (not currently in `.github/Ubuntu/build.sh` or Linux README).

### 3) Debugging stack (`copilotDebug_*.ps1`) via CDB/WinDBG commands

**Windows-only signals:**
- Uses CDB through Windows Debugger tooling.
- Named pipe remote command model (`-server npipe`, `-remote npipe`).
- `cmd.exe` execution and WinDBG command vocabulary (`g`, `kn`, `bp`, `.frame`, etc.).

**Linux relevance:**
- Non-portable to Linux.
- Linux guidance already exists in `Test/Linux/README.md` using LLDB, PTY session, Ctrl-C/process-kill/quit workflow.

**Nuance to preserve in docs:**
- Keep CDB/WinDBG and PS debug scripts strictly in Windows-only section.
- For Linux, point to LLDB command model and interaction constraints.

### 4) Path syntax and shell semantics

**Windows-only signals:**
- Backslash paths (`REPO-ROOT\.github\Scripts\...`).
- PowerShell call syntax with `&` and `.ps1` entrypoints.
- `start powershell { ... }` patterns.

**Linux relevance:**
- Linux docs/scripts should use POSIX paths, bash command forms, and avoid suggesting `.ps1` invocations as primary path.

---

## What Is Linux-Specific and Should Be Preferred in Linux Instructions

### 1) Build toolchain: `vmake` + `vbuild`

- `.github/Ubuntu/build.sh` sets `VCPROOT`, extends `PATH` with `vl/cmd`, then runs:
  - `vmake --make`
  - `vbuild -b` (incremental) or `vbuild -f` (full)
- `Test/Linux/README.md` already wraps this as:
  - `../../.github/Ubuntu/build.sh`
  - `../../.github/Ubuntu/build.sh -f`

### 2) Linux execution

- Test command currently documented as direct process run:
  - `./Bin/UnitTest /C`

### 3) Linux debugging with LLDB

- `Test/Linux/README.md` provides LLDB lifecycle:
  - launch `lldb -- ./Bin/UnitTest /C`
  - use PTY-backed interaction
  - terminate safely (`quit`, or `Ctrl-C` + `process kill` + `quit`)
  - one-shot recommendation via `timeout` to avoid hangs

---

## Mixed / Potentially Confusing Areas

### A) `.github/copilot-instructions.md` contains both OS modes in one document

This is useful but easy to misapply. It currently states Linux replacements, while many linked guideline files remain Windows-script centric.

**Recommendation:**
- Add explicit per-OS headers in each linked guideline (`Building`, `Running-UnitTest`, `Running-CLI`, `Debugging`):
  - `## Windows` section (existing content)
  - `## Linux` section (concise Linux equivalents)
- In each section, include a one-line “Do not use the other OS flow”.

### B) `Running-CLI.md` appears to be copy-pasted from UnitTest mode

It says “run a unit test project” and example uses `-Mode UnitTest`, which is likely incorrect for CLI usage.

**Recommendation:**
- Split examples:
  - Windows CLI should use `copilotExecute.ps1 -Mode CLI -Executable PROJECT-NAME`.
  - Linux CLI should provide direct executable command pattern.

### C) Logging parity expectations

Windows helper scripts produce durable log artifacts (`Build.log`, `Execute.log`) with unfinished sentinel files.
Linux flow currently does not specify or generate equivalent logs in provided scripts/docs.

**Recommendation:**
- Document this difference clearly as intentional.
- Optional future Linux helper scripts can add tee-based logs, but should not be implied today.

---

## Proposed Cross-Platform Instruction Shape

Use a clear matrix in core guidance docs:

- **Build**
  - Windows: `copilotBuild.ps1` (+ log files)
  - Linux: `.github/Ubuntu/build.sh` (no default logs)
- **Run UnitTest**
  - Windows: `copilotExecute.ps1 -Mode UnitTest` (+ `Execute.log`)
  - Linux: `./Bin/UnitTest /C`
- **Run CLI**
  - Windows: `copilotExecute.ps1 -Mode CLI`
  - Linux: direct CLI binary invocation
- **Debug**
  - Windows: CDB/WinDBG + `copilotDebug_*.ps1`
  - Linux: LLDB workflow from `Test/Linux/README.md`

Include explicit “tool substitution” notes:
- PowerShell wrappers are **Windows-only orchestration**.
- Linux uses bash + native toolchain.
- Do not claim behavior equivalence where none exists (especially log generation and debugger protocol).

---

## Practical Update Checklist for Instruction Rewrite

1. Keep all `copilot*.ps1` references, but label them `Windows only` at first mention.
2. Add Linux counterparts adjacent to each Windows flow (build/run/debug).
3. Correct `Running-CLI.md` command examples and wording.
4. Avoid mixed path separators in examples; keep OS-native syntax.
5. Preserve existing Windows conventions (`VsDevCmd`, `Win32/x64`, CDB commands) without attempting pseudo-porting.
6. Promote LLDB guidance as the Linux default debugger flow.
7. Clearly document that Linux build script currently does not emit `Build.log`/`Execute.log` style artifacts.

---

## Bottom Line

Your current repository already has both ecosystems:
- A mature **Windows PowerShell orchestration** path with logs and CDB integration.
- A functional **Linux bash + vmake/vbuild + LLDB** path.

The key cross-platform improvement is not replacing Windows scripts, but **making OS boundaries explicit in every instruction entry point** and fixing places where Windows UnitTest text leaked into generic/CLI guidance.


## Summary of Affected Files

These files are directly affected by the cross-platform instruction rewrite scope identified in this report:

### Highest Priority (execution behavior / toolchain)
- `.github/copilot-instructions.md`
- `.github/Guidelines/Building.md`
- `.github/Guidelines/Running-UnitTest.md`
- `.github/Guidelines/Running-CLI.md`
- `.github/Guidelines/Debugging.md`
- `Test/Linux/README.md`

### Script Surface (keep Windows-only behavior explicit)
- `.github/Scripts/copilotBuild.ps1`
- `.github/Scripts/copilotExecute.ps1`
- `.github/Scripts/copilotDebug_Start.ps1`
- `.github/Scripts/copilotDebug_RunCommand.ps1`
- `.github/Scripts/copilotDebug_Stop.ps1`
- `.github/Scripts/copilotPrepare.ps1`
- `.github/Scripts/copilotPrepareReview.ps1`

### Linux Build Entry
- `.github/Ubuntu/build.sh`

### Prompt Files to Align with OS-specific execution branches
- `.github/prompts/0-scrum.prompt.md`
- `.github/prompts/1-design.prompt.md`
- `.github/prompts/2-planning.prompt.md`
- `.github/prompts/3-summarizing.prompt.md`
- `.github/prompts/4-execution.prompt.md`
- `.github/prompts/5-verifying.prompt.md`
- `.github/prompts/ask.prompt.md`
- `.github/prompts/code.prompt.md`
- `.github/prompts/investigate.prompt.md`
- `.github/prompts/kb.prompt.md`
- `.github/prompts/kb-sync.prompt.md`
- `.github/prompts/refine.prompt.md`
- `.github/prompts/review.prompt.md`
