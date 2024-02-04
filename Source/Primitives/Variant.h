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
	struct VariantIndex
	{
		static constexpr vint value = I;
	};
}

namespace vl::variant_internal
{
	template<typename T, typename ...TArgs>
	consteval T MaxOf(T first, TArgs ...others)
	{
		T result = first;
		T nexts[] = { others... };
		for (T next : nexts)
		{
			if (result < next) result = next;
		}
		return result;
	}
	template<typename T>
	consteval T MaxOf(T first)
	{
		return first;
	}

	template<vint I, typename T>
	struct VariantElement
	{
		template<typename U>
		static consteval VariantIndex<I> IndexOf()
			requires(std::is_same_v<T, U>)
		{
			return {};
		}

		static consteval T* TypeOf(VariantIndex<I>)
		{
			return nullptr;
		}

		static bool i_DefaultCtor(vint index, char* buffer)
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				if (I != index) return false;
				new (buffer)T();
				return true;
			}
			else
			{
				return false;
			}
		}

		static bool i_CopyCtor(vint index, char* buffer, char* source)
		{
			if constexpr (std::is_copy_constructible_v<T>)
			{
				if (I != index) return false;
				new (buffer)T(*reinterpret_cast<const T*>(source));
				return true;
			}
			else
			{
				return false;
			}
		}

		static bool i_MoveCtor(vint index, char* buffer, char* source)
		{
			if constexpr (std::is_move_constructible_v<T>)
			{
				if (I != index) return false;
				new (buffer)T(std::move(*reinterpret_cast<T*>(source)));
				return true;
			}
			else
			{
				return false;
			}
		}

		static bool i_Dtor(vint index, char* buffer)
		{
			if (I != index) return false;
			reinterpret_cast<T*>(buffer)->~T();
			return true;
		}

		static bool i_CopyAssign(vint index, char* buffer, char* source)
		{
			if constexpr (std::is_copy_assignable_v<T>)
			{
				if (I != index) return false;
				*reinterpret_cast<T*>(buffer) = *reinterpret_cast<T*>(source);
				return true;
			}
			else
			{
				return false;
			}
		}

		static bool i_MoveAssign(vint index, char* buffer, char* source)
		{
			if constexpr (std::is_move_constructible_v<T>)
			{
				if (I != index) return false;
				*reinterpret_cast<T*>(buffer) = std::move(*reinterpret_cast<T*>(source));
				return true;
			}
			else
			{
				return false;
			}
		}

		template<typename ...TArgs>
		static void Ctor(VariantIndex<I>, char* buffer, TArgs&& ...args)
			requires(std::is_constructible_v<T, TArgs&&...>)
		{
			new (buffer)T(std::forward<TArgs&&>(args)...);
		}

		static void DefaultCtor(VariantIndex<I>, char* buffer)
			requires(std::is_default_constructible_v<T>)
		{
			new (buffer)T();
		}

		static void CopyCtor(VariantIndex<I>, char* buffer, const T& source)
			requires(std::is_copy_constructible_v<T>)
		{
			new (buffer)T(source);
		}

		static void MoveCtor(VariantIndex<I>, char* buffer, T&& source)
			requires(std::is_move_constructible_v<T>)
		{
			new (buffer)T(std::move(source));
		}

		static void CopyAssign(VariantIndex<I>, char* buffer, const T& source)
			requires(std::is_copy_assignable_v<T>)
		{
			*reinterpret_cast<T*>(buffer) = source;
		}

		static void MoveAssign(VariantIndex<I>, char* buffer, T&& source)
			requires(std::is_move_assignable_v<T>)
		{
			*reinterpret_cast<T*>(buffer) = std::move(source);
		}
	};

	template<typename Is, typename ...TElements>
	struct VariantElementPack;

	template<std::size_t ...Is, typename ...TElements>
	struct VariantElementPack<std::index_sequence<Is...>, TElements...> : VariantElement<Is, TElements>...
	{
		using VariantElement<Is, TElements>::DefaultCtor...;
		using VariantElement<Is, TElements>::Ctor...;
		using VariantElement<Is, TElements>::CopyCtor...;
		using VariantElement<Is, TElements>::MoveCtor...;
		using VariantElement<Is, TElements>::CopyAssign...;
		using VariantElement<Is, TElements>::MoveAssign...;

		static bool DefaultCtor(vint index, char* buffer)
		{
			return (VariantElement<Is, TElements>::i_efaultCtor(index, buffer) || ...);
		}

		static bool CopyCtor(vint index, char* buffer, char* source)
		{
			return (VariantElement<Is, TElements>::i_CopyCtor(index, buffer, source) || ...);
		}

		static bool MoveCtor(vint index, char* buffer, char* source)
		{
			return (VariantElement<Is, TElements>::i_MoveCtor(index, buffer, source) || ...);
		}

		static bool Dtor(vint index, char* buffer)
		{
			return (VariantElement<Is, TElements>::i_Dtor(index, buffer) || ...);
		}

		static bool CopyAssign(vint index, char* buffer, char* source)
		{
			return (VariantElement<Is, TElements>::i_CopyAssign(index, buffer, source) || ...);
		}

		static bool MoveAssign(vint index, char* buffer, char* source)
		{
			return (VariantElement<Is, TElements>::i_MoveAssign(index, buffer, source) || ...);
		}
	};
}

namespace vl
{
	template<typename T>
	concept VariantElementType = requires()
	{
		std::is_same_v<T, std::remove_cvref_t<T>>;
	};

	template<VariantElementType ...TElements>
	class alignas(TElements...) Variant
	{
	public:
		template<VariantElementType ...UElements>
		friend class Variant;

		using ElementPack = variant_internal::VariantElementPack<std::make_index_sequence<sizeof...(TElements)>, TElements...>;

		template<typename T>
		static constexpr vint IndexOf = decltype(ElementPack::template IndexOf<T>())::value;

		template<std::size_t I>
		using ElementOf = std::remove_pointer_t<decltype(ElementPack::TypeOf(VariantIndex<I>{}))>;

		static constexpr std::size_t	MaxSize = variant_internal::MaxOf(sizeof(TElements)...);
		vint							index = -1;
		char							buffer[MaxSize];

	public:
		Variant()
			requires(std::is_default_constructible_v<ElementOf<0>>)
			: index(0)
		{
			ElementPack::DefaultCtor(VariantIndex<0>{}, buffer);
		}

		Variant(const Variant<TElements...>& variant)
			: index(variant.index)
		{
			ElementPack::CopyCtor(index, buffer, variant.buffer);
		}

		Variant(Variant<TElements...>&& variant)
			: index(variant.index)
		{
			ElementPack::MoveCtor(index, buffer, variant.buffer);
		}

		template<typename T>
		Variant(const T& element)
		{
			consteval auto i = IndexOf<T>;
			index = i;
			ElementPack::CopyCtor(VariantIndex<i>{}, element);
		}

		template<typename T>
		Variant(T&& element)
		{
			consteval auto i = IndexOf<T>;
			index = i;
			ElementPack::MoveCtor(VariantIndex<i>{}, std::move(element));
		}

		template<vint I, typename ...TArgs>
		Variant(VariantIndex<I> i, TArgs&& ...args)
			: index(i)
		{
			ElementPack::Ctor(i, std::forward<TArgs&&>(args)...);
		}

		~Variant()
		{
			ElementPack::Dtor(index, buffer);
		}

		Variant<TElements...>& operator=(const Variant<TElements...>& variant)
		{
			if (index == variant.index)
			{
				ElementPack::CopyAssign(index, buffer, variant.buffer);
			}
			else
			{
				ElementPack::Dtor(index, buffer);
				index = variant.index;
				ElementPack::CopyCtor(index, buffer, variant.buffer);
			}
			return *this;
		}

		Variant<TElements...>& operator=(Variant<TElements...>&& variant)
		{
			if (index == variant.index)
			{
				ElementPack::MoveAssign(index, buffer, variant.buffer);
			}
			else
			{
				ElementPack::Dtor(index, buffer);
				index = variant.index;
				ElementPack::MoveCtor(index, buffer, variant.buffer);
			}
			return *this;
		}

		template<typename T>
		Variant<TElements...>& operator=(const T& element)
		{
			consteval auto i = IndexOf<T>;
			if (index == i)
			{
				ElementPack::CopyAssign(VariantIndex<i>{}, buffer, element);
			}
			else
			{
				ElementPack::Dtor(index, buffer);
				index = i;
				ElementPack::CopyCtor(VariantIndex<i>{}, buffer, element);
			}
			return *this;
		}

		template<typename T>
		Variant<TElements...>& operator=(T&& element)
		{
			consteval auto i = IndexOf<T>;
			if (index == i)
			{
				ElementPack::MoveAssign(VariantIndex<i>{}, buffer, std::move(element));
			}
			else
			{
				ElementPack::Dtor(index, buffer);
				index = i;
				ElementPack::MoveCtor(VariantIndex<i>{}, buffer, std::move(element));
			}
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
			return const_cast<T*>(static_cast<const Variant<TElements...>*>(this)->TryGet<T>());
		}

		template<typename T>
		const T* TryGet() const
		{
			constexpr auto i = IndexOf<T>;
			return index == i ? reinterpret_cast<const T*>(buffer) : nullptr;
		}
	};
}

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#endif