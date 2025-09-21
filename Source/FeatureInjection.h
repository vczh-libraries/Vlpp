/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_FEATUREINJECTION
#define VCZH_FEATUREINJECTION

#include "Basic.h"

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

		/// <summary>
		/// Default implementation template for IFeatureImpl providing type-safe delegation.
		/// TImpl should be the actual feature interface that inherits from IFeatureImpl.
		/// </summary>
		template<typename TImpl>
		    requires std::is_base_of_v<IFeatureImpl, TImpl>
		class FeatureImpl : public Object, public virtual TImpl
        {
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

		protected:
			/// <summary>Get the previous implementation with type safety.</summary>
			/// <returns>The previous implementation, or nullptr if this is the first in chain.</returns>
			TImpl* Previous()
			{
				return _previousImpl; // Already verified and cast during BeginInjection
			}
		};

		/// <summary>
		/// Template class for managing feature injection with type-safe operations.
		/// Maintains a linked list of implementations and provides inject/eject functionality.
		/// </summary>
		template<typename TImpl>
		    requires std::is_base_of_v<IFeatureImpl, TImpl>
		class FeatureInjection
        {
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
				return dynamic_cast<TImpl*>(currentImpl);
			}

			/// <summary>Inject a new implementation, making it the current active one.</summary>
			/// <param name="impl">The implementation to inject, must not be nullptr.</param>
			void Inject(TImpl* impl)
			{
				CHECK_ERROR(impl != nullptr, L"vl::feature_injection::FeatureInjection<TImpl>::Inject(TImpl*): Implementation cannot be nullptr.");
				dynamic_cast<IFeatureImpl*>(impl)->BeginInjection(currentImpl);
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
		};
	}
}

#endif