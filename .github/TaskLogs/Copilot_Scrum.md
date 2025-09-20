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

# TASKS

- [ ] TASK No.1: Design and implement the IFeatureImpl base interface
- [ ] TASK No.2: Implement the FeatureInjection template helper class
- [ ] TASK No.3: Create specialized feature injection helpers for specific features
- [ ] TASK No.4: Migrate DateTime injection to use the new framework
- [ ] TASK No.5: Update documentation and examples

## TASK No.1: Design and implement the IFeatureImpl base interface

This task creates the foundational interface that all feature injection implementations must inherit from. The interface provides the contract for managing injection lifecycles and maintaining the linked list structure.

### what to be done

- Add `IFeatureImpl` interface declaration to `FeatureInjection.h`
- Define the three pure virtual methods: `GetPreviousImpl()`, `BeginInjection(IFeatureImpl* previousImpl)`, and `EndInjection()`
- Ensure the interface inherits from `virtual Interface` following Vlpp patterns
- Add comprehensive documentation explaining the injection lifecycle and linked list behavior
- Include namespace structure consistent with existing Vlpp code organization

### how to test it

- Compilation verification to ensure interface syntax is correct
- Verify interface can be inherited by concrete implementation classes
- Check that the interface follows proper virtual inheritance patterns
- Ensure no circular dependencies are introduced

### rationale

This interface is the foundation for the entire feature injection framework. It must be designed first because all subsequent components depend on it. The linked list design allows multiple injections to form a chain where each implementation can delegate to the previous one, enabling sophisticated composition patterns. The lifecycle methods `BeginInjection` and `EndInjection` provide hooks for implementations to manage resources and state transitions properly. This design follows the established pattern in Vlpp where interfaces inherit from `Interface` and use virtual inheritance to avoid diamond inheritance problems.

## TASK No.2: Implement the FeatureInjection template helper class

This task implements the core template class that manages the injection chain without memory allocation, providing type-safe access to the currently active implementation.

### what to be done

- Implement `FeatureInjection<TImpl>` template class in `FeatureInjection.h`
- Add private member to track the current implementation pointer (TImpl* current)
- Implement `Get()` method that returns the current implementation or nullptr
- Implement `Inject(TImpl* impl)` method that calls `impl->BeginInjection(current)` and updates current
- Implement `Eject(TImpl* impl)` method that finds impl in the chain and calls `EndInjection()` on all implementations from current down to impl
- Ensure no dynamic memory allocation is used in the implementation
- Add proper error handling for invalid eject operations
- Include comprehensive documentation with usage examples

### how to test it

- Test basic injection and ejection functionality with mock implementations
- Verify linked list traversal works correctly during ejection
- Test edge cases like ejecting from empty chain or non-existent implementations
- Verify no memory leaks occur during injection/ejection cycles
- Test multiple injection and ejection scenarios to ensure chain integrity

### rationale

This template class is the core of the feature injection framework. The template design ensures type safety while allowing the same pattern to work with any feature interface that inherits from `IFeatureImpl`. The linked list management through `GetPreviousImpl()` enables complex injection scenarios where multiple implementations can be stacked. The no-allocation constraint is critical for Vlpp's design philosophy of minimal runtime overhead. The ability to eject from the middle of the chain (not just the top) provides flexibility for complex injection scenarios where implementations may need to be removed in arbitrary order.

## TASK No.3: Create specialized feature injection helpers for specific features

This task provides concrete instantiations and helper functions for specific feature types, making the injection framework easy to use for common scenarios.

### what to be done

- Create template specialization or typedef for `DateTimeFeatureInjection` as `FeatureInjection<IDateTimeImpl>`
- Add global instance `dateTimeFeatureInjection` in `FeatureInjection.cpp`
- Implement convenience functions `InjectDateTimeFeature(IDateTimeImpl* impl)` and `EjectDateTimeFeature(IDateTimeImpl* impl)`
- Create helper function `GetCurrentDateTimeImpl()` that calls `dateTimeFeatureInjection.Get()`
- Ensure thread safety considerations are documented (following existing Vlpp patterns)
- Add proper initialization and cleanup patterns

### how to test it

- Test that the specialized helpers work correctly with DateTime operations
- Verify integration with existing DateTime injection infrastructure
- Test multiple concurrent injections and ejections
- Verify proper cleanup when multiple implementations are injected

### rationale

While the generic template provides the core functionality, specialized helpers make the framework more convenient to use. This follows the Vlpp pattern of providing both generic interfaces and convenient specializations. The global instance approach eliminates the need for users to manage FeatureInjection instances while maintaining the no-allocation constraint. This task demonstrates how the framework can be applied to real features and provides a template for extending to other features in the future.

## TASK No.4: Migrate DateTime injection to use the new framework

This task demonstrates the framework in action by migrating the existing DateTime injection pattern to use the new infrastructure while maintaining backward compatibility.

### what to be done

- Update `IDateTimeImpl` to inherit from `IFeatureImpl` in addition to `Interface`
- Add implementation of `GetPreviousImpl()`, `BeginInjection()`, and `EndInjection()` methods to concrete DateTime implementations
- Update existing `InjectDateTimeImpl()` function to delegate to the new feature injection framework
- Ensure `GetDateTimeImpl()` continues to work with the new system
- Maintain complete backward compatibility with existing client code
- Update implementation to properly chain to previous implementations when appropriate

### how to test it

- Run existing DateTime unit tests to ensure no regressions
- Test that existing injection code continues to work unchanged
- Verify new injection capabilities work correctly
- Test edge cases with multiple DateTime injections
- Ensure proper cleanup and resource management

### rationale

This task validates that the new framework can successfully replace existing injection patterns without breaking changes. DateTime injection is already well-tested and understood, making it an ideal candidate for demonstrating the framework's capabilities. The migration also ensures that the framework design is practical and can handle real-world injection scenarios. Maintaining backward compatibility is crucial to avoid disrupting existing code that depends on the current DateTime injection API.

## TASK No.5: Update documentation and examples

This task ensures the new framework is properly documented and provides clear guidance for developers who want to use or extend the injection system.

### what to be done

- Update relevant knowledge base documents to include feature injection framework
- Add code examples showing how to implement custom feature injections
- Document best practices for injection lifecycles and resource management
- Create examples showing advanced scenarios like injection chaining
- Update API documentation with comprehensive usage patterns
- Add troubleshooting guide for common injection issues

### how to test it

- Review documentation for completeness and accuracy
- Verify code examples compile and run correctly
- Test that documentation provides clear guidance for new users
- Ensure examples cover both simple and complex injection scenarios

### rationale

Proper documentation is essential for framework adoption and correct usage. The injection pattern introduces complexity that must be clearly explained to prevent misuse. Examples are particularly important because injection patterns can be subtle and error-prone. This task ensures that the framework can be effectively used by other developers and extended to new features in the future. Good documentation also serves as a design validation tool, helping identify any usability issues with the framework.

# Impact to the Knowledge Base

## Vlpp

The feature injection framework represents a significant new capability that needs to be properly documented in the knowledge base.

### New Content to Add

- **Feature Injection Framework**: Add new section under "Choosing APIs" for dependency injection capabilities
  - Describe `IFeatureImpl` interface and injection lifecycle
  - Document `FeatureInjection<T>` template and its usage patterns
  - Provide guidelines for when to use feature injection vs other patterns
  - Include examples of implementing custom injectable features

- **Design Explanation**: Add new topic explaining the feature injection architecture
  - Explain the linked list design and why it's beneficial
  - Document the no-allocation constraint and its implications
  - Describe integration with existing DateTime injection pattern
  - Provide rationale for template-based design

### Updates to Existing Content

- **DateTime Operations**: Update existing documentation to mention new injection framework
  - Add reference to enhanced injection capabilities
  - Update examples to show both old and new injection patterns
  - Clarify backward compatibility guarantees

- **Object Model and Memory Management**: Add mention of injection pattern as advanced technique
  - Reference feature injection as alternative to direct implementation replacement
  - Document how injection interacts with Interface and Object lifecycle

The knowledge base changes ensure that developers understand when and how to use the new injection framework, while maintaining documentation of existing patterns for backward compatibility.

# !!!FINISHED!!!