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
			std::strong_ordering operator<=>(const Pair<TKey, TValue>& p) const
				requires(std::three_way_comparable_with<const K, const TKey, std::strong_ordering> && std::three_way_comparable_with<const V, const TValue, std::strong_ordering>)
			{
				std::strong_ordering result;
				result = key <=> p.key; if (result != 0) return result;
				result = value <=> p.value; if (result != 0) return result;
				return std::strong_ordering::equal;
			}

			template<typename TKey, typename TValue>
			bool operator==(const Pair<TKey, TValue>& p) const
				requires(std::equality_comparable_with<const K, const TKey>&& std::equality_comparable_with<const V, const TValue>)
			{
				return key == p.key && value == p.value;
			}
		};

		template<typename K, typename V>
		Pair(K&&, V&&) -> Pair<std::remove_cvref_t<K>, std::remove_cvref_t<V>>;
	}
}

#endif