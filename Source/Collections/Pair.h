/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_PAIR
#define VCZH_COLLECTIONS_PAIR

#include "../Basic.h"

namespace vl
{
	namespace collections
	{
		template<typename K, typename V>
		class Pair;

		namespace pair_internal
		{
			template<typename T>
			struct AddConstInsideReference
			{
				using Type = const T;
			};

			template<typename T>
			struct AddConstInsideReference<T&>
			{
				using Type = const T&;
			};

			template<vint Index, typename TPair>
			struct TypePairElementRetriver;

			template<vint Index, typename K, typename V>
			struct TypePairElementRetriver<Index, Pair<K, V>>
			{
				using Type = TypeTupleElement<Index, TypeTuple<K, V>>;
			};

			template<vint Index, typename K, typename V>
			struct TypePairElementRetriver<Index, const Pair<K, V>>
			{
				using Type = typename AddConstInsideReference<TypeTupleElement<Index, TypeTuple<K, V>>>::Type;
			};
		}

		// G++ workaround
		template<typename K, typename V, size_t Index>
		struct PairGet;

		/// <summary>A type representing a pair of key and value.</summary>
		/// <typeparam name="K">Type of the key.</typeparam>
		/// <typeparam name="V">Type of the value.</typeparam>
		template<typename K, typename V>
		class Pair
		{
		public:
			/// <summary>The key.</summary>
			K				key;
			/// <summary>The value.</summary>
			V				value;

			Pair() = default;
			Pair(const Pair<K, V>&) = default;
			Pair(Pair<K, V>&&) = default;

			template<typename TKey, typename TValue>
			Pair(TKey&& _key, TValue&& _value)
				requires(std::is_constructible_v<K, TKey&&> && std::is_constructible_v<V, TValue&&>)
				: key(std::forward<TKey&&>(_key))
				, value(std::forward<TValue&&>(_value))
			{
			}

			template<typename TKey, typename TValue>
			Pair(const Pair<TKey, TValue>& p)
				requires(std::is_constructible_v<K, const TKey&> && std::is_constructible_v<K, const TKey&>)
				: key(p.key)
				, value(p.value)
			{
			}

			template<typename TKey, typename TValue>
			Pair(Pair<TKey, TValue>&& p)
				requires(std::is_constructible_v<K, TKey&&> && std::is_constructible_v<K, TKey&&>)
				: key(std::move(p.key))
				, value(std::move(p.value))
			{
			}

			Pair<K, V>& operator=(const Pair<K, V>&) = default;
			Pair<K, V>& operator=(Pair<K, V>&&) = default;

			template<typename TKey, typename TValue>
			auto operator<=>(const Pair<TKey, TValue>& p) const
				requires(std::three_way_comparable_with<const K, const TKey> && std::three_way_comparable_with<const V, const TValue>)
			{
				using TOrdering = OrderingOf<decltype(key <=> p.key), decltype(value <=> p.value)>;
				{ auto result = key <=> p.key; if (result != 0) return (TOrdering)result; }
				{ auto result = value <=> p.value; if (result != 0) return (TOrdering)result; }
				return (TOrdering)std::strong_ordering::equal;
			}

			template<typename TKey, typename TValue>
			bool operator==(const Pair<TKey, TValue>& p) const
				requires(std::equality_comparable_with<const K, const TKey>&& std::equality_comparable_with<const V, const TValue>)
			{
				return key == p.key && value == p.value;
			}

			template<size_t Index>
			decltype(auto) get()
			{
				return PairGet<K, V, Index>::get(*this);
			}

			template<size_t Index>
			decltype(auto) get() const
			{
				return PairGet<K, V, Index>::get(*this);
			}
		};

		template<typename K, typename V>
		struct PairGet<K, V, 0>
		{
			static typename pair_internal::TypePairElementRetriver<0, Pair<K, V>>::Type& get(Pair<K, V>& p) { return p.key; }
			static typename pair_internal::TypePairElementRetriver<0, const Pair<K, V>>::Type& get(const Pair<K, V>& p) { return p.key; }
		};


		template<typename K, typename V>
		struct PairGet<K, V, 1>
		{
			static typename pair_internal::TypePairElementRetriver<1, Pair<K, V>>::Type& get(Pair<K, V>& p) { return p.value; }
			static typename pair_internal::TypePairElementRetriver<1, const Pair<K, V>>::Type& get(const Pair<K, V>& p) { return p.value; }
		};

		template<typename K, typename V>
		Pair(K&&, V&&) -> Pair<typename RemoveCVRefArrayCtad<K>::Type, typename RemoveCVRefArrayCtad<V>::Type>;
	}
}

namespace std
{
	template<typename K, typename V>
	struct tuple_size<vl::collections::Pair<K, V>> : integral_constant<size_t, 2> {};

	template<size_t Index, typename K, typename V>
	struct tuple_element<Index, vl::collections::Pair<K, V>>
	{
		using type = decltype(std::declval<vl::collections::Pair<K, V>>().template get<Index>());
	};

	template<size_t Index, typename K, typename V>
	struct tuple_element<Index, const vl::collections::Pair<K, V>>
	{
		using type = decltype(std::declval<const vl::collections::Pair<K, V>>().template get<Index>());
	};
}

#endif