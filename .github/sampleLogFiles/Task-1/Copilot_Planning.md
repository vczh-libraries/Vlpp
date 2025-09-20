# !!!PLANNING!!!

# UPDATES

## UPDATE

Implement complete feature injection framework

This task implements the entire feature injection framework including the base interface and template helper class. The framework provides a generic way to create injection patterns for any feature while maintaining backward compatibility with existing code.

Key components to implement:
- Add `IFeatureImpl` interface declaration to `FeatureInjection.h` with the three pure virtual methods: `GetPreviousImpl()`, `BeginInjection(IFeatureImpl* previousImpl)`, and `EndInjection()`
- Implement `FeatureImpl<TImpl>` template class providing default implementation for `IFeatureImpl` with dynamic_cast validation and CHECK_ERROR for type mismatches
- Implement `FeatureInjection<TImpl>` template class in `FeatureInjection.h` with private member to track current implementation pointer
- Add constructor, `Get()`, `Inject()`, and `Eject()` methods with proper error handling and linked list behavior
- Ensure no dynamic memory allocation is used in the implementation

## UPDATE

- FeatureImpl<TImpl> should inherits from `public Object`
- no XML comment needed for overrided implementation in FeatureImpl, but the empty new BeginInjection needs
- CHECK_ERROR needs to show the full namespace for FeatureImpl function name

## UPDATE

For FeatureInjection

- CHECK_ERROR needs to show full namespace for the function name
- In `Eject`, `// If impl is not found in the chain, operation is a silent no-op`, I want you to CHECK_ERROR it instead, also when impl == nullptr.
  - the actual eject code is too verbose, you can just do it in a while(true) and break after reaching impl
- there is no need to do dynamic_cast for GetPreviousImpl. Instead please put a `require` for the class to make sure `TImpl` does inherit from IFeatureImpl.

## UPDATE

There is no need for a test plan as I will do in later

## UPDATE

Just realize why you keep using static_cast and dynamic_cast in FeatureInjection. Since the whole implementation do not need anything from TImpl extra content, so currentImpl and current/toEject in Eject should just use IFeatureImpl. But keep the argument type unchanged for type safety. Then no casting is needed.

And `impl->BeginInjection` should call the IFeatureImpl version, so here is where static_cast is needed.

## UPDATE

Eject looks wired. There is a `(toEject != impl)` which totally missed my point. I told you to replace this `while(toEject != impl)` loop to `while(true)` so that you don't have to call EndInjection twice in the function. The outer `while(true)` also does not make any sense

## UPDATE

In FeatureInjection, `static_assert` is not good because it offers a compile error without context. This is actually why `requires` for template is useful.

# IMPROVEMENT PLAN

## STEP 1: Implement IFeatureImpl Base Interface

**What to change:**
Add the base interface `IFeatureImpl` to `FeatureInjection.h` that all feature injection interfaces must inherit from.

```cpp
namespace vl
{
	namespace feature_injection
	{
		/// <summary>
		/// Base interface for all feature injection implementations.
		/// Provides lifecycle management and delegation support through linked list structure.
		/// </summary>
		class IFeatureImpl : public virtual Interface
		{
		public:
			/// <summary>Get the previous implementation in the injection chain.</summary>
			/// <returns>The previous implementation, or nullptr if this is the first in chain.</returns>
			virtual IFeatureImpl*		GetPreviousImpl() = 0;
			
			/// <summary>Begin injection lifecycle, called when this implementation becomes active.</summary>
			/// <param name="previousImpl">The previously active implementation to delegate to.</param>
			virtual void				BeginInjection(IFeatureImpl* previousImpl) = 0;
			
			/// <summary>End injection lifecycle, called when this implementation is being removed.</summary>
			virtual void				EndInjection() = 0;
		};
	}
}
```

**Why this change is necessary:**
This interface establishes the foundation for the injection framework by defining the contract that all injection implementations must follow. The lifecycle methods `BeginInjection()` and `EndInjection()` provide explicit notification when implementations are installed or removed, while `GetPreviousImpl()` enables implementations to form a linked list for delegation. This design allows multiple injections to be chained together where implementations can delegate to previous ones, enabling sophisticated composition patterns.

## STEP 2: Implement FeatureImpl<TImpl> Default Template Class

**What to change:**
Add the default implementation template `FeatureImpl<TImpl>` to provide type-safe base functionality for feature implementations.

```cpp
namespace vl
{
	namespace feature_injection
	{
		/// <summary>
		/// Default implementation template for IFeatureImpl providing type-safe delegation.
		/// TImpl should be the actual feature interface that inherits from IFeatureImpl.
		/// </summary>
		template<typename TImpl>
		requires std::is_base_of_v<IFeatureImpl, TImpl>
		class FeatureImpl : public Object, public virtual IFeatureImpl
		private:
			TImpl*				_previousImpl = nullptr;

		public:
			IFeatureImpl* GetPreviousImpl() override
			{
				return _previousImpl;
			}

			void BeginInjection(IFeatureImpl* _previousImpl) override
			{
				TImpl* typedPrevious = nullptr;
				if (_previousImpl != nullptr)
				{
					typedPrevious = dynamic_cast<TImpl*>(_previousImpl);
					CHECK_ERROR(typedPrevious != nullptr, L"vl::feature_injection::FeatureImpl<TImpl>::BeginInjection(IFeatureImpl*): Invalid previous implementation type.");
				}
				this->_previousImpl = typedPrevious;
				BeginInjection(typedPrevious);
			}

			void EndInjection() override
			{
				_previousImpl = nullptr;
			}

			/// <summary>Type-safe injection hook for derived classes to override.</summary>
			/// <param name="_previousImpl">The type-safe previous implementation.</param>
			virtual void BeginInjection(TImpl* _previousImpl)
			{
			}
		};
	}
}
```

**Why this change is necessary:**
This template class simplifies the implementation of feature interfaces by providing a default implementation that handles the complex parts automatically. The key benefits are: (1) Type safety - internal storage as `TImpl*` provides compile-time type checking, (2) Dynamic cast validation - the base `BeginInjection(IFeatureImpl*)` validates type compatibility using dynamic_cast and triggers CHECK_ERROR for type mismatches, (3) Virtual hook - the type-safe `BeginInjection(TImpl*)` provides an override point with empty default implementation, and (4) Minimal implementation - provides the simplest possible implementation for all required virtual methods. This reduces boilerplate code for implementers while ensuring type safety and proper lifecycle management.

## STEP 3: Implement FeatureInjection<TImpl> Management Template

**What to change:**
Add the core management template `FeatureInjection<TImpl>` that provides type-safe injection operations.

```cpp
namespace vl
{
	namespace feature_injection
	{
		/// <summary>
		/// Template class for managing feature injection with type-safe operations.
		/// Maintains a linked list of implementations and provides inject/eject functionality.
		/// </summary>
		template<typename TImpl>
		requires std::is_base_of_v<IFeatureImpl, TImpl>
		class FeatureInjection
		private:
			IFeatureImpl*		currentImpl;

		public:
			/// <summary>Initialize with a default implementation.</summary>
			/// <param name="defaultImpl">The default implementation, must not be nullptr.</param>
			FeatureInjection(TImpl* defaultImpl)
				: currentImpl(defaultImpl)
			{
				CHECK_ERROR(defaultImpl != nullptr, L"vl::feature_injection::FeatureInjection<TImpl>::FeatureInjection(TImpl*): Default implementation cannot be nullptr.");
			}

			/// <summary>Get the current active implementation.</summary>
			/// <returns>The current implementation, never nullptr after proper initialization.</returns>
			TImpl* Get()
			{
				return static_cast<TImpl*>(currentImpl);
			}

			/// <summary>Inject a new implementation, making it the current active one.</summary>
			/// <param name="impl">The implementation to inject, must not be nullptr.</param>
			void Inject(TImpl* impl)
			{
				CHECK_ERROR(impl != nullptr, L"vl::feature_injection::FeatureInjection<TImpl>::Inject(TImpl*): Implementation cannot be nullptr.");
				static_cast<IFeatureImpl*>(impl)->BeginInjection(currentImpl);
				currentImpl = impl;
			}

			/// <summary>Eject an implementation from the chain, restoring previous implementations.</summary>
			/// <param name="impl">The implementation to eject.</param>
			void Eject(TImpl* impl)
			{
				CHECK_ERROR(impl != nullptr, L"vl::feature_injection::FeatureInjection<TImpl>::Eject(TImpl*): Implementation cannot be nullptr.");

				// Eject all implementations from currentImpl down to impl
				auto toEject = currentImpl;
				while (true)
				{
					CHECK_ERROR(toEject != nullptr, L"vl::feature_injection::FeatureInjection<TImpl>::Eject(TImpl*): Implementation not found in chain.");
					
					auto next = toEject->GetPreviousImpl();
					toEject->EndInjection();
					
					if (toEject == impl)
					{
						currentImpl = next;
						break;
					}
					
					toEject = next;
				}
			}
		};
	}
}
```

**Why this change is necessary:**
This template class provides the core functionality for managing injection operations in a type-safe manner. The key design decisions are: (1) Constructor initialization - requires a default implementation to ensure Get() never returns nullptr, (2) Single member variable - only currentImpl pointer needed with no dynamic allocation, (3) Type safety - template ensures compile-time type checking for specific feature interfaces, (4) Injection algorithm - calls impl->BeginInjection(currentImpl) to notify the new implementation about the previous one, then updates currentImpl, (5) Ejection algorithm - traverses the linked list to find the target implementation, then calls EndInjection() on each implementation from currentImpl down to the target in reverse order. The framework uses only a single pointer member variable and delegates all memory management to the caller, following Vlpp's no-allocation philosophy.

# !!!FINISHED!!!
