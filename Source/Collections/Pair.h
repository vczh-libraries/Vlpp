/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Pair

Classes:
	Pair<K, V>							: Pair
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

			Pair()
			{
			}

			Pair(const K& _key, const V& _value)
			{
				key=_key;
				value=_value;
			}

			Pair(const Pair<K, V>& pair)
			{
				key=pair.key;
				value=pair.value;
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

	template<typename K, typename V>
	struct POD<collections::Pair<K, V>>
	{
		static const bool Result=POD<K>::Result && POD<V>::Result;
	};
}

#endif