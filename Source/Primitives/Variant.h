/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_VARIANT
#define VCZH_VARIANT

#include "../Basic.h"

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif

namespace vl
{
	template<vint I>
	struct VariantIndex {};

	namespace variant_internal
	{
		template<vint I, typename T>
		struct VariantElement
		{
			static consteval vint IndexOf(T*)				{ return I; }
			static consteval T* TypeOf(VariantIndex<I>)		{ return nullptr; }

			static void DefaultCtor(vint index, char* buffer)
			{
				if (I != index) return;
				new T(buffer)();
			}

			static void CopyCtor(vint index, char* buffer, char* source)
			{
				if (I != index) return;
				new T(buffer)(*reinterpret_cast<const T*>(source));
			}

			static void MoveCtor(vint index, char* buffer, char* source)
			{
				if (I != index) return;
				new T(buffer)(std::move(*reinterpret_cast<T*>(source)));
			}

			static void Dtor(vint index, char* buffer)
			{
				if (I != index) return;
				reinterpret_cast<T*>(buffer)->~T();
			}

			static void CopyAssign(vint index, char* buffer, char* source)
			{
				if (I != index) return;
				*reinterpret_cast<T*>(buffer) = *reinterpret_cast<T*>(source);
			}

			static void MoveAssign(vint index, char* buffer, char* source)
			{
				if (I != index) return;
				*reinterpret_cast<T*>(buffer) = std::move(*reinterpret_cast<T*>(source));
			}

			template<typename ...TArgs>
			static void Ctor(VariantIndex<I>, char* buffer, TArgs&& ...args)
			{
				new T(buffer)(std::forward<TArgs&&>(args)...);
			}

			static void CopyCtor(VariantIndex<I>, char* buffer, const T& source)
			{
				new T(buffer)(source);
			}

			static void MoveCtor(VariantIndex<I>, char* buffer, T&& source)
			{
				new T(buffer)(std::move(source));
			}

			static void CopyAssign(VariantIndex<I>, char* buffer, const T& source)
			{
				*reinterpret_cast<T*>(buffer) = source;
			}

			static void CopyAssign(VariantIndex<I>, char* buffer, T&& source)
			{
				*reinterpret_cast<T*>(buffer) = std::move(source);
			}
		};

		template<typename Is, typename ...TElements>
		struct VariantElementPack;

		template<std::size_t ...Is, typename ...TElements>
		struct VariantElementPack<std::index_sequence<Is...>, TElements...> : VariantElement<Is, TElements>...
		{
		};

		template<typename ...TElements>
		using VariantElementPackOf = VariantElementPack<std::make_index_sequence<sizeof...(TElements)>, TElements...>;
	}

	template<typename ...TElements>
		requires((
			(std::is_same_v<TElements, std::remove_cvref_t<TElements>>)
			&& ...
		))
	class alignas(TElements...) Variant
	{
		using ElementPack = variant_internal::VariantElementPackOf<TElements>;
		static constexpr std::size_t	MaxSize = std::max(sizeof(TElements...));
		vint							index = -1;
		char							buffer[MaxSize];

	public:
		Variant()
		{
		}

		Variant(const Variant<TElements...>& variant)
		{
		}

		Variant(Variant<TElements...>&& variant)
		{
		}

		template<typename T>
		Variant(const T& element)
		{
		}

		template<typename T>
		Variant(T&& element)
		{
		}

		template<vint I, typename ...TArgs>
		Variant(VariantIndex<I>, TArgs&& ...args)
		{
		}

		~Variant()
		{
		}

		Variant<TElements...>& operator=(const Variant<TElements...>& variant)
		{
		}

		Variant<TElements...>& operator=(Variant<TElements...>&& variant)
		{
		}

		template<typename T>
		Variant<TElements...>& operator=(const T& element)
		{
		}

		template<typename T>
		Variant<TElements...>& operator=(T&& element)
		{
		}

		vint Index() const
		{
			return index;
		}

		template<typename T>
		T& Get()&
		{
			auto result = TryGet<T>();
			CHECK_ERROR(result != nullptr, L"vl::Variant<T...>::Get<T>()#Content does not match the type.");
			return *result;
		}

		template<typename T>
		T&& Get()&&
		{
			auto result = TryGet<T>();
			CHECK_ERROR(result != nullptr, L"vl::Variant<T...>::Get<T>()#Content does not match the type.");
			return std::move(*result);
		}

		template<typename T>
		const T& Get() const&
		{
			auto result = TryGet<T>();
			CHECK_ERROR(result != nullptr, L"vl::Variant<T...>::Get<T>()#Content does not match the type.");
			return *result;
		}

		template<typename T>
		T* TryGet()
		{
			return const_cast<T*>(static_cast<const Variant<TElements...>>(this)->TryGet());
		}

		template<typename T>
		const T* TryGet() const
		{
		}
	};
}

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#endif