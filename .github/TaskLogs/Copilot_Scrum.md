# !!!SCRUM!!!

# DESIGN REQUEST

There is `IDateTimeImpl`, `InjectDateTimeImpl`, `GetOSDateTimeImpl` and `GetDateTimeImpl`. This is a pattern of feature injection. I would like to create a construction so that I can continue to make more feature injections for other feature

No unit test is needed in this design. Such constructions can be written to `FeatureInjection.h` and `FeatureInjection.cpp`.

My idea is that, we have an interface for all feature injection interface like:
```cpp
class IFeatureImpl : public virtual Interface
{
public:
    virtual IFeatureImpl* GetPreviousImpl() = 0;
    virtual void BeginInjection(IFeatureImpl* previousImpl) = 0;
    virtual void EndInjection() = 0;
};
```

A feature injection may need to know the implementation of the currently installed one, and multiple feature injection could form a linked list. IFeatureImpl will be the root interface of all specific features.

And then we will have a registration helper
```cpp
template<typename TImpl>
struct FeatureInjection
{
    TImpl* Get();
    void Inject(TImpl* impl);
    void Eject(TImpl* impl)
};
```

At the beginning Get() returns nullptr obviously. At least one Inject is needed to make Get() returns a valid value.

By calling Inject, impl->BeginInjection will be called with the return value of Get() and after that Get() returns impl.

By calling Eject, impl->EndInjection will be called and Get() will be restored.

It is important to remember, Eject does not require to pass the value of the last Inject call. It could be the one in the middle. In this case, the last one all the way to the Eject argument will be finalize (aka EndInjection) one by one in order.

I want no memory allocation in FeatureInjection.

## UPDATE

I think the first 3 tasks could be merged to one
I think the last task could be removed, no change is needed for knowledge base

## UPDATE

I believe in task 1 you mentioned about DateTime things, we should clearly leave all of that to task 2

`FeatureInjection<IDateTimeImpl>` will server as a global variable in DateTime.cpp, we still keep InjectDateTimeImpl but redirect its implementation to FeatureInjection

IDateTimeImpl will clearly need to inherits from IFeatureImpl

## UPDATE

For Task 2. In DateTime.cpp. now we can remove `dateTimeImpl` and `GetDateTimeImpl`, the `FeatureInjection<IDateTimeImpl>` will be enough to replace both of them.

But clearly we need to initialize it with a default implementation, so maybe add a constructor argument to `FeatureInjection<T>`, and in DateTime.cpp we could just pass `GetOSDateTimeImpl()` to it?

# TASKS

- [ ] TASK No.1: Implement complete feature injection framework
- [ ] TASK No.2: Migrate DateTime injection to use the new framework

## TASK No.1: Implement complete feature injection framework

This task implements the entire feature injection framework including the base interface and template helper class. The framework provides a generic way to create injection patterns for any feature while maintaining backward compatibility with existing code.

### what to be done

- Add `IFeatureImpl` interface declaration to `FeatureInjection.h` with the three pure virtual methods: `GetPreviousImpl()`, `BeginInjection(IFeatureImpl* previousImpl)`, and `EndInjection()`
- Implement `FeatureInjection<TImpl>` template class in `FeatureInjection.h` with private member to track current implementation pointer
- Add constructor to `FeatureInjection<TImpl>` that takes a default implementation argument to initialize the framework
- Implement `Get()` method that returns the current implementation (never nullptr after proper initialization)
- Implement `Inject(TImpl* impl)` method that calls `impl->BeginInjection(current)` and updates current
- Implement `Eject(TImpl* impl)` method that finds impl in the chain and calls `EndInjection()` on all implementations from current down to impl
- Ensure no dynamic memory allocation is used in the implementation
- Add proper error handling for invalid eject operations
- Include comprehensive documentation explaining the injection lifecycle and linked list behavior
- Include namespace structure consistent with existing Vlpp code organization

### how to test it

- Compilation verification to ensure interface syntax is correct
- Verify interface can be inherited by concrete implementation classes
- Check that the interface follows proper virtual inheritance patterns
- Test basic injection and ejection functionality with mock implementations
- Verify linked list traversal works correctly during ejection
- Test edge cases like ejecting from empty chain or non-existent implementations
- Verify no memory leaks occur during injection/ejection cycles
- Test multiple injection and ejection scenarios to ensure chain integrity

### rationale

This task focuses purely on the generic feature injection framework without any specific feature implementations. The interface design and template implementation are tightly coupled and should be implemented together as a complete unit. The framework provides the foundation for all future feature injections while maintaining the no-allocation constraint critical for Vlpp's design philosophy. The linked list design allows multiple injections to form chains where implementations can delegate to previous ones, enabling sophisticated composition patterns. By keeping this task generic, it ensures the framework can be applied to any feature type without being tied to specific implementations.

## TASK No.2: Migrate DateTime injection to use the new framework

This task demonstrates the framework in action by migrating the existing DateTime injection pattern to use the new infrastructure while maintaining backward compatibility. It involves replacing the existing `dateTimeImpl` global variable and `GetDateTimeImpl()` function with a `FeatureInjection<IDateTimeImpl>` instance initialized with the default OS implementation.

### what to be done

- Update `IDateTimeImpl` to inherit from `IFeatureImpl` in addition to `Interface` in `DateTime.h`
- Add implementation of `GetPreviousImpl()`, `BeginInjection()`, and `EndInjection()` methods to concrete DateTime implementations
- Remove the existing `dateTimeImpl` global variable from `DateTime.cpp`
- Remove the existing `GetDateTimeImpl()` function from `DateTime.cpp`
- Add a global `FeatureInjection<IDateTimeImpl>` instance in `DateTime.cpp`, initialized with `GetOSDateTimeImpl()` in its constructor
- Update existing `InjectDateTimeImpl()` function to delegate to the new feature injection framework using the global instance
- Update all DateTime methods to use the new framework's `Get()` method instead of calling the old `GetDateTimeImpl()`
- Maintain complete backward compatibility with existing client code
- Update implementation to properly chain to previous implementations when appropriate

### how to test it

- Run existing DateTime unit tests to ensure no regressions
- Test that existing injection code continues to work unchanged
- Verify new injection capabilities work correctly
- Test edge cases with multiple DateTime injections
- Ensure proper cleanup and resource management

### rationale

This task validates that the new framework can successfully replace existing injection patterns without breaking changes. By removing the old `dateTimeImpl` variable and `GetDateTimeImpl()` function and replacing them with a single `FeatureInjection<IDateTimeImpl>` instance, we simplify the code while gaining enhanced capabilities. The constructor-based initialization with `GetOSDateTimeImpl()` ensures that the framework always has a valid default implementation, eliminating the need for null checks. DateTime injection is already well-tested and understood, making it an ideal candidate for demonstrating the framework's capabilities. This migration ensures that the framework design is practical and can handle real-world injection scenarios while maintaining backward compatibility.

# !!!FINISHED!!!