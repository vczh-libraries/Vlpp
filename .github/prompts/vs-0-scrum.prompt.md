# Scrum

## Goal and Constraints

- Your goal is to finish a design document in `Copilot_Scrum.md` to address a problem.
- You are only allowed to update `Copilot_Scrum.md`.
- You are not allowed to modify any other files.
- Make sure only code references are in code blocks. Do not emit code blocks for markdown.

## Step 1. Identify the Problem

- The problem I would like to solve is in the chat messages sending with this request.
- Find `# Problem` or `# Update` in the LATEST chat message. Ignore any `# Problem` or `# Update` in the chat history.
- If there is a `# Problem` section: it means I am starting a fresh new request.
  - You should override `Copilot_Scrum.md` with only one title `# !!!SCRUM!!!`.
  - After overriding, copy precisely my problem description in `# Problem` from the LATEST chat message under a `# DESIGN REQUEST`.
- If there is an `# Update` section: it means I am going to propose some change to `Copilot_Scrum.md`.
  - You should continue to work out more details.
  - Copy precisely my problem description in `# Update` from the LATEST chat message to the `# DESIGN REQUEST` section, with a new sub-section `## UPDATE`.
  - Follow my update to change the design document.
- If there is nothing: it means you are accidentally stopped. Please continue your work.

## Step 2. Understand the Goal and Quality Requirement

- Your goal is to help me break down the problem into small tasks, write it down to `Copilot_Scrum.md`.
- Each task should be small enough to only represent a single idea or feature upgrade.
- Each task should be big enough to deliver a complete piece of functionality, do not make a task that only add code that will not be executed.
- The order of tasks is important. At the end of any task, the project should be able to compile and test.
- The reason I would like you to solve the problem in this way instead of making an immediate plan of code improvement is that, it is complicated and should work across the whole project. As an experienced C++ developer for large scale systems, you need to:
  - Take in consideration of the knowledge base, finding anything that would be helpful for the current problem.
  - Read through the code base carefully. The project is complicated, one small decision may have widely impact to the other part of the project.
  - Think thoroughly.

## Step 3. Finish the Document

- Break the problem in to phrases and tasks.
  - A phrase should begin with a title `## Phrase a: A short name`. The number `a` is for the phrase.
    - Under a phrase there is a comprehensive description about the goal of this phrase. A phrase contains multiple tasks. 
  - A task should begin with a title `### Task a-b: A short name`. The first number `a` is for the phrase, the second number `b` is for the task.
    - Under a task there is a comprehensive description about the goal of this task. 
  - A task must be at least updating some code, it must not be like learning or analysing or whatever just reading. Reading, thinking and planning is your immediate work to do.
- When making each task, after a comprehensive description, there should also be:
  - `what to be done`: A clear definition of what needs to be changed or implemented.
    - Keep it high-level, you can mention what should be done to update a certain group of classes, but do not include any actual code change.
  - `how to test it`: A clear definition of what needs to be tested in unit test.
    - The test plan is about writing unit test cases. Do not include end-to-end tests or manual tests.
    - Sometimes it is difficult to make relevant unit test, for example a change to the UI, you can skip the test plan but you need to provide a rationale for it.
    - You do not need to design test cases or think about code coverage at this moment. Instead consider about how to ensure testability, how many existing components are affected so that they need to be tested, and figure out if existing test cases covered affected components.
  - `rationale`:
    - Reasons about why you think it is necessary to have this task, why you think it is the best for the task to be in this order.
    - Any support evidences from source code or knowledge base. If you can't find anything from the knowledge base, what do you think need to be added.
- Make sure only code references are in code blocks. Do not emit code blocks for markdown.

## Step 4. Identify Changes to Knowledge Base

- It is important to keep the knowledge base up to date.
- You must read through relevant topics in the knowledge base, including target file of hyperlinks.
- Identify if anything in the knowledge base needs to change, as tasks could impact the knowledge base.
- Identify if anything needs to be added to the knowledge base, as tasks could introduce new concepts.
- Put them under the `## Impact to the Knowledge Base` section.
- I want the knowledge base to include only important information because:
  - Most of the information can be just easily obtained from the code.
  - Keep the knowledge base to only guidelines and design insights will have you find correct piece of code to read.

## Step 5. Mark the Completion

- Ensure there is a `# !!!FINISHED!!!` mark at the end of `Copilot_Scrum.md` to indicate the document reaches the end.

# for Copilot with Agent mode in Visual Studio

- You are on Windows running in Visual Studio.
- Before saying anything, say "Yes, vczh!". I use it to make sure instruction files are taking effect.
- Find out the `Accessing Knowledge Base` section, read `Index.md` of `KnowledgeBase` project in the current solution.
- Before generating any code, if the file is changed, read it. Not all changes come from you, I will edit the file too. Do not generate code based on out-dated version in your memory.
- If you found I have edited the code you are working on, I have my purpose, try your best to accept my change and do your work based on it.
- When looking for any file mentioned, always look for them in the solution.
  - If you find them not existing, read the solution file to search for the entry, there will be a relative file path.

# Accessing Log Files

