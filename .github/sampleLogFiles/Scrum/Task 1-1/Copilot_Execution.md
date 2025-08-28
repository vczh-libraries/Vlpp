# !!!EXECUTION!!!

## Improvement Plan

### Task 1: Core Infrastructure - Convert suppressFailure to FailureMode Enum

**Location**: `Source/UnitTest/UnitTest.cpp` in the `execution_impl` namespace

**Add enum class definition** (after line 28):
```cpp
enum class FailureMode
{
    Debug,    // corresponds to /D - no exception suppression
    Release,  // corresponds to /R - suppress and continue  
    Copilot   // corresponds to /C - suppress, record, and rethrow
};
```

**Replace boolean variable** (line 39):
```cpp
bool suppressFailure = false;
```
**With**:
```cpp
FailureMode failureMode = FailureMode::Debug;
```

### Task 2: Update Command Line Parsing Logic

**Location**: `Source/UnitTest/UnitTest.cpp` in `RunAndDisposeTests` method

**Add _C variable** (after line 140):
```cpp
bool _C = false;
```

**Extend parsing loop** (in the option parsing loop around line 148):
```cpp
else if (option == L"/C")
{
    _C = true;
}
```

**Update mutual exclusivity check** (line 157):
```cpp
if (unrecognized || (_D && _R))
{
    return PrintUsages();
}
```
**With**:
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

**Update mode assignment logic** (lines 162-170):
```cpp
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
**With**:
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

### Task 3: Update ExecuteAndSuppressFailure Control Flow

**Location**: `Source/UnitTest/UnitTest.cpp` lines 90-98

**Replace**:
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
**With**:
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

### Task 4: Implement Copilot Mode Exception Handling

**Location**: `Source/UnitTest/UnitTest.cpp` in `SuppressCppFailure` (lines 55-75)

**Add rethrow logic after each `RecordFailure` call**:
```cpp
catch (const UnitTestAssertError& e)
{
    RecordFailure(e.message);
}
```
**With**:
```cpp
catch (const UnitTestAssertError& e)
{
    RecordFailure(e.message);
    if (failureMode == FailureMode::Copilot)
    {
        throw;
    }
}
```

**Similar pattern for all other catch blocks in SuppressCppFailure**:
```cpp
catch (const UnitTestConfigError& e)
{
    RecordFailure(e.message);
    if (failureMode == FailureMode::Copilot)
    {
        throw;
    }
}
```

```cpp
catch (const Error& e)
{
    RecordFailure(e.Description());
    if (failureMode == FailureMode::Copilot)
    {
        throw;
    }
}
```

```cpp
catch (const Exception& e)
{
    RecordFailure(e.Message());
    if (failureMode == FailureMode::Copilot)
    {
        throw;
    }
}
```

```cpp
catch (...)
{
    RecordFailure(L"Unknown exception occurred!");
    if (failureMode == FailureMode::Copilot)
    {
        throw;
    }
}
```

**Location**: `Source/UnitTest/UnitTest.cpp` in `SuppressCFailure` (lines 77-88)

**Replace**:
```cpp
__except (/*EXCEPTION_EXECUTE_HANDLER*/ 1)
{
    RecordFailure(L"Runtime exception occurred!");
}
```
**With**:
```cpp
__except (/*EXCEPTION_EXECUTE_HANDLER*/ 1)
{
    RecordFailure(L"Runtime exception occurred!");
    if (failureMode == FailureMode::Copilot)
    {
        throw Exception(L"Runtime exception occurred!");
    }
}
```

### Task 5: Update Status Messages

**Location**: `Source/UnitTest/UnitTest.cpp` in `RunAndDisposeTests` (lines 175-180)

**Replace**:
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
**With**:
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

### Task 6: Update Documentation and Help Text

**Location**: `Source/UnitTest/UnitTest.cpp` in `PrintUsages` (line 127)

**Replace**:
```cpp
PrintMessage(L"Usage: [/D | /R] {/F:TestFile}", MessageKind::Error);
```
**With**:
```cpp
PrintMessage(L"Usage: [/D | /R | /C] {/F:TestFile}", MessageKind::Error);
```

**Location**: `Source/UnitTest/UnitTest.h` class documentation (lines 35-100)

**Add documentation for the new `/C` mode**:
- Description of the new `/C` (Copilot) mode
- Explanation that `/C` mode prints the first error and immediately exits
- Note about mutual exclusivity of `/D`, `/R`, and `/C` options

## Test Plan

No new test files need to be created. The existing `FakeUnitTestProject` provides comprehensive coverage for testing the new `/C` mode functionality.

# !!!FINISHED!!!
