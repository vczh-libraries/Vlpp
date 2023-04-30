/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/
#ifndef VCZH_TUPLE
#define VCZH_TUPLE

#include "../Basic.h"

namespace vl
{
	namespace tuple_internal
	{
		template<vint I, typename T>
		struct TupleElement
		{
			T						element;

			TupleElement() = default;

			template<typename U>
			TupleElement(U&& _element)
				:element(std::forward<U&&>(_element))
			{
			}

			template<typename U>
			void AssignCopy(const TupleElement<I, U>& value)
			{
				element = value.GetElement();
			}

			template<typename U>
			void AssignMove(TupleElement<I, U>&& value)
			{
				element = std::move(value.GetElement());
			}

			template<typename U>
			bool operator==(const TupleElement<I, U>& value) const
			{
				return element == value.GetElement();
			}

			T& GetElement()
			{
				return element;
			}

			const T& GetElement() const
			{
				return element;
			}
		};

		template<vint I, typename T>
		struct TupleElement<I, T&>
		{
			T*						element;

			TupleElement() = delete;

			template<typename U>
			TupleElement(U&& _element)
				:element(&_element)
			{
			}

			template<typename U>
			void AssignCopy(const TupleElement<I, U>& value)
			{
				element = &value.GetElement();
			}

			template<typename U>
			void AssignMove(TupleElement<I, U>&& value)
			{
				element = &value.GetElement();
			}

			template<typename U>
			bool operator==(const TupleElement<I, U>& value) const
			{
				return *element == value.GetElement();
			}

			T& GetElement()
			{
				return *element;
			}

			const T& GetElement() const
			{
				return *element;
			}
		};

		template<typename TOrdering, typename T, typename U>
		struct TupleElementComparison
		{
			const T&				t;
			const U&				u;

			TupleElementComparison(const T& _t, const U& _u)
				: t(_t)
				, u(_u)
			{
			}

			template<typename TPreviousOrdering>
			friend TOrdering operator*(TPreviousOrdering order, const TupleElementComparison<TOrdering, T, U>& t)
			{
				if (order != 0) return (TOrdering)order;
				return (TOrdering)(t.t <=> t.u);
			}
		};

		struct TupleCtorElementsTag {};
		struct TupleCtorTupleTag {};

		template<typename Is, typename ...TArgs>
		struct TupleBase;

		template<std::size_t ...Is, typename ...TArgs> requires(sizeof...(Is) == sizeof...(TArgs))
		struct TupleBase<std::index_sequence<Is...>, TArgs...>
			: TupleElement<Is, TArgs>...
		{
		private:
			using TSelf = TupleBase<std::index_sequence<Is...>, TArgs...>;

			template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
			using TCompatible = TupleBase<std::index_sequence<Is...>, UArgs...>;

		public:
			TupleBase() = default;

			template<typename ...UArgs>
			TupleBase(TupleCtorElementsTag, UArgs&& ...xs)
				: TupleElement<Is, TArgs>(std::forward<UArgs&&>(xs)) ...
			{
			}

			template<typename ...UArgs>
			TupleBase(TupleCtorTupleTag, const TCompatible<UArgs...>& t)
				: TupleElement<Is, TArgs>(static_cast<const TupleElement<Is, UArgs>&>(t).GetElement()) ...
			{
			}

			template<typename ...UArgs>
			TupleBase(TupleCtorTupleTag, TCompatible<UArgs...>&& t)
				: TupleElement<Is, TArgs>(std::move(static_cast<TupleElement<Is, UArgs>&>(t).GetElement())) ...
			{
			}

			template<typename ...UArgs>
			void AssignCopy(const TCompatible<UArgs...>& t)
			{
				((
					static_cast<TupleElement<Is, TArgs>*>(this)->AssignCopy
					(
						static_cast<const TupleElement<Is, UArgs>&>(t)
					)
				), ...);
			}

			template<typename ...UArgs>
			void AssignMove(TCompatible<UArgs...>&& t)
			{
				((
					static_cast<TupleElement<Is, TArgs>*>(this)->AssignMove
					(
						std::move(static_cast<TupleElement<Is, UArgs>&&>(t))
					)
				), ...);
			}

			template<typename ...UArgs>
			bool AreEqual(const TCompatible<UArgs...>& t) const
			{
				return (true && ... && (
					*static_cast<const TupleElement<Is, TArgs>*>(this) == static_cast<const TupleElement<Is, UArgs>&>(t)
					));
			}

			template<typename ...UArgs>
			auto Compare(const TCompatible<UArgs...>& t) const
			{
#define LEFT_ELEMENT (static_cast<const TupleElement<Is, TArgs>*>(this)->GetElement())
#define RIGHT_ELEMENT (static_cast<const TupleElement<Is, UArgs>&>(t).GetElement())
				using TOrdering = OrderingOf<decltype(LEFT_ELEMENT <=> RIGHT_ELEMENT)...>;
				return (std::strong_ordering::equal * ... * (TupleElementComparison<TOrdering, TArgs, UArgs>(LEFT_ELEMENT, RIGHT_ELEMENT)));
#undef LEFT_ELEMENT
#undef RIGHT_ELEMENT
			}
		};
	}

	template<typename ...TArgs>
	class Tuple : private tuple_internal::TupleBase<std::make_index_sequence<sizeof...(TArgs)>, TArgs...>
	{
		template<typename ...UArgs>
		friend class Tuple;

		using TSelf = Tuple<TArgs...>;
		using TBase = tuple_internal::TupleBase<std::make_index_sequence<sizeof...(TArgs)>, TArgs...>;

		template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
		using TCompatible = Tuple<UArgs...>;

		template<typename ...UArgs> requires(sizeof...(TArgs) == sizeof...(UArgs))
		using TCompatibleBase = tuple_internal::TupleBase<std::make_index_sequence<sizeof...(TArgs)>, UArgs...>;

	public:
		Tuple() = default;

		template<typename ...UArgs>
		Tuple(UArgs&& ...xs) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				tuple_internal::TupleCtorElementsTag{},
				std::forward<UArgs&&>(xs)...
			)
		{
		}

		template<typename ...UArgs>
		Tuple(const TCompatible<UArgs...>& t) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				tuple_internal::TupleCtorTupleTag{},
				static_cast<const TCompatibleBase<UArgs...>&>(t)
			)
		{
		}

		template<typename ...UArgs>
		Tuple(TCompatible<UArgs...>&& t) requires(sizeof...(TArgs) == sizeof...(UArgs))
			: TBase(
				tuple_internal::TupleCtorTupleTag{},
				static_cast<TCompatibleBase<UArgs...>&&>(t)
			)
		{
		}

		template<typename ...UArgs>
		TSelf& operator=(const TCompatible<UArgs...>& t)
		{
			this->AssignCopy(t);
			return *this;
		}

		template<typename ...UArgs>
		TSelf& operator=(TCompatible<UArgs...>&& t)
		{
			this->AssignMove(std::move(t));
			return *this;
		}

		template<typename ...UArgs>
		auto operator<=>(const TCompatible<UArgs...>& t)const
			requires (true && ... && std::three_way_comparable_with<TArgs, UArgs>)
		{
			return this->Compare(t);
		}

		template<typename ...UArgs>
		bool operator==(const TCompatible<UArgs...>& t)const
			requires (true && ... && std::equality_comparable_with<TArgs, UArgs>)
		{
			return this->AreEqual(t);
		}

		template<vint Index>
		decltype(auto) get()
		{
			using TElement = tuple_internal::TupleElement<Index, TypeTupleElement<Index, TypeTuple<TArgs...>>>;
			return static_cast<TElement*>(this)->GetElement();
		}

		template<vint Index>
		decltype(auto) get() const
		{
			using TElement = tuple_internal::TupleElement<Index, TypeTupleElement<Index, TypeTuple<TArgs...>>>;
			return static_cast<const TElement*>(this)->GetElement();
		}
	};

	template<typename ...TArgs>
	Tuple(TArgs&&...) -> Tuple<typename RemoveCVRefArrayCtad<TArgs>::Type...>;

	template<vint Index, typename ...TArgs>
	decltype(auto) get(Tuple<TArgs...>& t)
	{
		return t.template get<Index>();
	}
	
	template<vint Index, typename ...TArgs>
	decltype(auto) get(const Tuple<TArgs...>& t)
	{
		return t.template get<Index>();
	}
}

namespace std
{
	template<typename ...TArgs>
	struct tuple_size<vl::Tuple<TArgs...>> : integral_constant<size_t, sizeof...(TArgs)> {};

	template<size_t Index, typename ...TArgs>
	struct tuple_element<Index, vl::Tuple<TArgs...>>
	{
		using type = decltype(std::declval<vl::Tuple<TArgs...>>().template get<Index>());
	};

	template<size_t Index, typename ...TArgs>
	struct tuple_element<Index, const vl::Tuple<TArgs...>>
	{
		using type = decltype(std::declval<const vl::Tuple<TArgs...>>().template get<Index>());
	};
}

#endif