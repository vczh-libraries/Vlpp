# Usage

- Copilot would only work after executing the following script in the `UnitTest.sln` folder:
  - `Tools\Copilot\copilotInit.ps1`
  - `Tools` is the `Tools` repo under the same github organization.
- To initialize a code change, use `#prompt:1-design.prompt.md` and type your problem in chat, with `# Problem`.
- Run all steps until your idea is well implemented.
- You must review created document after each step. If you are unsatisfied, use the same prompt file again with `# Update Document` in chat and explain what you want to do.
- The `prompts` folder copies all task in Visual Studio github copilot recognized format.
- The `sampleLogFiles` folder copies generated document files for a simple task as a demo:
  - `1-design.prompt.md` creates `Copilot_Task.md`.
  - `2-planning.prompt.md` creates `Copilot_Planning.md`.
  - `3-execution.prompt.md` and `4-verifying.prompt.md` create `Copilot_Execution.md`.
