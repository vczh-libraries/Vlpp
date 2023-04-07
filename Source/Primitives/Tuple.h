/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/
#ifndef VCZH_TUPLE
#define VCZH_TUPLE

#include "../Basic.h"

namespace vl
{
	template<typename ...TArgs>
	class Tuple;

	template<typename T, typename ...TArgs>
	class Tuple<T, TArgs...> : private Tuple<TArgs...>
	{
		template<typename ...TArgs>
		friend class Tuple;
	private:
		T				firstValue;

	public:
		Tuple() = default;
		Tuple(const Tuple<T, TArgs...>&) = default;
		Tuple(Tuple<T, TArgs...>&&) = default;
		~Tuple() = default;

		template<typename U, typename ...UArgs>
		Tuple(U&& x, UArgs&& ...xs)
			requires(sizeof...(TArgs) == sizeof...(UArgs) && (std::is_constructible_v<T, U&&> && ... && std::is_convertible_v<TArgs, UArgs&&>))
			: firstValue(std::forward<U&&>(x))
			, Tuple<TArgs...>(std::forward<UArgs&&>(xs)...)
		{
		}

		template<typename U, typename ...UArgs>
		Tuple(const Tuple<U, UArgs...>& t)
			requires(sizeof...(TArgs) == sizeof...(UArgs) && (std::is_constructible_v<T, const U&> && ... && std::is_convertible_v<TArgs, const UArgs&>))
			: firstValue(t.firstValue)
			, Tuple<TArgs...>(static_cast<const Tuple<UArgs...>&>(t))
		{
		}

		template<typename U, typename ...UArgs>
		Tuple(Tuple<U, UArgs...>&& t)
			requires(sizeof...(TArgs) == sizeof...(UArgs) && (std::is_constructible_v<T, const U&&> && ... && std::is_convertible_v<TArgs, UArgs&&>))
			: firstValue(std::move(t.firstValue))
			, Tuple<TArgs...>(std::move(static_cast<Tuple<UArgs...>&>(t)))
		{
		}

		Tuple<T, TArgs...>& operator=(const Tuple<T, TArgs...>&) = default;
		Tuple<T, TArgs...>& operator=(Tuple<T, TArgs...>&&) = default;

		template<vint Index>
		__forceinline TypeTupleElement<Index, TypeTuple<T, TArgs...>>& get()
		{
			if constexpr (Index == 0)
			{
				return firstValue;
			}
			else
			{
				return static_cast<Tuple<TArgs...>*>(this)->get<Index - 1>();
			}
		}

		template<vint Index>
		__forceinline const TypeTupleElement<Index, TypeTuple<T, TArgs...>>& get() const
		{
			if constexpr (Index == 0)
			{
				return firstValue;
			}
			else
			{
				return static_cast<const Tuple<TArgs...>*>(this)->get<Index - 1>();
			}
		}
	};

	template<>
	class Tuple<>
	{
		constexpr std::strong_ordering operator<=>(const Tuple<>&) const { return std::strong_ordering::equal; }
		constexpr bool operator==(const Tuple<>&) const { return true; }
	};

	template<typename ...TArgs>
	Tuple(TArgs&&...) -> Tuple<std::remove_cvref_t<TArgs>...>;

	template<vint Index, typename ...TArgs>
	__forceinline auto& get(const Tuple<TArgs...>& t)
	{
		return t.get<Index>();
	}
	
	template<vint Index, typename ...TArgs>
	__forceinline  auto& get(Tuple<TArgs...>& t)
	{
		return t.get<Index>();
	}
}

namespace std
{
	template<typename ...TArgs>
	struct tuple_size<vl::Tuple<TArgs...>> : integral_constant<size_t, sizeof...(TArgs)> {};

	template<size_t Index, typename ...TArgs>
	struct tuple_element<Index, vl::Tuple<TArgs...>>
	{
		using type = vl::TypeTupleElement<Index, vl::TypeTuple<TArgs...>>;
	};
}

#endif