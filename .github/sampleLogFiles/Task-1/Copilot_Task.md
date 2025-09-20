# !!!TASK!!!

# PROBLEM DESCRIPTION

## TASK No.1: Implement complete feature injection framework

This task implements the entire feature injection framework including the base interface and template helper class. The framework provides a generic way to create injection patterns for any feature while maintaining backward compatibility with existing code.

### what to be done

- Add `IFeatureImpl` interface declaration to `FeatureInjection.h` with the three pure virtual methods: `GetPreviousImpl()`, `BeginInjection(IFeatureImpl* previousImpl)`, and `EndInjection()`
- Implement `FeatureImpl<TImpl>` template class providing default implementation for `IFeatureImpl`:
  - Store previous implementation as `TImpl*` type
  - Implement `BeginInjection(IFeatureImpl*)` with dynamic_cast validation and CHECK_ERROR for type mismatches
  - Add virtual `BeginInjection(TImpl*)` method with empty default implementation
  - Use `_previousImpl` parameter naming to avoid conflicts
  - Provide minimal implementation for all virtual methods
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
- Test `FeatureImpl<TImpl>` default implementation:
  - Verify dynamic_cast validation works correctly in `BeginInjection(IFeatureImpl*)`
  - Test CHECK_ERROR is triggered for type mismatches
  - Verify type-safe `BeginInjection(TImpl*)` method can be overridden
  - Test lifecycle methods work correctly (GetPreviousImpl, EndInjection)
- Test basic injection and ejection functionality with mock implementations
- Verify linked list traversal works correctly during ejection
- Test edge cases like ejecting from empty chain or non-existent implementations
- Verify no memory leaks occur during injection/ejection cycles
- Test multiple injection and ejection scenarios to ensure chain integrity

### rationale

This task focuses purely on the generic feature injection framework without any specific feature implementations. The interface design and template implementation are tightly coupled and should be implemented together as a complete unit. The framework provides the foundation for all future feature injections while maintaining the no-allocation constraint critical for Vlpp's design philosophy. The linked list design allows multiple injections to form chains where implementations can delegate to previous ones, enabling sophisticated composition patterns. By keeping this task generic, it ensures the framework can be applied to any feature type without being tied to specific implementations.

# INSIGHTS AND REASONING

## UPDATE

I would like to propose a default implementation for IFeatureImpl, which is FeatureImpl<TImpl>. TImpl is supposed to be an actual feature interface inheriting from IFeatureImpl. It does the following thing:

- Implement all virtual functions in the minimum way, which means GetPreviousImpl returns the argument from BeginInjectin and return null after EndInjection.
- Add a new virtual method but with an empty implementation, aka BeginInjection(TImpl* previousImpl).
- The original BeginInjection will check if previousImpl is TImpl via dynamic_cast. If it is nullptr, everything is fine. If it is not null but dynamic_cast fails, CHECK_ERROR.
- The internal stored previousImpl will be in type TImpl*.
- Given previousImpl conflict with the argument name, FeatureImpl<Impl> will use _previousImpl in function arguments, while keep IFeatureImpl namings unchanged.

## Analysis of Current DateTime Injection Pattern

I have analyzed the existing DateTime injection implementation in `Source/Primitives/DateTime.cpp` and `DateTime.h`. The current pattern includes:

1. **Global state management**: A global `IDateTimeImpl* dateTimeImpl` variable initialized to `nullptr`
2. **Fallback mechanism**: `GetDateTimeImpl()` function that returns `dateTimeImpl` if not null, otherwise returns `GetOSDateTimeImpl()`  
3. **Simple injection**: `InjectDateTimeImpl(IDateTimeImpl* impl)` function that directly assigns `impl` to the global variable
4. **Interface design**: `IDateTimeImpl` inherits from `Interface` and provides date/time operation methods

This pattern has several limitations:
- **Single injection**: Only supports one injected implementation at a time, no chaining
- **No lifecycle management**: No notification when implementations are replaced
- **Global coupling**: Direct global variable access without abstraction
- **No restoration mechanism**: No way to "eject" implementations and restore previous ones

## Proposed Feature Injection Framework Design

### Core Interface: IFeatureImpl

The foundation will be a base interface that all feature injection interfaces must inherit from:

```cpp
namespace vl
{
    namespace feature_injection
    {
        class IFeatureImpl : public virtual Interface
        {
        public:
            virtual IFeatureImpl*   GetPreviousImpl() = 0;
            virtual void           BeginInjection(IFeatureImpl* previousImpl) = 0;
            virtual void           EndInjection() = 0;
        };
    }
}
```

**Design rationale:**
- **Linked list support**: `GetPreviousImpl()` enables implementations to maintain references to previous implementations in the chain
- **Lifecycle management**: `BeginInjection()` and `EndInjection()` provide explicit notification when implementations are installed or removed
- **Delegation capability**: Implementations can delegate to previous implementations via `GetPreviousImpl()`
- **Virtual inheritance**: Follows Vlpp pattern for interface inheritance to avoid diamond problem

### Default Implementation Template: FeatureImpl<TImpl>

To simplify implementation of feature interfaces, a default base template will be provided:

```cpp
namespace vl
{
    namespace feature_injection
    {
        template<typename TImpl>
        class FeatureImpl : public virtual IFeatureImpl
        {
        private:
            TImpl* _previousImpl;

        public:
            FeatureImpl();

            // IFeatureImpl implementation
            IFeatureImpl*       GetPreviousImpl() override;
            void               BeginInjection(IFeatureImpl* _previousImpl) override;
            void               EndInjection() override;

            // Type-safe previous implementation access
            virtual void       BeginInjection(TImpl* _previousImpl);
        };
    }
}
```

**Design rationale:**
1. **Type-safe storage**: Internal `_previousImpl` is stored as `TImpl*` for type safety
2. **Dynamic cast validation**: The base `BeginInjection(IFeatureImpl*)` performs dynamic_cast to validate type compatibility
3. **Error checking**: Uses `CHECK_ERROR` if dynamic_cast fails when previousImpl is not null
4. **Virtual hook**: `BeginInjection(TImpl*)` provides a type-safe override point with empty default implementation
5. **Minimal implementation**: Provides the simplest possible implementation for all required virtual methods
6. **Parameter naming**: Uses `_previousImpl` in function parameters to avoid conflict with member variable name

**Implementation behavior:**
- **Constructor**: Initializes `_previousImpl` to `nullptr`
- **GetPreviousImpl()**: Returns `_previousImpl` (which becomes `nullptr` after `EndInjection()`)
- **BeginInjection(IFeatureImpl*)**: Validates type via dynamic_cast, then calls type-safe overload
- **BeginInjection(TImpl*)**: Virtual method with empty implementation for derived classes to override
- **EndInjection()**: Sets `_previousImpl` to `nullptr`

### Template Helper: FeatureInjection<TImpl>

The core management class will be a template that provides type-safe injection operations:

```cpp
namespace vl
{
    namespace feature_injection
    {
        template<typename TImpl>
        class FeatureInjection
        {
        private:
            TImpl* currentImpl;

        public:
            FeatureInjection(TImpl* defaultImpl);
            
            TImpl*              Get();
            void               Inject(TImpl* impl);
            void               Eject(TImpl* impl);
        };
    }
}
```

**Design decisions:**
1. **Constructor initialization**: Requires a default implementation to ensure `Get()` never returns nullptr
2. **Single member variable**: Only `currentImpl` pointer needed - no dynamic allocation
3. **Type safety**: Template ensures compile-time type checking for specific feature interfaces
4. **Consistent API**: Simple three-method interface for all injection operations

### Injection Algorithm

**Inject(TImpl* impl) workflow:**
1. Call `impl->BeginInjection(currentImpl)` to notify the new implementation about the previous one
2. Set `currentImpl = impl` to make the new implementation active
3. The new implementation can store the previous implementation reference for delegation

**Eject(TImpl* impl) workflow:**
1. Traverse the linked list starting from `currentImpl` following `GetPreviousImpl()` until finding `impl`
2. Call `EndInjection()` on each implementation from `currentImpl` down to `impl` in reverse order
3. Set `currentImpl` to the implementation that was previous to `impl`
4. If `impl` is not found in the chain, the operation is a no-op (fail silently)

### Error Handling Strategy

**No exceptions approach**: Following Vlpp's philosophy of minimal dependencies and avoiding exceptions in core infrastructure:
- **Invalid eject operations**: Eject of non-existent implementations will be silent no-ops
- **Null pointer protection**: Constructor requires non-null default implementation
- **Chain integrity**: Implementations are responsible for maintaining valid `GetPreviousImpl()` chains

### Memory Management

**No dynamic allocation**: The framework uses only a single pointer member variable per `FeatureInjection<T>` instance. All memory management is delegated to the caller:
- **Implementation lifetime**: Callers are responsible for keeping injected implementations alive
- **Chain management**: Each implementation stores its previous implementation reference
- **No ownership transfer**: The framework doesn't take ownership of implementations

### Integration with Existing Patterns

**Backward compatibility preservation**: The new framework will maintain compatibility with existing injection patterns:
- **Same interface footprint**: Existing `InjectDateTimeImpl()` function signature preserved
- **Behavioral compatibility**: `Get()` method replaces `GetDateTimeImpl()` functionality
- **Default implementation**: Constructor-based initialization replaces global variable + fallback pattern

## Implementation Strategy

### Phase 1: Framework Foundation
1. Add `IFeatureImpl` interface to `FeatureInjection.h`
2. Implement `FeatureImpl<TImpl>` template class with type-safe default implementation
3. Implement `FeatureInjection<TImpl>` template class for injection management
4. Add comprehensive inline documentation

### Phase 2: Algorithm Implementation
1. Implement `FeatureImpl<TImpl>` methods:
   - Constructor initialization
   - `GetPreviousImpl()` simple return
   - `BeginInjection(IFeatureImpl*)` with dynamic_cast validation and CHECK_ERROR
   - `BeginInjection(TImpl*)` virtual method with empty implementation
   - `EndInjection()` that nullifies previous implementation
2. Implement `FeatureInjection<TImpl>` methods:
   - `Get()` method (simple return of current implementation)
   - `Inject()` method with lifecycle notification
   - `Eject()` method with chain traversal and cleanup

### Phase 3: Integration Points
1. Ensure namespace organization follows Vlpp conventions
2. Add proper header guards and includes
3. Verify template instantiation works correctly
4. Test dynamic_cast validation in `FeatureImpl<TImpl>`

## Quality Considerations

**Thread safety**: The framework is designed for single-threaded injection scenarios. Multi-threaded access to the same `FeatureInjection<T>` instance requires external synchronization.

**Performance**: All operations are O(1) except `Eject()` which is O(n) where n is the chain length. This is acceptable since ejection is expected to be rare.

**Debugging support**: The linked list structure makes it easy to inspect the injection chain during debugging.

**Testing approach**: The framework can be thoroughly tested with mock implementations without requiring platform-specific features.

# !!!FINISHED!!!
