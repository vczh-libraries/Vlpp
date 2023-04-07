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

			std::strong_ordering operator<=>(const Pair<K, V>&) const = default;
			bool operator==(const Pair<K, V>&) const = default;
		};

		template<typename K, typename V>
		class Pair<const K&, const V&>
		{
		public:
			const K&		key;
			const V&		value;

#if defined(__clang__)
#pragma clang dignostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#elif defined(__GNUC__)
#pragma GCC dignostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
			Pair()
				: key(*(const K*)nullptr)
				, value(*(const V*)nullptr)
			{
			}
#if defined(__clang__)
#pragma clang dignostic pop
#elif defined(__GNUC__)
#pragma GCC dignostic popd
#endif

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

			std::strong_ordering operator<=>(const Pair<const K&, const V&>& p) const
			{
				std::strong_ordering result;
				result = key <=> p.key; if (result != 0) return result;
				result = value <=> p.value; if (result != 0) return result;
				return std::strong_ordering::equal;
			}

			bool operator==(const Pair<const K&, const V&>& p) const
			{
				return operator<=>(p) == 0;
			}
		};
	}
}

#endif