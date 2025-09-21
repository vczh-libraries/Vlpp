# !!!KNOWLEDGE BASE!!!

# DOCUMENT REQUEST

## TOPIC

I would like you to create new knowledge base topic about IFeatureImpl, FeatureImpl<TImpl> and FeatureInjection<TImpl>.

Currently DateTime/InjectDateTimeImpl/EjectDateTimeImpl is the first thing that adopted this new feature. Please read through the implementation around DateTime, alsk take a look at DateTime and FeatureInjection unit test, make a knowledge base about it and cover the following thing:

- Basic introduction to FeatureInjection
- The standard way to implement an injectable feature
- How can an injected implementation delegates part of it to the previous implementation

## DRAFT

Excellent! I would like you to draft the document. The title could be "Implementing an Injectable Feature". Explain the usage of IFeatureImpl/FeatureImpl/FeatureInjection, and bring the demo you just created. I believe this demo is a quick learning guidance.

### Insight

**FeatureInjection System Architecture**

The FeatureInjection system provides a linked-list based dependency injection mechanism that allows runtime replacement and extension of feature implementations while maintaining delegation capabilities. The architecture consists of three core components:

**Core Components:**

1. **IFeatureImpl**: Base interface for all injectable implementations
   - `GetPreviousImpl()`: Returns the previous implementation in the chain
   - `BeginInjection(IFeatureImpl* previousImpl)`: Lifecycle hook called when implementation becomes active
   - `EndInjection()`: Lifecycle hook called when implementation is being removed

2. **FeatureImpl<TImpl>**: Template base class providing type-safe delegation
   - Inherits from `Object` and virtually from `TImpl`
   - Stores typed `_previousImpl` pointer for delegation
   - Provides `Previous()` method for type-safe access to previous implementation
   - Handles type casting and validation in `BeginInjection`
   - Virtual `BeginInjection(TImpl* previousImpl)` for derived classes to override

3. **FeatureInjection<TImpl>**: Manager class for injection chain
   - Maintains `currentImpl` pointer to active implementation
   - `Inject(TImpl* impl)`: Adds new implementation to top of chain
   - `Eject(TImpl* impl)`: Removes implementation and all subsequent ones
   - `EjectAll()`: Removes all injected implementations, restoring default
   - `Get()`: Returns current active implementation

**DateTime Implementation Pattern:**

The DateTime system demonstrates the standard implementation pattern:

1. **Interface Definition**: `IDateTimeImpl` extends `IFeatureImpl` with DateTime-specific methods:
   - `FromDateTime`, `FromOSInternal`, `LocalTime`, `UtcTime`
   - `LocalToUtcTime`, `UtcToLocalTime`, `Forward`, `Backward`

2. **Default Implementation**: `WindowsDateTimeImpl` and `LinuxDateTimeImpl` inherit from `FeatureImpl<IDateTimeImpl>`
   - Provides OS-specific behavior using Windows/Linux APIs
   - Serves as the base implementation in the injection chain

3. **Global Injection Management**:
   - `GetDateTimeInjection()` returns static `FeatureInjection<IDateTimeImpl>` instance
   - `InjectDateTimeImpl(IDateTimeImpl* impl)` delegates to injection manager
   - `EjectDateTimeImpl(IDateTimeImpl* impl)` with nullptr support for `EjectAll()`

4. **Feature Usage**: `DateTime` static methods call `GetDateTimeInjection().Get()->methodName()`
   - All DateTime operations go through the injection system
   - Transparent to end users whether using default or injected implementation

**Delegation Mechanism:**

Injected implementations can delegate to previous implementations through the `Previous()` method:

1. **Partial Override**: MockDateTimeImpl in tests overrides only `LocalTime()` and `UtcTime()` to return fixed values
2. **Full Delegation**: Other methods like `FromDateTime()` call `Previous()->FromDateTime()` unchanged
3. **Chain Traversal**: Each implementation can modify behavior and pass control down the chain
4. **Type Safety**: `FeatureImpl<TImpl>` ensures Previous() returns correctly typed pointer

**Injection Chain Behavior:**

1. **LIFO Structure**: Last injected becomes current, previous current becomes its predecessor
2. **Cascading Ejection**: Ejecting from middle removes all subsequent implementations
3. **Lifecycle Management**: `BeginInjection`/`EndInjection` called automatically during inject/eject
4. **Error Handling**: Null checks and chain validation with descriptive error messages

**Usage Examples from Tests:**

1. **Basic Chain**: Default -> Impl1 -> Impl2 produces "Default-Impl1-Impl2" feature combination
2. **Middle Ejection**: Removing Impl1 from Default->Impl1->Impl2->Impl3 results in Default only
3. **State Verification**: Call counts on BeginInjection/EndInjection verify proper lifecycle management
4. **DateTime Mocking**: Tests inject MockDateTimeImpl to return fixed times for deterministic testing

## ASK

There is a reason that GetDateTimeInjection and GetOSDateTimeImpl all returning a static local variable. What do you think it is a reason? Is that part of the pattern?

### Insight

**Static Local Variables Pattern in FeatureInjection**

Yes, the use of static local variables in both `GetDateTimeInjection()` and `GetOSDateTimeImpl()` is a fundamental part of the FeatureInjection pattern, serving multiple critical purposes:

**Singleton Pattern Implementation:**

1. **Guaranteed Single Instance**: Static local variables ensure exactly one instance of the injection manager and default implementation exist throughout the application lifetime
2. **Thread-Safe Initialization**: In C++11 and later, static local variable initialization is guaranteed to be thread-safe, preventing race conditions during first access
3. **Lazy Initialization**: The instances are created only when first accessed, not at program startup

**Global State Management:**

1. **Application-Wide Consistency**: All DateTime operations across the entire application use the same injection chain, ensuring consistent behavior
2. **Cross-Module Sharing**: Static local variables in functions provide controlled global access without polluting the global namespace
3. **Initialization Order Independence**: Unlike global variables, static locals don't suffer from static initialization order fiasco

**Memory Management Benefits:**

1. **Automatic Lifetime**: Static locals are destroyed at program termination, no manual cleanup required
2. **No Heap Allocation**: Objects live in static storage, avoiding dynamic allocation overhead
3. **Guaranteed Availability**: The injection manager and default implementation are always available when needed

**Pattern Evidence from Code:**

1. **GetDateTimeInjection()** in `DateTime.cpp`:
   - Returns `static feature_injection::FeatureInjection<IDateTimeImpl> injection(GetOSDateTimeImpl())`
   - Ensures single injection manager instance per application
   - Initialized with default OS implementation

2. **GetOSDateTimeImpl()** (declared in `DateTime.cpp`, implemented in OS-specific files):
   - Returns `static WindowsDateTimeImpl impl` or `static LinuxDateTimeImpl impl`
   - Provides platform-specific default implementation
   - Guaranteed to exist for injection manager initialization

**Design Pattern Integration:**

This static local pattern is essential for the FeatureInjection system because:

1. **Global Access Point**: Provides controlled access to the injection chain without exposing implementation details
2. **Encapsulation**: Hides the injection manager instance behind function calls
3. **Testability**: Allows tests to inject mock implementations globally while maintaining proper cleanup
4. **Platform Abstraction**: OS-specific implementations are hidden behind the same function signature

The pattern ensures that feature injection works consistently across the entire application while maintaining proper initialization order and thread safety.

## ASK

Assuming we are going to make a new injectable feature. For example, IMyFeature with two methods DoThis and DoThat. What do you think is the best way to finish that and expose necessary functions as API?

- A default implementation, for demo, DoThis and DoThat is empty except comments.
- InjectMyFeatureImpl and EjectMyFeatureImpl, and there implementation
- A demo program to inject another implementaiton, while DoThis is empty with a comment, but DoThat delegates to the previous implementation.

### Insight

**Standard Pattern for Creating New Injectable Features**

Based on the DateTime implementation pattern, here's the recommended approach for creating `IMyFeature` with `DoThis` and `DoThat` methods:

**Step 1: Interface Definition**

Define `IMyFeature` interface in a header file (e.g., `MyFeature.h`):
- Inherit from `vl::feature_injection::IFeatureImpl`
- Declare pure virtual methods `DoThis()` and `DoThat()`
- Include necessary parameter types and return values

**Step 2: Default Implementation**

Create default implementation class:
- Inherit from `vl::feature_injection::FeatureImpl<IMyFeature>`
- Implement `DoThis()` and `DoThat()` with empty bodies and comments
- Place in same header or separate implementation file
- Follow naming pattern like `DefaultMyFeatureImpl`

**Step 3: Global Management Functions**

Following DateTime pattern structure:
- Create `GetMyFeatureInjection()` function returning static `feature_injection::FeatureInjection<IMyFeature>&`
- Initialize with static default implementation instance
- Implement `InjectMyFeatureImpl(IMyFeature* impl)` calling injection manager's `Inject()`
- Implement `EjectMyFeatureImpl(IMyFeature* impl)` with nullptr support for `EjectAll()`

**Step 4: Global Access Functions**

Provide convenient access functions:
- Create wrapper functions that call `GetMyFeatureInjection().Get()->DoThis()` and `GetMyFeatureInjection().Get()->DoThat()`
- These can be static methods in a class or standalone functions
- Follow the pattern used in DateTime static methods

**Step 5: Demo Implementation**

Create demonstration implementation:
- Inherit from `vl::feature_injection::FeatureImpl<IMyFeature>`
- Implement `DoThis()` with empty body and comment explaining demo purpose
- Implement `DoThat()` calling `Previous()->DoThat()` to demonstrate delegation
- Show proper use of `Previous()` method for type-safe delegation

**Key Implementation Details:**

1. **Static Local Variables**: Use static locals in `GetMyFeatureInjection()` and default implementation getter for singleton pattern and thread safety
2. **Type Safety**: Leverage `FeatureImpl<IMyFeature>` template for automatic type casting and validation
3. **Lifecycle Management**: Override `BeginInjection(IMyFeature* previous)` if custom initialization needed
4. **Error Handling**: Include null pointer checks and descriptive error messages following DateTime pattern
5. **Delegation Pattern**: Use `Previous()` method to access previous implementation in chain safely

**File Organization:**

Following DateTime structure:
- Main interface and default implementation in `MyFeature.h`
- Implementation logic in `MyFeature.cpp`
- Platform-specific code (if needed) in `MyFeature.Windows.cpp` and `MyFeature.Linux.cpp`
- Demo code in test files or separate demo application

**Example Integration:**

The demo program would:
- Create instance of demo implementation class
- Call `InjectMyFeatureImpl(&demoImpl)`
- Test both `DoThis()` (overridden) and `DoThat()` (delegated) behaviors
- Call `EjectMyFeatureImpl(nullptr)` for cleanup
- Verify behavior returns to default implementation

This pattern ensures consistency with existing FeatureInjection usage while providing a complete, testable injectable feature system.

## ASK

Looks good, please show me the complete content of MyFeature.h and MyFeature.cpp. but you can omit includes and namespaces, that's are not important as a demo, and you can also omit `vl::feature_injection`.

### Insight

**Complete MyFeature Implementation Example**

**MyFeature.h:**

```cpp
class IMyFeatureImpl : public virtual IFeatureImpl
{
public:
	virtual void DoThis() = 0;
	virtual void DoThat() = 0;
};

class DefaultMyFeatureImpl : public FeatureImpl<IMyFeatureImpl>
{
public:
	void DoThis() override
	{
		// Default implementation - empty for demo purposes
	}
	
	void DoThat() override
	{
		// Default implementation - empty for demo purposes
	}
};

extern void InjectMyFeatureImpl(IMyFeatureImpl* impl);
extern void EjectMyFeatureImpl(IMyFeatureImpl* impl);
```

**MyFeature.cpp:**

```cpp
IMyFeatureImpl* GetDefaultMyFeatureImpl()
{
	static DefaultMyFeatureImpl impl;
	return &impl;
}

FeatureInjection<IMyFeatureImpl>& GetMyFeatureInjection()
{
	static FeatureInjection<IMyFeatureImpl> injection(GetDefaultMyFeatureImpl());
	return injection;
}

void InjectMyFeatureImpl(IMyFeatureImpl* impl)
{
	GetMyFeatureInjection().Inject(impl);
}

void EjectMyFeatureImpl(IMyFeatureImpl* impl)
{
	if (impl == nullptr)
	{
		GetMyFeatureInjection().EjectAll();
	}
	else
	{
		GetMyFeatureInjection().Eject(impl);
	}
}
```

**Demo Implementation Class:**

```cpp
class DemoMyFeatureImpl : public FeatureImpl<IMyFeatureImpl>
{
public:
	void DoThis() override
	{
		// Demo override - empty with comment explaining purpose
	}
	
	void DoThat() override
	{
		// Demo delegation - calls previous implementation
		Previous()->DoThat();
	}
};
```

**Demo Usage Program:**

```cpp
void DemoMyFeature()
{
	DemoMyFeatureImpl demoImpl;
	
	// Inject demo implementation
	InjectMyFeatureImpl(&demoImpl);
	
	// Test both methods - DoThis is overridden, DoThat delegates
	GetMyFeatureInjection().Get()->DoThis();  // Uses demo implementation
	GetMyFeatureInjection().Get()->DoThat();  // Delegates to default through Previous()
	
	// Always cleanup
	EjectMyFeatureImpl(nullptr);
}
```

This example follows the exact pattern established by DateTime implementation, showing the complete structure needed for a new injectable feature with both override and delegation patterns.

# DRAFT-LOCATION

This document will be added as a new topic under the **Design Explanation** section of the **Vlpp** project in `Index.md` of the knowledge base.

The topic will provide comprehensive guidance on implementing injectable features using the FeatureInjection system, covering the architecture, implementation patterns, and practical examples.

# DRAFT-TITLE

Implementing an Injectable Feature

# DRAFT-CONTENT

## Overview

The FeatureInjection system provides a linked-list based dependency injection mechanism that allows runtime replacement and extension of feature implementations while maintaining delegation capabilities. This system enables testing with mock implementations, feature enhancement through layered behaviors, and platform-specific customizations without modifying core application code.

## Core Components Architecture

### IFeatureImpl Base Interface

The `IFeatureImpl` interface serves as the foundation for all injectable implementations:

- `GetPreviousImpl()`: Returns the previous implementation in the injection chain
- `BeginInjection(IFeatureImpl* previousImpl)`: Lifecycle hook called when implementation becomes active in the chain
- `EndInjection()`: Lifecycle hook called when implementation is being removed from the chain

**IMPORTANT**: The `EndInjection` lifecycle method has limited guarantees - it will only be invoked during explicit `Eject` and `EjectAll` operations. During application shutdown and object destruction, `EndInjection` calls cannot be guaranteed due to unpredictable destruction order.

### FeatureImpl<TImpl> Template Base Class

The `FeatureImpl<TImpl>` template provides type-safe delegation and chain management:

- Inherits from `Object` and virtually from `TImpl` interface
- Maintains typed `_previousImpl` pointer for safe delegation access
- Provides `Previous()` method returning correctly typed pointer to previous implementation
- Handles automatic type casting and validation in `BeginInjection`
- Offers virtual `BeginInjection(TImpl* previousImpl)` for derived classes to override

### FeatureInjection<TImpl> Manager Class

The `FeatureInjection<TImpl>` class manages the injection chain:

- Maintains `currentImpl` pointer to the active implementation
- `Inject(TImpl* impl)`: Adds new implementation to top of chain (LIFO structure)
- `Eject(TImpl* impl)`: Removes implementation and all subsequent ones from chain
- `EjectAll()`: Removes all injected implementations, restoring default behavior
- `Get()`: Returns current active implementation for feature usage

## Standard Implementation Pattern

### Interface Definition

Define the feature interface inheriting from `IFeatureImpl`:

```cpp
class IMyFeatureImpl : public virtual IFeatureImpl
{
public:
	virtual void DoThis() = 0;
	virtual void DoThat() = 0;
};
```

### Default Implementation

Create default implementation inheriting from `FeatureImpl<TImpl>`:

```cpp
class DefaultMyFeatureImpl : public FeatureImpl<IMyFeatureImpl>
{
public:
	void DoThis() override
	{
		// Default implementation - empty for demo purposes
	}
	
	void DoThat() override
	{
		// Default implementation - empty for demo purposes
	}
};
```

### Global Management Functions

Implement singleton pattern with static local variables for thread safety and initialization order independence:

```cpp
IMyFeatureImpl* GetDefaultMyFeatureImpl()
{
	static DefaultMyFeatureImpl impl;
	return &impl;
}

FeatureInjection<IMyFeatureImpl>& GetMyFeatureInjection()
{
	static FeatureInjection<IMyFeatureImpl> injection(GetDefaultMyFeatureImpl());
	return injection;
}

void InjectMyFeatureImpl(IMyFeatureImpl* impl)
{
	GetMyFeatureInjection().Inject(impl);
}

void EjectMyFeatureImpl(IMyFeatureImpl* impl)
{
	if (impl == nullptr)
	{
		GetMyFeatureInjection().EjectAll();
	}
	else
	{
		GetMyFeatureInjection().Eject(impl);
	}
}
```

## Delegation Mechanism

### Type-Safe Previous Access

Injected implementations can delegate to previous implementations through the `Previous()` method provided by `FeatureImpl<TImpl>`:

```cpp
class DemoMyFeatureImpl : public FeatureImpl<IMyFeatureImpl>
{
public:
	void DoThis() override
	{
		// Demo override - completely replace behavior
	}
	
	void DoThat() override
	{
		// Demo delegation - calls previous implementation
		Previous()->DoThat();
	}
};
```

### Chain Traversal Behavior

- **LIFO Structure**: Last injected implementation becomes current, previous current becomes its predecessor
- **Cascading Ejection**: Ejecting from middle of chain removes all subsequent implementations
- **Type Safety**: `FeatureImpl<TImpl>` ensures `Previous()` returns correctly typed pointer
- **Lifecycle Management**: `BeginInjection`/`EndInjection` called automatically during inject/eject operations

## Real-World Example: DateTime System

### Interface Implementation

The DateTime system demonstrates the complete pattern through `IDateTimeImpl`:

- Extends `IFeatureImpl` with DateTime-specific methods: `FromDateTime`, `FromOSInternal`, `LocalTime`, `UtcTime`, `LocalToUtcTime`, `UtcToLocalTime`, `Forward`, `Backward`
- Platform-specific implementations: `WindowsDateTimeImpl` and `LinuxDateTimeImpl` inherit from `FeatureImpl<IDateTimeImpl>`
- Global management through `GetDateTimeInjection()`, `InjectDateTimeImpl()`, and `EjectDateTimeImpl()`

### Testing Integration

The DateTime tests demonstrate practical usage:

- `MockDateTimeImpl` overrides only `LocalTime()` and `UtcTime()` to return fixed values for deterministic testing
- Other methods like `FromDateTime()` delegate through `Previous()->FromDateTime()` unchanged
- Tests verify proper lifecycle management through call counts on `BeginInjection`/`EndInjection`

## Static Local Variables Pattern

### Critical Design Choice

The use of static local variables in `GetMyFeatureInjection()` and `GetDefaultMyFeatureImpl()` serves multiple purposes:

- **Thread-Safe Singleton**: C++11+ guarantees thread-safe initialization of static locals
- **Global State Management**: Ensures application-wide consistency for feature behavior
- **Lazy Initialization**: Objects created only when first accessed, avoiding startup overhead
- **Initialization Order Independence**: Prevents static initialization order fiasco
- **Automatic Lifetime**: Objects destroyed at program termination without manual cleanup

### Memory and Performance Benefits

- **No Heap Allocation**: Objects live in static storage, avoiding dynamic allocation overhead
- **Guaranteed Availability**: Injection manager and default implementation always available when needed
- **Cross-Module Sharing**: Static locals in functions provide controlled global access without namespace pollution

## Usage Example

### Complete Demo Program

```cpp
void DemoMyFeature()
{
	DemoMyFeatureImpl demoImpl;
	
	// Inject demo implementation
	InjectMyFeatureImpl(&demoImpl);
	
	// Test both methods - DoThis is overridden, DoThat delegates
	GetMyFeatureInjection().Get()->DoThis();  // Uses demo implementation
	GetMyFeatureInjection().Get()->DoThat();  // Delegates to default through Previous()
	
	// Always cleanup to restore default behavior
	EjectMyFeatureImpl(nullptr);
}
```

### Key Usage Principles

- **Always Clean Up**: Use `EjectMyFeatureImpl(nullptr)` to restore default behavior
- **LIFO Injection**: Last injected becomes current implementation
- **Delegation Choice**: Each method can choose to override completely or delegate to previous
- **Error Handling**: Include null pointer checks and descriptive error messages
- **Testing Support**: Perfect for injecting mock implementations during unit tests
- **IMPORTANT**: Restrict injection and ejection calls to application-level code, frameworks, or unit tests only - never from within library code. This constraint exists because injection/ejection operations must maintain strict ordering to prevent crashes. For instance, calling `Inject(a)`, `Inject(b)`, `Eject(a)`, `Eject(b)` will cause `Eject(a)` to remove both implementations, making the subsequent `Eject(b)` call fail. Applications and frameworks maintain global knowledge of the injection state and can ensure proper cleanup ordering, while libraries cannot guarantee safe usage patterns.
