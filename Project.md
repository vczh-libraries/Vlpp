# General Instruction

## Solution to Work On

You are working on the solution `REPO-ROOT/Test/UnitTest/UnitTest.sln`,
therefore `SOLUTION-ROOT` is `REPO-ROOT/Test/UnitTest`.

## Projects for Verification

The `REPO-ROOT/Test/UnitTest/UnitTest/UnitTest.vcxproj` is the unit test project.
When any *.h or *.cpp file is changed, unit test is required to run.

When any test case fails, you must fix the issue immediately, even those errors are unrelated to the issue you are working on.

## Linux/macOS Specific

`REPO-ROOT/Test/Linux` stores linux configurations for `UnitTest.vcxproj`.

You need to build, test and debug in that specific folder, otherwise the unit test will not function properly.
For native Linux builds, only configuration "debug x64" is available, no need to build or run other native configurations.

## WebAssembly

The following unit test projects could be built to web assembly and run with a browser:
- `REPO-ROOT/Test/Linux`
