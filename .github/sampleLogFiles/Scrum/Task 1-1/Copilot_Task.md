# !!!TASK!!!

# PROBLEM DESCRIPTION

## Task 1-1: Convert suppressFailure to Enum and Implement /C Mode Logic

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

## UPDATE

In `FailureMode`, Actually C means Copilot not Crash. Please rename, but it doesn't change how it is used.

## Context Analysis

The current unit test framework in `vl::unittest::UnitTest` (defined in Source/UnitTest/UnitTest.h and Source/UnitTest/UnitTest.cpp) has two operational modes:

1. **Debug mode (`/D`)**: `suppressFailure = false` - Exceptions are not caught, allowing debuggers to catch them directly
2. **Release mode (`/R`)**: `suppressFailure = true` - Exceptions are caught and suppressed, allowing tests to continue

The goal is to add a third mode:
3. **Copilot mode (`/C`)**: Should print the first error and immediately exit the program

# HIGH-LEVEL DESIGN

## Solution Architecture

### Core Design Principle

The implementation will follow the existing architectural pattern where the `execution_impl` namespace contains the core logic and global state. The key insight is to replace the boolean `suppressFailure` with an enum class that can represent three distinct states, allowing for more sophisticated failure handling logic.

### Design Components

#### 1. Enum Class Introduction

Create a new enum class `vl::unittest::execution_impl::FailureMode` with three values:
- `Debug`: Corresponds to current `/D` behavior (no exception suppression)
- `Release`: Corresponds to current `/R` behavior (exception suppression and continuation)  
- `Copilot`: New `/C` behavior (exception suppression with immediate termination)

#### 2. Global State Modification

Replace the current `bool suppressFailure` global variable in `execution_impl` namespace with `FailureMode failureMode`. This maintains the existing global state pattern while enabling tri-state logic.

#### 3. Command Line Parsing Enhancement

Extend the command line parsing logic in `UnitTest::RunAndDisposeTests` to recognize `/C` option and ensure mutual exclusivity with `/D` and `/R` options, following the existing validation pattern.

#### 4. Exception Handling Strategy

The key architectural insight is that `SuppressCppFailure` and `SuppressCFailure` functions are only called when exception suppression is needed (i.e., not in Debug mode). This makes them ideal locations for implementing the copilot behavior:

- In `Release` mode: Continue current behavior (catch and record failures)
- In `Copilot` mode: Catch exceptions, record failures via `RecordFailure`, then rethrow to cause immediate program termination

#### 5. Control Flow Modification

Update `ExecuteAndSuppressFailure` template function to check the enum value instead of the boolean, maintaining the same branching logic but with enhanced state representation.

## Implementation Strategy

### Phase 1: Core Infrastructure Changes

1. **Enum Definition**: Add `enum class FailureMode { Debug, Release, Copilot };` in `execution_impl` namespace
2. **Variable Replacement**: Replace `bool suppressFailure` with `FailureMode failureMode`  
3. **Logic Updates**: Update all boolean checks to enum comparisons

### Phase 2: Command Line Interface Enhancement

1. **Option Parsing**: Add `/C` recognition in the option parsing loop
2. **Validation Logic**: Extend mutual exclusivity checks to include `/C`
3. **Default Behavior**: Maintain existing default behavior when no option is specified

### Phase 3: Exception Handling Logic

1. **SuppressCppFailure Enhancement**: Add failureMode check and rethrow logic for Copilot mode
2. **SuppressCFailure Enhancement**: Similar enhancement for structured exception handling  
3. **Message Updates**: Update status messages to reflect the three modes

## Technical Rationale

### Why This Approach is Optimal

1. **Architectural Consistency**: Follows existing patterns in the codebase (global state in `execution_impl`, template-based exception handling)

2. **Minimal Code Changes**: Leverages existing exception handling infrastructure rather than creating new pathways

3. **Clean Separation of Concerns**: Exception recording (`RecordFailure`) remains unchanged, while termination logic is added at the appropriate layer

4. **Type Safety**: Enum class provides compile-time validation and eliminates possibility of invalid state combinations

5. **Extensibility**: Future modes can be easily added without architectural changes

### Key Implementation Points

1. **Exception Flow**: In Copilot mode, exceptions follow this path:
   - Exception occurs in test code
   - `ExecuteAndSuppressFailure` routes to `SuppressCFailure` 
   - `SuppressCFailure` routes to `SuppressCppFailure`
   - `SuppressCppFailure` catches exception, calls `RecordFailure`, then rethrows
   - Rethrown exception causes program termination

2. **Message Handling**: Error messages are printed via `RecordFailure` before rethrowing, ensuring users see the failure reason before program exit

3. **Exit Code**: Program will exit with non-zero return code due to unhandled exception, maintaining expected behavior for testing frameworks

## Files to Modify

1. **Source/UnitTest/UnitTest.cpp**: Primary implementation changes
   - Add enum definition in `execution_impl` namespace  
   - Replace boolean variable and update all references
   - Enhance command line parsing logic
   - Modify exception handling functions

2. **Source/UnitTest/UnitTest.h**: Interface updates
   - Update class documentation to describe new `/C` mode
   - Update `PrintUsages()` message format

## Testing Strategy

The existing `FakeUnitTestProject` provides comprehensive test coverage for the new `/C` mode:

- **TestFileAssertFailed.cpp**: Tests assertion failures and TEST_ERROR/TEST_EXCEPTION scenarios
- **TestFileException.cpp**: Tests various exception types (Error, Exception, runtime exceptions)  
- **TestFileConfigFailed.cpp**: Tests configuration error handling
- **TestFilePassed.cpp**: Ensures normal operation is unaffected

Manual testing approach:
1. Run `FakeUnitTestProject /C` and verify immediate exit after first failure
2. Compare behavior with `/D` and `/R` modes
3. Verify error messages are printed before exit
4. Confirm non-zero exit code on failure

## Backward Compatibility

This design maintains full backward compatibility:
- Existing `/D` and `/R` options behave identically
- Default behavior (no options specified) remains unchanged  
- All existing APIs and interfaces are preserved
- No changes to test authoring patterns or macros

# !!!FINISHED!!!
