# !!!PLANNING!!!

# UPDATES

## UPDATE

`STEP 4: Add Test File to UnitTest Project` is not necessary, the file is already there.

Since this is a pure unit test task, so no `# TEST PLAN` is necessary.

# IMPROVEMENT PLAN

## STEP 1: Enhance FeatureImpl<TImpl> with Previous() Method

Add a protected `Previous()` method to the `FeatureImpl<TImpl>` class in `FeatureInjection.h`:

```cpp
protected:
    /// <summary>Get the previous implementation with type safety.</summary>
    /// <returns>The previous implementation, or nullptr if this is the first in chain.</returns>
    TImpl* Previous()
    {
        return _previousImpl; // Already verified and cast during BeginInjection
    }
```

This method will be added after the existing `BeginInjection(TImpl* _previousImpl)` virtual method in the `FeatureImpl<TImpl>` class.

**Why this change is necessary**: This enhancement makes the framework more useful for all implementations by providing type-safe access to the previous implementation. It eliminates the need for manual tracking in mock implementations and provides a standard way for implementations to delegate to previous implementations. The `_previousImpl` has already been verified and cast during `BeginInjection`, so this simply exposes the already safe reference.

## STEP 2: Create Simplified Mock Interface and Implementation

Create a simple mock interface `IMockFeatureImpl` and mock implementation `MockFeatureImpl` in `TestFeatureInjection.cpp`:

```cpp
class IMockFeatureImpl
{
public:
    virtual WString GetFeatures() = 0;
};

class MockFeatureImpl : public vl::feature_injection::FeatureImpl<MockFeatureImpl>, public virtual IMockFeatureImpl
{
public:
    // Call tracking - public for easy testing access
    vint beginInjectionCallCount = 0;
    vint endInjectionCallCount = 0;
    
    // Feature identification
    WString name;
    
    MockFeatureImpl(const WString& _name) : name(_name) {}
    
    // Override lifecycle methods to track calls
    void BeginInjection(MockFeatureImpl* previous) override
    {
        beginInjectionCallCount++;
    }
    
    void EndInjection() override
    {
        endInjectionCallCount++;
    }
    
    // Recursive chain visualization using framework's Previous() method
    WString GetFeatures() override
    {
        auto previous = Previous();
        if (previous == nullptr)
        {
            return name;
        }
        return previous->GetFeatures() + L"-" + name;
    }
};
```

**Why this change is necessary**: This simplified mock design provides all necessary testing capabilities without complexity. The public call counters allow direct verification of lifecycle method calls. The `GetFeatures()` method provides clear visualization of the injection chain structure using the framework's `Previous()` method, making test assertions intuitive and readable.

## STEP 3: Implement Comprehensive Test Categories

Create test categories covering all aspects of the feature injection framework:

### Basic Functionality Tests
```cpp
TEST_CATEGORY(L"Basic Functionality")
{
    TEST_CASE(L"Constructor with valid default implementation")
    {
        auto defaultImpl = new MockFeatureImpl(L"Default");
        FeatureInjection<MockFeatureImpl> injection(defaultImpl);
        TEST_ASSERT(injection.Get()->GetFeatures() == L"Default");
    }
    
    TEST_CASE(L"Constructor with null parameter throws error")
    {
        // Test that constructor properly validates null parameter
    }
    
    TEST_CASE(L"Get returns current implementation")
    {
        // Test that Get() always returns the current active implementation
    }
}
```

### Injection Chain Management Tests
```cpp
TEST_CATEGORY(L"Injection Chain Management")
{
    TEST_CASE(L"Single injection creates proper chain")
    {
        auto defaultImpl = new MockFeatureImpl(L"Default");
        auto impl1 = new MockFeatureImpl(L"Impl1");
        FeatureInjection<MockFeatureImpl> injection(defaultImpl);
        
        injection.Inject(impl1);
        TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1");
        TEST_ASSERT(impl1->beginInjectionCallCount == 1);
    }
    
    TEST_CASE(L"Multiple injections build correct chain")
    {
        // Test sequential injections create proper linked chain
    }
    
    TEST_CASE(L"Ejection of last implementation")
    {
        // Test ejecting the most recently injected implementation
    }
    
    TEST_CASE(L"Ejection from middle of chain")
    {
        // Test ejecting implementation from middle, verify cleanup sequence
    }
    
    TEST_CASE(L"Injection after ejection")
    {
        // Test that framework state is properly reset after ejection
    }
}
```

### Lifecycle Method Verification Tests
```cpp
TEST_CATEGORY(L"Lifecycle Method Verification")
{
    TEST_CASE(L"BeginInjection call count verification")
    {
        // Verify beginInjectionCallCount increments correctly
    }
    
    TEST_CASE(L"EndInjection call count verification")
    {
        // Verify endInjectionCallCount increments correctly during ejection
    }
    
    TEST_CASE(L"Ejection calls EndInjection in correct order")
    {
        // Verify that ejection calls EndInjection on all removed implementations
    }
}
```

### Error Handling Tests
```cpp
TEST_CATEGORY(L"Error Handling")
{
    TEST_CASE(L"Inject null implementation throws error")
    {
        // Test error handling for null parameter in Inject
    }
    
    TEST_CASE(L"Eject null implementation throws error")
    {
        // Test error handling for null parameter in Eject
    }
    
    TEST_CASE(L"Eject non-existent implementation throws error")
    {
        // Test error handling for implementation not in chain
    }
}
```

**Why this change is necessary**: These comprehensive test categories ensure all aspects of the framework are validated. The tests verify both positive scenarios (correct behavior) and negative scenarios (error handling). Using the simplified mock with `GetFeatures()` makes test assertions clear and maintainable. The organization into categories makes the test suite easy to understand and maintain.

# !!!FINISHED!!!
