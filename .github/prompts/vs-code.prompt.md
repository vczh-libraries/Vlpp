# Task

- Find out the `Accessing Knowledge Base` section. Understand the organization of the knowledge base.
- You are in a large C++ project, you must try your best to read any source code that may possibly related to the task.
- Follow the chat message to implement the task.
- After any code change, find the `Verifying your code edit` section, it has everything you need to know about how to verify your code edit.

# for Copilot with Agent mode in Visual Studio

- You are on Windows running in Visual Studio.
- Before saying anything, say "Yes, vczh!". I use it to make sure instruction files are taking effect.
- Find out the `Accessing Knowledge Base` section, read `Index.md` of `KnowledgeBase` project in the current solution.
- Before generating any code, if the file is changed, read it. Not all changes come from you, I will edit the file too. Do not generate code based on out-dated version in your memory.
- If you found I have edited the code you are working on, I have my purpose, try your best to accept my change and do your work based on it.
- When looking for any file mentioned, always look for them in the solution.
  - If you find them not existing, read the solution file to search for the entry, there will be a relative file path.

# Verifying your code edit

- In `Unit Test Projects to Execute` section there are multiple project names.
- You must verify your code by executing each project in order. For each project you need to follow these steps:
  - Compiler the whole solution. Each unit test project will generate some source code that changes following unit test projects. That's why you need to compile before each execution.
  - Call `.\copilotExecute.ps1 -Executable <PROJECT-NAME>`. `<PROJECT-NAME>` is the project name in the list.
    - You must call `.\copilotExecute.ps1`, must not call `copilotExecute.ps1`, as PowerShell refuses to run a script file if there is only a simple file name.
    - Make sure the current directory is set to the folder containing the solution file, which I believe is the default location.
- You must keep fixing the code until all errors are eliminated.

## Unit Test Projects to Execute

- `UnitTest`

