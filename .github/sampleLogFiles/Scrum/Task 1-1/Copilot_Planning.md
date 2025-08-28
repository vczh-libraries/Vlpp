# !!!PLANNING!!!

# UPDATES

## UPDATE

FailureMode failureMode = FailureMode::Release;

Actually the default value should be `Debug`, it keeps the semantic of `bool suppressFailure = false;` unchanged.

## UPDATE

if (unrecognized || (_D && _R) || (_D && _C) || (_R && _C))

It looks funny. You can now make a counter, if _D then ++, repeat for _C and _R, and you can check if it is exactly one.

## Improvement Plan

### Understanding the Current Architecture

Based on thorough analysis of the existing code in `Source/UnitTest/UnitTest.h` and `Source/UnitTest/UnitTest.cpp`, the unit test framework uses a well-structured architecture:

1. **Global State Management**: The `execution_impl` namespace (line 28-39 in UnitTest.cpp) contains all global state including the current `bool suppressFailure` variable
2. **Exception Handling Chain**: The framework uses a sophisticated three-layer exception handling approach:
   - `ExecuteAndSuppressFailure` (lines 90-98) - Controls whether exceptions are caught
   - `SuppressCFailure` (lines 77-88) - Handles structured exceptions (Windows SEH)
   - `SuppressCppFailure` (lines 55-75) - Handles C++ exceptions
3. **Command Line Processing**: `RunAndDisposeTests` (lines 134-241) handles option parsing with mutual exclusivity validation
4. **Cross-Platform Support**: `IsDebuggerAttached()` is implemented in platform-specific files (UnitTest.Windows.cpp and UnitTest.Linux.cpp)

### Task 1: Core Infrastructure - Convert suppressFailure to FailureMode Enum

**Location**: `Source/UnitTest/UnitTest.cpp` in the `execution_impl` namespace

**Current Implementation** (line 39):
```cpp
bool suppressFailure = false;
```

**Proposed Changes**:

1. **Add enum class definition** (after line 28):
```cpp
enum class FailureMode
{
    Debug,    // corresponds to /D - no exception suppression
    Release,  // corresponds to /R - suppress and continue  
    Copilot   // corresponds to /C - suppress, record, and rethrow
};
```

2. **Replace boolean variable** (line 39):
```cpp
FailureMode failureMode = FailureMode::Debug;
```

**Rationale**: The enum clearly represents three distinct operational modes and eliminates the need for additional boolean flags. Using `FailureMode::Debug` as default maintains semantic consistency with the original `bool suppressFailure = false` since Debug mode corresponds to no exception suppression (suppressFailure = false). This preserves the exact same default behavior where exceptions are not suppressed unless explicitly requested via command line options or debugger detection logic.

### Task 2: Update Command Line Parsing Logic

**Location**: `Source/UnitTest/UnitTest.cpp` in `RunAndDisposeTests` method (lines 134-170)

**Current Implementation**:
```cpp
bool _D = false;
bool _R = false;
// ...parsing loop...
if (unrecognized || (_D && _R))
{
    return PrintUsages();
}
if (_D)
{
    suppressFailure = false;
}
else if (_R)
{
    suppressFailure = true;
}
else
{
    suppressFailure = !IsDebuggerAttached();
}
```

**Proposed Changes**:

1. **Add _C variable** (after line 140):
```cpp
bool _C = false;
```

2. **Extend parsing loop** (in the option parsing loop around line 148):
```cpp
else if (option == L"/C")
{
    _C = true;
}
```

3. **Update mutual exclusivity check with counter approach** (line 157):
```cpp
vint modeCount = 0;
if (_D) modeCount++;
if (_R) modeCount++;
if (_C) modeCount++;
if (unrecognized || modeCount > 1)
{
    return PrintUsages();
}
```

4. **Update mode assignment logic** (lines 162-170):
```cpp
if (_D)
{
    failureMode = FailureMode::Debug;
}
else if (_R)
{
    failureMode = FailureMode::Release;
}
else if (_C)
{
    failureMode = FailureMode::Copilot;
}
else
{
    failureMode = IsDebuggerAttached() ? FailureMode::Debug : FailureMode::Release;
}
```

**Rationale**: Using a counter approach for mutual exclusivity validation is cleaner and more scalable than multiple boolean combinations. This makes the code more maintainable when adding future modes. The logic ensures exactly one mode can be specified at a time and preserves the existing behavior where debugger attachment determines the default mode.

### Task 3: Update ExecuteAndSuppressFailure Control Flow

**Location**: `Source/UnitTest/UnitTest.cpp` lines 90-98

**Current Implementation**:
```cpp
template<typename TCallback>
void ExecuteAndSuppressFailure(TCallback&& callback)
{
    if (suppressFailure)
    {
        SuppressCFailure(std::forward<TCallback&&>(callback));
    }
    else
    {
        callback();
    }
}
```

**Proposed Changes**:
```cpp
template<typename TCallback>
void ExecuteAndSuppressFailure(TCallback&& callback)
{
    if (failureMode == FailureMode::Debug)
    {
        callback();
    }
    else
    {
        SuppressCFailure(std::forward<TCallback&&>(callback));
    }
}
```

**Rationale**: The control flow remains binary - either exceptions are suppressed (Release/Copilot modes) or they aren't (Debug mode). This maintains the existing architecture while enabling enum-based logic. Both Release and Copilot modes need exception handling, with the difference implemented in the exception handlers themselves.

### Task 4: Implement Copilot Mode Exception Handling

**Location**: `Source/UnitTest/UnitTest.cpp` in `SuppressCppFailure` (lines 55-75)

**Current Implementation**:
```cpp
template<typename TCallback>
void SuppressCppFailure(TCallback&& callback)
{
    try
    {
        callback();
    }
    catch (const UnitTestAssertError& e)
    {
        RecordFailure(e.message);
    }
    // ... other catch blocks ...
}
```

**Proposed Changes**:
Add rethrow logic after each `RecordFailure` call:
```cpp
template<typename TCallback>
void SuppressCppFailure(TCallback&& callback)
{
    try
    {
        callback();
    }
    catch (const UnitTestAssertError& e)
    {
        RecordFailure(e.message);
        if (failureMode == FailureMode::Copilot)
        {
            throw;
        }
    }
    catch (const UnitTestConfigError& e)
    {
        RecordFailure(e.message);
        if (failureMode == FailureMode::Copilot)
        {
            throw;
        }
    }
    // Similar pattern for Error, Exception, and ... catch blocks
}
```

**Location**: `Source/UnitTest/UnitTest.cpp` in `SuppressCFailure` (lines 77-88)

**Current Implementation**:
```cpp
template<typename TCallback>
void SuppressCFailure(TCallback&& callback)
{
#ifdef VCZH_MSVC
    __try
    {
        SuppressCppFailure(std::forward<TCallback&&>(callback));
    }
    __except (/*EXCEPTION_EXECUTE_HANDLER*/ 1)
    {
        RecordFailure(L"Runtime exception occurred!");
    }
#else
    SuppressCppFailure(callback);
#endif
}
```

**Proposed Changes**:
```cpp
template<typename TCallback>
void SuppressCFailure(TCallback&& callback)
{
#ifdef VCZH_MSVC
    __try
    {
        SuppressCppFailure(std::forward<TCallback&&>(callback));
    }
    __except (/*EXCEPTION_EXECUTE_HANDLER*/ 1)
    {
        RecordFailure(L"Runtime exception occurred!");
        if (failureMode == FailureMode::Copilot)
        {
            // Convert SEH to C++ exception to enable consistent rethrowing
            throw Exception(L"Runtime exception occurred!");
        }
    }
#else
    SuppressCppFailure(callback);
#endif
}
```

**Rationale**: This approach preserves the existing `RecordFailure` behavior (which prints error messages and marks tests as failed) while adding the immediate termination logic for Copilot mode. The `RecordFailure` function correctly handles the error reporting, and then the rethrow causes program termination. For structured exceptions on Windows, we convert to a C++ exception to maintain consistent exception flow.

### Task 5: Update Status Messages

**Location**: `Source/UnitTest/UnitTest.cpp` in `RunAndDisposeTests` (lines 175-180)

**Current Implementation**:
```cpp
if (suppressFailure)
{
    PrintMessage(L"Failures are suppressed.", MessageKind::Info);
}
else
{
    PrintMessage(L"Failures are not suppressed.", MessageKind::Info);
}
```

**Proposed Changes**:
```cpp
switch (failureMode)
{
case FailureMode::Debug:
    PrintMessage(L"Failures are not suppressed.", MessageKind::Info);
    break;
case FailureMode::Release:
    PrintMessage(L"Failures are suppressed.", MessageKind::Info);
    break;
case FailureMode::Copilot:
    PrintMessage(L"Failures cause immediate exit.", MessageKind::Info);
    break;
}
```

**Rationale**: Clear status messages help users understand the current operational mode and expected behavior when failures occur.

### Task 6: Update Documentation and Help Text

**Location**: `Source/UnitTest/UnitTest.cpp` in `PrintUsages` (line 127)

**Current Implementation**:
```cpp
PrintMessage(L"Usage: [/D | /R] {/F:TestFile}", MessageKind::Error);
```

**Proposed Changes**:
```cpp
PrintMessage(L"Usage: [/D | /R | /C] {/F:TestFile}", MessageKind::Error);
```

**Location**: `Source/UnitTest/UnitTest.h` class documentation (lines 35-100)

**Proposed Changes**: Update the class-level documentation to include:
- Description of the new `/C` (Copilot) mode
- Explanation that `/C` mode prints the first error and immediately exits
- Note about mutual exclusivity of `/D`, `/R`, and `/C` options

**Rationale**: Documentation updates ensure users understand the new functionality and can make informed decisions about which mode to use.

## Test Plan

### Overview

The testing strategy leverages the existing `FakeUnitTestProject` which provides comprehensive coverage for all exception types and failure scenarios. The project contains four test files that cover the complete spectrum of unit test framework behavior:

1. **TestFileAssertFailed.cpp**: Contains assertion failures and macro testing
2. **TestFileException.cpp**: Contains various exception types (Error, Exception, runtime exceptions)
3. **TestFileConfigFailed.cpp**: Contains configuration errors
4. **TestFilePassed.cpp**: Contains passing tests

### Test Case 1: Basic Copilot Mode Functionality

**Objective**: Verify that `/C` mode prints the first error and immediately exits

**Test Steps**:
1. Compile the updated UnitTest framework
2. Run `FakeUnitTestProject /C`
3. Observe program behavior

**Expected Results**:
- Program should print startup message: "Failures cause immediate exit."
- First test file execution should begin: "TestFileAssertFailed.cpp"
- First assertion failure should be printed: "Assertion failure: false"
- Program should immediately exit with non-zero exit code
- No subsequent test cases should execute

**Corner Cases**:
- Verify that error message is printed before exit (via `RecordFailure`)
- Confirm exit code is non-zero
- Ensure no test result summary is printed (since program exits immediately)

### Test Case 2: Command Line Option Mutual Exclusivity

**Objective**: Verify that `/C` is mutually exclusive with `/D` and `/R`

**Test Steps**:
1. Run `FakeUnitTestProject /D /C`
2. Run `FakeUnitTestProject /R /C`
3. Run `FakeUnitTestProject /D /R /C`

**Expected Results**:
- All three commands should print the updated usage message: "Usage: [/D | /R | /C] {/F:TestFile}"
- All three commands should exit with non-zero return code
- No test execution should occur

### Test Case 3: Exception Type Coverage

**Objective**: Verify `/C` mode handles all exception types correctly

**Test Steps**:
1. Create individual test files that trigger specific exception types:
   - UnitTestAssertError (from TEST_ASSERT failure)
   - UnitTestConfigError (from invalid TEST_ASSERT placement)
   - Error class exceptions
   - Exception class exceptions
   - Runtime exceptions (division by zero, access violations)

**Expected Results**:
- Each exception type should be caught, recorded via `RecordFailure`, and rethrown
- Program should exit immediately after printing the error message
- Error messages should be appropriate for each exception type

**Verification**: The existing `TestFileException.cpp` already contains comprehensive coverage:
- `throw vl::Error(L"An error.")` - tests Error exception handling
- `throw vl::Exception(L"An exception.")` - tests Exception handling  
- `throw 0` - tests unknown exception handling
- Division by zero - tests structured exception handling (Windows)
- Access violation - tests structured exception handling (Windows)

### Test Case 4: Backward Compatibility

**Objective**: Ensure existing `/D` and `/R` modes continue to work unchanged

**Test Steps**:
1. Run `FakeUnitTestProject /D` 
2. Run `FakeUnitTestProject /R`
3. Run `FakeUnitTestProject` (no options)

**Expected Results**:
- `/D` mode: Should display "Failures are not suppressed." and allow debugger to catch exceptions
- `/R` mode: Should display "Failures are suppressed." and continue execution after failures
- No options: Should behave as `/R` mode when no debugger is attached, `/D` when debugger is present

### Test Case 5: Status Message Verification

**Objective**: Verify correct status messages are displayed for each mode

**Test Steps**:
1. Run tests with each option and verify initial status message
2. Check that status messages accurately reflect the behavior

**Expected Results**:
- `/D`: "Failures are not suppressed."
- `/R`: "Failures are suppressed."  
- `/C`: "Failures cause immediate exit."

### Test Case 6: Platform-Specific Behavior

**Objective**: Ensure `/C` mode works correctly on both Windows and Linux

**Test Steps**:
1. Test structured exception handling on Windows (access violations, division by zero)
2. Test C++ exception handling on both platforms
3. Verify consistent behavior across platforms

**Expected Results**:
- Windows: Both C++ exceptions and structured exceptions should be properly handled in `/C` mode
- Linux: C++ exceptions should be properly handled (structured exceptions not applicable)
- Consistent error reporting and immediate exit behavior on both platforms

### Test Case 7: File Filtering Interaction

**Objective**: Verify `/C` mode works correctly with `/F:` file filtering

**Test Steps**:
1. Run `FakeUnitTestProject /C /F:TestFilePassed.cpp`
2. Run `FakeUnitTestProject /C /F:TestFileAssertFailed.cpp`

**Expected Results**:
- When filtering to passing tests: Should complete successfully with appropriate messages
- When filtering to failing tests: Should exit immediately after first failure in filtered file
- File filtering should not interfere with `/C` mode behavior

### Testing Methodology

**Manual Testing Approach**: 
The unit test framework is inherently difficult to test programmatically since it controls the test execution environment. The recommended approach is manual testing with the `FakeUnitTestProject`:

1. **Build Verification**: Ensure the solution compiles without errors after changes
2. **Behavioral Testing**: Run manual tests with different combinations of options
3. **Comparison Testing**: Compare `/C` behavior against existing `/D` and `/R` modes
4. **Edge Case Testing**: Test invalid option combinations and error scenarios

**Automation Considerations**:
While full automation is challenging, some aspects can be verified:
- Return codes can be checked programmatically
- Output parsing can verify that appropriate error messages are printed
- Process termination timing can be verified (immediate vs. continued execution)

### Test Coverage Assessment

The existing `FakeUnitTestProject` provides excellent coverage because:

1. **Exception Type Coverage**: All exception types handled by the framework are represented
2. **Test Structure Coverage**: Tests are organized in files, categories, and cases - covering all execution contexts
3. **Configuration Coverage**: Invalid TEST_ASSERT placement tests configuration error handling
4. **Success Path Coverage**: Passing tests ensure normal operation is unaffected

**Additional Testing Considerations**:
- Memory leak testing should be performed to ensure immediate exit doesn't cause resource leaks
- Performance testing should verify that the enum-based approach doesn't introduce overhead
- Integration testing should ensure the changes don't affect other components that might depend on the unit test framework

# !!!FINISHED!!!
