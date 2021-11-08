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

			vint CompareTo(const Pair<K, V>& pair)const
			{
				if(key<pair.key)
				{
					return -1;
				}
				else if(key>pair.key)
				{
					return 1;
				}
				else if(value<pair.value)
				{
					return -1;
				}
				else if(value>pair.value)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

			bool operator==(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)==0;
			}

			bool operator!=(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)!=0;
			}

			bool operator<(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)<0;
			}

			bool operator<=(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)<=0;
			}

			bool operator>(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)>0;
			}

			bool operator>=(const Pair<K, V>& pair)const
			{
				return CompareTo(pair)>=0;
			}
		};
	}
}

#endif