/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/
#ifndef VCZH_TUPLE
#define VCZH_TUPLE

#include "../Basic.h"

namespace vl
{
	template<vint I, typename T>
	class TupleElement
	{
	protected:
		T						element;

		TupleElement() = default;

		template<typename U>
		TupleElement(U&& _element)
			:element(std::forward<U&&>(_element))
		{
		}
	};

	struct TupleCtorElementsTag {};
	struct TupleCtorTupleTag {};

	template<typename Is, typename ...TArgs>
	class TupleBase;

	template<std::size_t ...Is, typename ...TArgs> requires(sizeof...(Is) == sizeof...(TArgs))
	class TupleBase<std::index_sequence<Is...>, TArgs...>
		: protected TupleElement<Is, TArgs>...
	{
	private:
		using TSelf = TupleBase<std::index_sequence<Is...>, TArgs...>;

		template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
		using TCompatible = TupleBase<std::index_sequence<Is...>, UArgs...>;

	protected:
		TupleBase() = default;

		template<typename ...UArgs>
		TupleBase(TupleCtorElementsTag, UArgs&& ...xs)
			: TupleElement<Is, TArgs>(std::forward<UArgs&&>(xs)) ...
		{
		}

		template<typename ...UArgs>
		TupleBase(TupleCtorTupleTag, const TCompatible<UArgs...>& t)
			: TupleElement<Is, TArgs>(static_const<const TupleElement<Is, UArgs>&>(t).element) ...
		{
		}

		template<typename ...UArgs>
		TupleBase(TupleCtorTupleTag, TCompatible<UArgs...>&& t)
			: TupleElement<Is, TArgs>(std::move(static_const<TupleElement<Is, UArgs>&>(t).element)) ...
		{
		}

		template<typename ...UArgs>
		void AssignCopy(const TCompatible<UArgs...>& t)
		{
			((static_cast<TupleElement<Is, TArgs>*>(this)->element = static_cast<const TupleElement<Is, UArgs>&>(t).element), ...);
		}

		template<typename ...UArgs>
		void AssignMove(TCompatible<UArgs...>&& t)
		{
			((static_cast<TupleElement<Is, TArgs>*>(this)->element = std::move(static_cast<TupleElement<Is, UArgs>&&>(t).element)), ...);
		}

		template<typename ...UArgs>
		bool AreEqual(const TCompatible<UArgs...>& t)const
		{
			return (true && ... && (static_cast<const TupleElement<Is, TArgs>*>(this)->element == static_cast<const TupleElement<Is, UArgs>&>(t).element));
		}
	};

	template<typename ...TArgs>
	class Tuple : protected TupleBase<std::make_index_sequence<sizeof...(TArgs)>, TArgs...>
	{
	private:
		using TSelf = Tuple<TArgs...>;
		using TBase = TupleBase<std::make_index_sequence<sizeof...(TArgs)>, TArgs...>;

		template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
		using TCompatible = Tuple<UArgs...>;

		template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
		using TCompatibleBase = TupleBase<std::make_index_sequence<sizeof...(TArgs)>, UArgs...>;

	public:
		Tuple() = default;

		template<typename ...UArgs>
		Tuple(UArgs&& ...xs) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				TupleCtorElementsTag,
				std::forward<UArgs&&>(xs)...
			)
		{
		}

		template<typename ...UArgs>
		Tuple(const TCompatible<UArgs...>& t) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				TupleCtorTupleTag,
				static_cast<const TCompatibleBase<UArgs...>&>(t)
			)
		{
		}

		template<typename ...UArgs>
		Tuple(TCompatible<UArgs...>&& t) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				TupleCtorTupleTag,
				static_cast<TCompatibleBase<UArgs...>&&>(t)
			)
		{
		}

		template<typename ...UArgs>
		TSelf& operator=(const TCompatible<UArgs...>& t)
		{
			AssignCopy(t);
			return *this;
		}

		template<typename ...UArgs>
		TSelf& operator=(TCompatible<UArgs...>&& t)
		{
			AssignMove(std::move(t));
			return *this;
		}

		template<typename ...UArgs>
		bool operator==(const TCompatible<UArgs...>& t)const
		{
			return AreEqual(t);
		}

		template<vint Index>
		TypeTupleElement<Index, TypeTuple<TArgs...>>& Get()
		{
			return static_cast<TupleElement<Index, TypeTupleElement<Index, TypeTuple<TArgs...>>>*>(this)->element;
		}

		template<vint Index>
		const TypeTupleElement<Index, TypeTuple<TArgs...>>& Get()const
		{
			return static_cast<const TupleElement<Index, TypeTupleElement<Index, TypeTuple<TArgs...>>>*>(this)->element;
		}
	};

	template<>
	class Tuple<>
	{
	public:
		Tuple() = default;
		Tuple(const Tuple<>&) = default;
		Tuple(Tuple<>&&) = default;

		Tuple<>& operator=(const Tuple<>&) = default;
		Tuple<>& operator=(Tuple<>&&) = default;
		constexpr std::strong_ordering operator<=>(const Tuple<>&) const { return std::strong_ordering::equal; }
		constexpr bool operator==(const Tuple<>&) const { return true; }
	};

	template<typename T>
	struct TupleElementCtad { using Type = std::remove_cvref_t<T>; };

	template<typename T, vint I>
	struct TupleElementCtad<T(&)[I]> { using Type = T*; };

	template<typename ...TArgs>
	Tuple(TArgs&&...) -> Tuple<typename TupleElementCtad<TArgs>::Type...>;

	template<vint Index, typename ...TArgs>
	auto& get(const Tuple<TArgs...>& t)
	{
		return t.template Get<Index>();
	}
	
	template<vint Index, typename ...TArgs>
	 auto& get(Tuple<TArgs...>& t)
	{
		return t.template Get<Index>();
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