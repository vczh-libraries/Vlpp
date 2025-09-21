# !!!EXECUTION!!!

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

## UPDATE

Good job to guess the silent crash in one shot I am impressive!

I think the same technique for `GetDateTimeInjection` can be applied to two `GetOSDateTimeImpl` functions. What do you think?

# IMPROVEMENT PLAN

## STEP 1: Update IDateTimeImpl Interface Inheritance

```cpp
// In DateTime.h, change this:
class IDateTimeImpl : public virtual Interface

// To this:
#include "../FeatureInjection.h"

class IDateTimeImpl : public virtual feature_injection::IFeatureImpl
```

## STEP 2: Update WindowsDateTimeImpl to use FeatureImpl Base Class

```cpp
// In DateTime.Windows.cpp, change this:
class WindowsDateTimeImpl : public Object, public virtual IDateTimeImpl

// To this:
class WindowsDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
```

## STEP 3: Update LinuxDateTimeImpl to use FeatureImpl Base Class

```cpp
// In DateTime.Linux.cpp, change this:
class LinuxDateTimeImpl : public Object, public virtual IDateTimeImpl

// To this:
class LinuxDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
```

## STEP 4: Replace Global Injection State with FeatureInjection Instance

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

## STEP 5: Update InjectDateTimeImpl Function

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

## STEP 6: Update All DateTime Methods to Use New Framework

```cpp
// In DateTime.cpp, change all occurrences of:
GetDateTimeImpl()->

// To:
dateTimeInjection.Get()->
```

## STEP 7: Add Required Include Directives

```cpp
// At the top of DateTime.cpp, add:
#include "../FeatureInjection.h"
```

## STEP 8: Add EjectAll Method to FeatureInjection Framework

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

## STEP 9: Add EjectDateTimeImpl Function

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

## STEP 10: Apply Lazy Initialization to GetOSDateTimeImpl Functions

```cpp
// In DateTime.Windows.cpp, change this:
WindowsDateTimeImpl osDateTimeImpl;

IDateTimeImpl* GetOSDateTimeImpl()
{
    return &osDateTimeImpl;
}

// To this:
IDateTimeImpl* GetOSDateTimeImpl()
{
    static WindowsDateTimeImpl osDateTimeImpl;
    return &osDateTimeImpl;
}
```

```cpp
// In DateTime.Linux.cpp, change this:
LinuxDateTimeImpl osDateTimeImpl;

IDateTimeImpl* GetOSDateTimeImpl()
{
    return &osDateTimeImpl;
}

// To this:
IDateTimeImpl* GetOSDateTimeImpl()
{
    static LinuxDateTimeImpl osDateTimeImpl;
    return &osDateTimeImpl;
}
```

# TEST PLAN

## FeatureInjection Framework Tests

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

## DateTime Injection/Ejection Tests

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

# !!!FINISHED!!!
