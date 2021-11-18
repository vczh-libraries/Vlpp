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

			template<typename TKey, typename TValue>
			Pair(TKey&& _key, TValue&& _value)
				: key(std::forward<TKey&&>(_key))
				, value(std::forward<TValue&&>(_value))
			{
			}

			Pair(const K& _key, const V& _value)
				: key(_key)
				, value(_value)
			{
			}

			Pair(const K& _key, V&& _value)
				: key(_key)
				, value(std::move(_value))
			{
			}

			Pair(K&& _key, const V& _value)
				: key(std::move(_key))
				, value(_value)
			{
			}

			Pair(K&& _key, V&& _value)
				: key(std::move(_key))
				, value(std::move(_value))
			{
			}

			Pair(const Pair<const K&, const V&>& pair)
				: key(pair.key)
				, value(pair.value)
			{
			}

			Pair(const Pair<K, V>& pair)
				: key(pair.key)
				, value(pair.value)
			{
			}

			Pair(Pair<K, V>&& pair)
				: key(std::move(pair.key))
				, value(std::move(pair.value))
			{
			}

			Pair<K, V>& operator=(const Pair<K, V>& pair)
			{
				key = pair.key;
				value = pair.value;
				return *this;
			}

			Pair<K, V>& operator=(Pair<K, V>&& pair)
			{
				key = std::move(pair.key);
				value = std::move(pair.value);
				return *this;
			}

			template<typename K2, typename V2>
			auto CompareTo(const Pair<K2, V2>& pair) const -> std::enable_if_t<
				std::is_same_v<std::remove_cvref_t<K>, std::remove_cvref_t<K2>> &&
				std::is_same_v<std::remove_cvref_t<V>, std::remove_cvref_t<V2>>,
				vint>
			{
				if (key < pair.key)
				{
					return -1;
				}
				else if (key > pair.key)
				{
					return 1;
				}
				else if (value < pair.value)
				{
					return -1;
				}
				else if (value > pair.value)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

			template<typename TPair>
			bool operator==(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) == 0;
			}

			template<typename TPair>
			bool operator!=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) != 0;
			}

			template<typename TPair>
			bool operator<(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) < 0;
			}

			template<typename TPair>
			bool operator<=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) <= 0;
			}

			template<typename TPair>
			bool operator>(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) > 0;
			}

			template<typename TPair>
			bool operator>=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) >= 0;
			}
		};

		template<typename K, typename V>
		class Pair<const K&, const V&>
		{
		public:
			const K&		key;
			const V&		value;

			Pair()
				: key(*(const K*)nullptr)
				, value(*(const V*)nullptr)
			{
			}

			Pair(const K& _key, const V& _value)
				: key(_key)
				, value(_value)
			{
			}

			Pair(const Pair<const K&, const V&>& pair)
				: key(pair.key)
				, value(pair.value)
			{
			}

			Pair<const K&, const V&>& operator=(const Pair<const K&, const V&>& pair)
			{

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif
				this->~Pair<const K&, const V&>();
				new(this) Pair<const K&, const V&>(pair);
				return *this;
#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif
			}

			template<typename K2, typename V2>
			auto CompareTo(const Pair<K2, V2>& pair) const -> std::enable_if_t<
				std::is_same_v<std::remove_cvref_t<K>, std::remove_cvref_t<K2>>&&
				std::is_same_v<std::remove_cvref_t<V>, std::remove_cvref_t<V2>>,
				vint>
			{
				if (key < pair.key)
				{
					return -1;
				}
				else if (key > pair.key)
				{
					return 1;
				}
				else if (value < pair.value)
				{
					return -1;
				}
				else if (value > pair.value)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

			template<typename TPair>
			bool operator==(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) == 0;
			}

			template<typename TPair>
			bool operator!=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) != 0;
			}

			template<typename TPair>
			bool operator<(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) < 0;
			}

			template<typename TPair>
			bool operator<=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) <= 0;
			}

			template<typename TPair>
			bool operator>(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) > 0;
			}

			template<typename TPair>
			bool operator>=(TPair&& pair)const
			{
				return CompareTo(std::forward<TPair&&>(pair)) >= 0;
			}
		};
	}
}

#endif