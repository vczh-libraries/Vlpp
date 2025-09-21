# !!!PLANNING!!!

# UPDATES

## UPDATE

There are several IDateTimeImpl implementation, everyone should inherit only from FeatureImpl<IDateTimeImpl>, so that they don't have to implement IFeatureImpl members.

## UPDATE

There is no test plan needed as I believe the current unit test already covers.

## UPDATE

I think the behavior of `InjectDateTimeImpl` does not match the design of `FeatureInjection`. We need to add another `EjectDateTimeImpl`. And when `EjectDateTimeImpl(nullptr)`, we should eject all the way to the default one.

But the FeatureInjection does not offer the feature of `eject all the way to the default one`, we should add an `EjectAll()` method to it.

So now we need to test:

- FeatureInjection::EjectAll
  - Just like existing test case, inject three impls and EjectAll, see if Get returns default.
- InjectDateTimeImpl and EjectDateTimeImpl
  - Just one test case, create a mock implementation, redirect everything to the previous implementation, but LocalTime and UtcTime always return 2000-1-1 0:0:0:0.
  - After injecting it, we can call DateTime::LocalTime and UtcTime to see if it works
  - Must call EjectDateTimeImpl(nullptr) otherwise it would affect other test cases. In order to make sure this function is called even test failure happen, we should store the result of LocalTime and UtcTime, eject, and then TEST_ASSERT

## UPDATE

In `MockDateTimeImpl`, I think it is not possible to make a `FIXED_TIME` as it is platform dependent. We can just call `FromDateTime`.

# IMPROVEMENT PLAN

## STEP 1: Update IDateTimeImpl Interface Inheritance

Update the `IDateTimeImpl` interface in `DateTime.h` to inherit from `IFeatureImpl` instead of just `Interface`.

```cpp
// In DateTime.h, change this:
class IDateTimeImpl : public virtual Interface

// To this:
#include "../FeatureInjection.h"

class IDateTimeImpl : public virtual feature_injection::IFeatureImpl
```

This change is necessary because:
- The new feature injection framework requires all implementations to support lifecycle management through `IFeatureImpl`
- This ensures type compatibility with the `FeatureInjection<IDateTimeImpl>` template class
- It provides the foundation for proper chain management and delegation

## STEP 2: Update WindowsDateTimeImpl to use FeatureImpl Base Class

Modify the `WindowsDateTimeImpl` class in `DateTime.Windows.cpp` to inherit from `FeatureImpl<IDateTimeImpl>` instead of implementing `IFeatureImpl` directly.

```cpp
// In DateTime.Windows.cpp, change this:
class WindowsDateTimeImpl : public Object, public virtual IDateTimeImpl

// To this:
class WindowsDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
```

This change is necessary because:
- `FeatureImpl<IDateTimeImpl>` provides automatic implementations of `GetPreviousImpl()`, `BeginInjection()`, and `EndInjection()` methods
- It eliminates boilerplate code and reduces the chance of implementation errors
- The base class handles all injection lifecycle management automatically
- Implementations can focus purely on DateTime-specific functionality

## STEP 3: Update LinuxDateTimeImpl to use FeatureImpl Base Class

Similarly, modify the `LinuxDateTimeImpl` class in `DateTime.Linux.cpp` to inherit from `FeatureImpl<IDateTimeImpl>`.

```cpp
// In DateTime.Linux.cpp, change this:
class LinuxDateTimeImpl : public Object, public virtual IDateTimeImpl

// To this:
class LinuxDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
```

This change is necessary for the same reasons as the Windows implementation:
- Provides automatic lifecycle management
- Ensures consistent behavior across platforms
- Reduces code duplication and potential bugs

## STEP 4: Replace Global Injection State with FeatureInjection Instance

Replace the existing global injection mechanism in `DateTime.cpp` with a `FeatureInjection<IDateTimeImpl>` instance.

```cpp
// In DateTime.cpp, remove these lines:
IDateTimeImpl* dateTimeImpl = nullptr;

IDateTimeImpl* GetDateTimeImpl()
{
    return dateTimeImpl ? dateTimeImpl : GetOSDateTimeImpl();
}

// Add this instead:
feature_injection::FeatureInjection<IDateTimeImpl> dateTimeInjection(GetOSDateTimeImpl());
```

This change is necessary because:
- The new framework eliminates the need for manual null checks and fallback logic
- Constructor-based initialization ensures the framework always has a valid implementation
- It provides enhanced chain management capabilities not available in the old implementation
- The framework manages the linked list of implementations automatically

## STEP 5: Update InjectDateTimeImpl Function

Update the `InjectDateTimeImpl()` function to delegate to the new framework.

```cpp
// In DateTime.cpp, change this:
void InjectDateTimeImpl(IDateTimeImpl* impl)
{
    dateTimeImpl = impl;
}

// To this:
void InjectDateTimeImpl(IDateTimeImpl* impl)
{
    dateTimeInjection.Inject(impl);
}
```

This change is necessary because:
- It maintains complete backward compatibility with existing client code
- The framework's `Inject()` method provides validation and proper chain management
- It enables enhanced features like proper ejection from the middle of chains

## STEP 6: Update All DateTime Methods to Use New Framework

Replace all calls to `GetDateTimeImpl()` with calls to `dateTimeInjection.Get()` in all DateTime static methods.

```cpp
// In DateTime.cpp, change all occurrences of:
GetDateTimeImpl()->

// To:
dateTimeInjection.Get()->
```

For example:
```cpp
// Change this:
DateTime DateTime::LocalTime()
{
    return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->LocalTime());
}

// To this:
DateTime DateTime::LocalTime()
{
    return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->LocalTime());
}
```

This change is necessary because:
- The framework's `Get()` method always returns a valid implementation (never nullptr)
- It provides type-safe access to the current implementation
- It eliminates the need for manual null checks in every DateTime method

## STEP 7: Add Required Include Directives

Add the necessary include directive for the feature injection framework in `DateTime.cpp`.

```cpp
// At the top of DateTime.cpp, add:
#include "../FeatureInjection.h"
```

This change is necessary because:
- The global `FeatureInjection<IDateTimeImpl>` instance requires the framework headers
- It ensures proper compilation and type resolution

## STEP 8: Add EjectAll Method to FeatureInjection Framework

Add an `EjectAll()` method to the `FeatureInjection<TImpl>` template class in `FeatureInjection.h`.

```cpp
// In FeatureInjection.h, add this method to the FeatureInjection class:
/// <summary>Eject all implementations, restoring the default implementation.</summary>
void EjectAll()
{
    while (currentImpl->GetPreviousImpl() != nullptr)
    {
        auto toEject = currentImpl;
        currentImpl = currentImpl->GetPreviousImpl();
        toEject->EndInjection();
    }
}
```

This change is necessary because:
- It provides a convenient way to reset the injection chain to the default implementation
- It matches the expected behavior of `EjectDateTimeImpl(nullptr)` which should eject all injected implementations
- It ensures proper cleanup of all injected implementations in the chain

## STEP 9: Add EjectDateTimeImpl Function

Add the `EjectDateTimeImpl()` function in `DateTime.h` and implement it in `DateTime.cpp`.

```cpp
// In DateTime.h, add this declaration:
extern void EjectDateTimeImpl(IDateTimeImpl* impl);

// In DateTime.cpp, add this implementation:
void EjectDateTimeImpl(IDateTimeImpl* impl)
{
    if (impl == nullptr)
    {
        dateTimeInjection.EjectAll();
    }
    else
    {
        dateTimeInjection.Eject(impl);
    }
}
```

This change is necessary because:
- It provides the missing ejection functionality to match the injection design pattern
- When `impl` is `nullptr`, it ejects all implementations back to the default, which is the expected behavior
- When `impl` is not `nullptr`, it ejects the specific implementation from the chain
- It maintains API consistency with the injection pattern

# TEST PLAN

## FeatureInjection Framework Tests

### Test EjectAll Method

Add a test case for the new `EjectAll()` method in the existing feature injection test file.

```cpp
TEST_CASE(L"FeatureInjection EjectAll")
{
    // Create test implementations (reuse existing test implementations)
    MockFeatureImpl impl1, impl2, impl3;
    MockFeatureImpl defaultImpl;
    
    // Initialize with default
    FeatureInjection<IMockFeature> injection(&defaultImpl);
    
    // Inject three implementations
    injection.Inject(&impl1);
    injection.Inject(&impl2);  
    injection.Inject(&impl3);
    
    // Verify current is impl3
    TEST_ASSERT(injection.Get() == &impl3);
    
    // EjectAll and verify default is restored
    injection.EjectAll();
    TEST_ASSERT(injection.Get() == &defaultImpl);
}
```

This test is necessary because:
- It verifies that `EjectAll()` properly removes all injected implementations
- It ensures the chain is correctly restored to the default implementation
- It validates that the lifecycle methods are called properly during ejection

## DateTime Injection/Ejection Tests

### Test InjectDateTimeImpl and EjectDateTimeImpl

Add a test case for DateTime injection and ejection in the DateTime test file.

```cpp
TEST_CASE(L"DateTime Injection and Ejection")
{
    // Create mock implementation that returns fixed time
    class MockDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
    {
    public:
        DateTime FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds) override
        {
            return Previous()->FromDateTime(_year, _month, _day, _hour, _minute, _second, _milliseconds);
        }
        
        DateTime FromOSInternal(vuint64_t osInternal) override
        {
            return Previous()->FromOSInternal(osInternal);
        }
        
        vuint64_t LocalTime() override
        {
            // Return fixed time: 2000-1-1 0:0:0:0
            DateTime fixedDateTime = Previous()->FromDateTime(2000, 1, 1, 0, 0, 0, 0);
            return fixedDateTime.osInternal;
        }
        
        vuint64_t UtcTime() override
        {
            // Return fixed time: 2000-1-1 0:0:0:0
            DateTime fixedDateTime = Previous()->FromDateTime(2000, 1, 1, 0, 0, 0, 0);
            return fixedDateTime.osInternal;
        }
        
        vuint64_t LocalToUtcTime(vuint64_t osInternal) override
        {
            return Previous()->LocalToUtcTime(osInternal);
        }
        
        vuint64_t UtcToLocalTime(vuint64_t osInternal) override
        {
            return Previous()->UtcToLocalTime(osInternal);
        }
        
        vuint64_t Forward(vuint64_t osInternal, vuint64_t milliseconds) override
        {
            return Previous()->Forward(osInternal, milliseconds);
        }
        
        vuint64_t Backward(vuint64_t osInternal, vuint64_t milliseconds) override
        {
            return Previous()->Backward(osInternal, milliseconds);
        }
    };
    
    MockDateTimeImpl mockImpl;
    
    // Inject mock implementation
    InjectDateTimeImpl(&mockImpl);
    
    // Store results before ejection to ensure cleanup happens even on test failure
    DateTime localResult = DateTime::LocalTime();
    DateTime utcResult = DateTime::UtcTime();
    
    // Always eject to ensure no side effects on other tests
    EjectDateTimeImpl(nullptr);
    
    // Verify the mock implementation was used
    TEST_ASSERT(localResult.year == 2000);
    TEST_ASSERT(localResult.month == 1);
    TEST_ASSERT(localResult.day == 1);
    TEST_ASSERT(localResult.hour == 0);
    TEST_ASSERT(localResult.minute == 0);
    TEST_ASSERT(localResult.second == 0);
    TEST_ASSERT(localResult.milliseconds == 0);
    
    TEST_ASSERT(utcResult.year == 2000);
    TEST_ASSERT(utcResult.month == 1);
    TEST_ASSERT(utcResult.day == 1);
    TEST_ASSERT(utcResult.hour == 0);
    TEST_ASSERT(utcResult.minute == 0);
    TEST_ASSERT(utcResult.second == 0);
    TEST_ASSERT(utcResult.milliseconds == 0);
}
```

This test is necessary because:
- It verifies that `InjectDateTimeImpl()` properly injects the implementation into the chain
- It ensures that the injected implementation is actually used by DateTime methods
- It validates that `EjectDateTimeImpl(nullptr)` properly restores the default implementation
- It demonstrates proper delegation to previous implementations for non-overridden methods
- It ensures test isolation by always cleaning up injections

# !!!FINISHED!!!
