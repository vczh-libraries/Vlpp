# !!!SCRUM!!!

# DESIGN REQUEST

Add a `/C` mode to unit test, it prints the first error/exception/runtime error, and then exit.

The current implementation of unit test is that, `/D` won't print and `/R` won't stop.

## UPDATE

I think you break the problem into too small tasks. Please combine 1, 2 and 3 together, keep 4 alone.

I think I would like to keep `RecordFailure` just record the failure as it is named. I believe there should be a better place to exit, just like `/D` it exists because the exception is not ignored, leading to crashing of the unit test (which is actually good because I want Visual Studio debugger to catch it), could you do some research and make another proposal?

## UPDATE

What about extending `suppressFailure` to be an enum class? So in `SuppressCppFailure` and `SuppressCFailure` it can check if it is `/C` or `/R`, and just rethrow the exception under `/C`. They are not called in `/D` anyway so it is no need to handle `/D` inside them.

## UPDATE

Reconsider the test part. It is actually funny to test unit test by unit test, so I made a `FakeUnitTestProject`. Usually I will run it manually with all possible options (so now will be `/D`, `/R` and `/C`). Do you think the current `FakeUnitTestProject` is good enough to cover the `/C` scenario?

## Phrase 1: Design and Implement the /C Command Line Option with Enum-Based Failure Mode

The goal of this phrase is to add a new command line mode `/C` (Crash on failure) that will cause the unit test framework to print the first error/exception/runtime error and immediately exit. This will be implemented by converting the existing `suppressFailure` boolean to an enum class that can represent three states: `/D` (Debug), `/R` (Release), and `/C` (Crash). The exception handling functions `SuppressCppFailure` and `SuppressCFailure` will check the enum value and rethrow exceptions in `/C` mode to cause immediate program termination.

### Task 1-1: Convert suppressFailure to Enum and Implement /C Mode Logic [PROCESSED]

Replace the existing boolean `suppressFailure` with an enum class that represents the three failure handling modes, and implement the corresponding logic in command line parsing and exception handling functions.

**What to be done:**
- Create a new enum class `FailureMode` with values `Debug`, `Release`, and `Crash` in the `execution_impl` namespace
- Replace the `bool suppressFailure` global variable with `FailureMode failureMode`
- Update the command line parsing logic in `RunAndDisposeTests` to recognize the `/C` option and set `failureMode` accordingly
- Ensure `/C` is mutually exclusive with `/D` and `/R` options (similar to how `/D` and `/R` are currently mutually exclusive)
- Modify `ExecuteAndSuppressFailure` to check for `FailureMode::Debug` and call the callback directly (no change in behavior for `/D` mode)
- Update `SuppressCppFailure` and `SuppressCFailure` to check the `failureMode` and rethrow exceptions when `failureMode == FailureMode::Crash`
- In `/C` mode, exceptions should be caught, failure recorded via `RecordFailure`, and then rethrown to cause immediate program termination
- Update the status message logic to display appropriate messages for all three modes

**What to test:**
Based on the analysis of the current `FakeUnitTestProject`, it provides excellent coverage for the `/C` scenario because it includes:
- `TestFileAssertFailed.cpp`: Contains various assertion failures, TEST_ERROR, TEST_EXCEPTION scenarios that will trigger the failure recording and exception rethrowing logic
- `TestFileException.cpp`: Contains direct exception throws (Error, Exception, integer throws, division by zero, access violations) that will test both `SuppressCppFailure` and `SuppressCFailure` paths in `/C` mode
- `TestFileConfigFailed.cpp`: Contains configuration errors that will test the configuration error handling in `/C` mode
- `TestFilePassed.cpp`: Contains passing tests to ensure `/C` mode doesn't affect successful test execution

The current `FakeUnitTestProject` is sufficient for testing `/C` mode because:
1. It covers all exception types that the unit test framework handles (UnitTestAssertError, UnitTestConfigError, Error, Exception, runtime exceptions)
2. It tests both structured exception handling (Windows) and C++ exception handling paths
3. Manual testing with `/C` on this project will demonstrate immediate exit behavior after the first failure
4. The variety of failure types ensures comprehensive coverage of the rethrow logic in both `SuppressCppFailure` and `SuppressCFailure`

**Manual testing approach:**
- Run `FakeUnitTestProject` with `/C` option and verify it exits immediately after the first failure is printed
- Compare behavior with `/D` and `/R` modes to ensure the difference is clear
- Verify that the error message is printed before the program exits (via `RecordFailure`)
- Test that the exit code is non-zero when failures occur in `/C` mode

**Reasons for this task:**
Converting `suppressFailure` to an enum class is a cleaner architectural solution that scales better than adding additional boolean flags. The enum clearly represents the three distinct failure handling strategies. The rethrowing approach in `SuppressCppFailure` and `SuppressCFailure` is elegant because these functions are only called when exceptions need to be handled (not in `/D` mode), so the logic naturally fits there. Rethrowing after recording the failure ensures the error message is printed while still causing immediate program termination, which is exactly the desired behavior for `/C` mode.

**Support evidence:**
Looking at the existing code in `UnitTest.cpp`, the current `suppressFailure` boolean around line 39 controls whether `ExecuteAndSuppressFailure` calls the callback directly (line 98) or goes through the exception handling chain. The `/D` and `/R` option parsing around lines 162-170 follows a pattern that can be easily extended to include `/C`. The exception handling functions `SuppressCppFailure` (lines 55-75) and `SuppressCFailure` (lines 77-88) are the perfect places to implement the rethrow logic since they already handle all exception types and are only called when suppression is needed.

### Task 1-2: Update Documentation and Help Text

Update the user-facing documentation and help text to include information about the new `/C` option and its behavior.

**What to be done:**
- Update the `PrintUsages()` function to include the `/C` option in the usage message, changing it to `[/D | /R | /C]`
- Update any comments or documentation strings in the header file to describe the new `/C` mode
- Consider updating the class-level documentation for the `UnitTest` class to mention the new mode and its behavior
- Ensure that the help text clearly explains the difference between `/D`, `/R`, and `/C` modes

**What to test:**
The `FakeUnitTestProject` can be used to test the updated help text:
- Run `FakeUnitTestProject` with invalid combinations like `/D /C` or `/R /C` to verify the updated usage message is displayed
- Run with unrecognized options to ensure the help text shows the new `/C` option
- Manual verification that the help text accurately describes the behavior differences

**Reasons for this task:**
Good documentation is essential for usability. Users need to understand what the `/C` option does and how it differs from the existing options. This task ensures that the new functionality is properly documented and accessible to users.

**Support evidence:**
The existing `PrintUsages()` function around line 127 in `UnitTest.cpp` provides a simple usage message that needs to be updated. The class-level documentation in `UnitTest.h` around lines 35-100 provides detailed information about the unit test framework's behavior and should be updated to include the new mode.

# !!!FINISHED!!!