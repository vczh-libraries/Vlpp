/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_DICTIONARY
#define VCZH_COLLECTIONS_DICTIONARY

#include "List.h"

namespace vl
{
	namespace collections
	{
		/// <summary>Dictionary: one to one map container.</summary>
		/// <typeparam name="KT">Type of keys.</typeparam>
		/// <typeparam name="VT">Type of values.</typeparam>
		/// <typeparam name="KK">Type of the key type of keys. It is recommended to use the default value.</typeparam>
		/// <typeparam name="VK">Type of the key type of values. It is recommended to use the default value.</typeparam>
		template<
			typename KT,
			typename VT,
			typename KK=typename KeyType<KT>::Type, 
			typename VK=typename KeyType<VT>::Type
		>
		class Dictionary : public EnumerableBase<Pair<KT, VT>>
		{
		public:
			typedef SortedList<KT, KK>			KeyContainer;
			typedef List<VT, VK>				ValueContainer;
		protected:
			class Enumerator : public Object, public virtual IEnumerator<Pair<KT, VT>>
			{
			private:
				const Dictionary<KT, VT, KK, VK>*	container;
				vint								index;
				Pair<KT, VT>						current;

				void UpdateCurrent()
				{
					if(index<container->Count())
					{
						current.key=container->Keys().Get(index);
						current.value=container->Values().Get(index);
					}
				}
			public:
				Enumerator(const Dictionary<KT, VT, KK, VK>* _container, vint _index=-1)
				{
					container=_container;
					index=_index;
				}
				
				IEnumerator<Pair<KT, VT>>* Clone()const override
				{
					return new Enumerator(container, index);
				}

				const Pair<KT, VT>& Current()const override
				{
					return current;
				}

				vint Index()const override
				{
					return index;
				}

				bool Next() override
				{
					index++;
					UpdateCurrent();
					return index>=0 && index<container->Count();
				}

				void Reset() override
				{
					index=-1;
					UpdateCurrent();
				}

				bool Evaluated()const override
				{
					return true;
				}
			};

			KeyContainer						keys;
			ValueContainer						values;
		public:
			/// <summary>Create an empty dictionary.</summary>
			Dictionary() = default;
			~Dictionary() = default;

			Dictionary(const Dictionary<KT, VT, KK, VK>&) = delete;
			Dictionary(Dictionary<KT, VT, KK, VK> && _move)
				: keys(std::move(_move.keys))
				, values(std::move(_move.values))
			{
			}

			Dictionary<KT, VT, KK, VK>& operator=(const Dictionary<KT, VT, KK, VK>&) = delete;
			Dictionary<KT, VT, KK, VK>& operator=(Dictionary<KT, VT, KK, VK> && _move)
			{
				keys = std::move(_move.keys);
				values = std::move(_move.values);
				return* this;
			}

			IEnumerator<Pair<KT, VT>>* CreateEnumerator()const
			{
				return new Enumerator(this);
			}

			/// <summary>Get all keys.</summary>
			/// <returns>All keys.</returns>
			const KeyContainer& Keys()const
			{
				return keys;
			}
			
			/// <summary>Get all values.</summary>
			/// <returns>All values.</returns>
			const ValueContainer& Values()const
			{
				return values;
			}

			/// <summary>Get the number of keys.</summary>
			/// <returns>The number of keys. It is also the number of values.</returns>
			vint Count()const
			{
				return keys.Count();
			}

			/// <summary>Get the reference to the value associated to a specified key.</summary>
			/// <returns>The reference to the value. It will crash if the key does not exist.</returns>
			/// <param name="key">The key to find.</param>
			const VT& Get(const KK& key)const
			{
				return values.Get(keys.IndexOf(key));
			}

			/// <summary>Get the reference to the value associated to a specified key.</summary>
			/// <returns>The reference to the value. It will crash if the key does not exist.</returns>
			/// <param name="key">The key to find.</param>
			const VT& operator[](const KK& key)const
			{
				return values.Get(keys.IndexOf(key));
			}
			
			/// <summary>Replace the value associated to a specified key.</summary>
			/// <typeparam name="TKeyItem">The type of the new key.</typeparam>
			/// <typeparam name="TValueItem">The type of the new value.</typeparam>
			/// <returns>Returns true if the value is replaced.</returns>
			/// <param name="key">The key to find. If the key does not exist, it will be added to the dictionary.</param>
			/// <param name="value">The associated value to replace.</param>
			template<typename TKeyItem, typename TValueItem>
			bool Set(TKeyItem&& key, TValueItem&& value)
			{
				using TKeyAccept = memory_management::AcceptType<KT, TKeyItem&&>;
				using TKeyForward = memory_management::ForwardType<KT, TKeyItem&&>;
				TKeyAccept keyAccept = memory_management::RefOrConvert<KT>(std::forward<TKeyItem&&>(key));

				vint index = keys.IndexOf(KeyType<KT>::GetKeyValue(keyAccept));
				if (index == -1)
				{
					index = keys.Add(std::forward<TKeyForward>(keyAccept));
					values.Insert(index, std::forward<TValueItem&&>(value));
				}
				else
				{
					values[index] = std::forward<TValueItem&&>(value);
				}
				return true;
			}

			bool Set(const KT& key, const VT& value) { return Set<const KT&, const VT&>(key, value); }
			bool Set(const KT& key, VT&& value) { return Set<const KT&, VT>(key, std::move(value)); }
			bool Set(KT&& key, const VT& value) { return Set<KT, const VT&>(std::move(key), value); }
			bool Set(KT&& key, VT&& value) { return Set<KT, VT>(std::move(key), std::move(value)); }

			/// <summary>Add a key with an associated value.</summary>
			/// <returns>Returns true if the pair is added. If will crash if the key exists.</returns>
			/// <param name="value">The pair of key and value.</param>
			bool Add(const Pair<KT, VT>& value)
			{
				return Add(value.key, value.value);
			}

			/// <summary>Add a key with an associated value.</summary>
			/// <returns>Returns true if the pair is added. If will crash if the key exists.</returns>
			/// <param name="value">The pair of key and value.</param>
			bool Add(Pair<KT, VT>&& value)
			{
				return Add(std::move(value.key), std::move(value.value));
			}

			/// <summary>Add a key with an associated value.</summary>
			/// <typeparam name="TKeyItem">The type of the new key.</typeparam>
			/// <typeparam name="TValueItem">The type of the new value.</typeparam>
			/// <returns>Returns true if the pair is added. If will crash if the key exists.</returns>
			/// <param name="key">The key to add.</param>
			/// <param name="value">The value to add.</param>
			template<typename TKeyItem, typename TValueItem>
			bool Add(TKeyItem&& key, TValueItem&& value)
			{
				using TKeyAccept = memory_management::AcceptType<KT, TKeyItem&&>;
				using TKeyForward = memory_management::ForwardType<KT, TKeyItem&&>;
				TKeyAccept keyAccept = memory_management::RefOrConvert<KT>(std::forward<TKeyItem&&>(key));

				CHECK_ERROR(!keys.Contains(KeyType<KT>::GetKeyValue(keyAccept)), L"Dictionary<KT, KK, ValueContainer, VT, VK>::Add(const KT&, const VT&)#Key already exists.");
				vint index = keys.Add(std::forward<TKeyForward>(keyAccept));
				values.Insert(index, std::forward<TValueItem&&>(value));

				return true;
			}

			bool Add(const KT& key, const VT& value) { return Add<const KT&, const VT&>(key, value); }
			bool Add(const KT& key, VT&& value) { return Add<const KT&, VT>(key, std::move(value)); }
			bool Add(KT&& key, const VT& value) { return Add<KT, const VT&>(std::move(key), value); }
			bool Add(KT&& key, VT&& value) { return Add<KT, VT>(std::move(key), std::move(value)); }

			/// <summary>Remove a key with the associated value.</summary>
			/// <returns>Returns true if the key and the value is removed.</returns>
			/// <param name="key">The key to find.</param>
			bool Remove(const KK& key)
			{
				vint index=keys.IndexOf(key);
				if(index!=-1)
				{
					keys.RemoveAt(index);
					values.RemoveAt(index);
					return true;
				}
				else
				{
					return false;
				}
			}

			/// <summary>Remove all elements.</summary>
			/// <returns>Returns true if all elements are removed.</returns>
			bool Clear()
			{
				keys.Clear();
				values.Clear();
				return true;
			}
		};

		/// <summary>Group: one to many map container.</summary>
		/// <typeparam name="KT">Type of keys.</typeparam>
		/// <typeparam name="VT">Type of values.</typeparam>
		/// <typeparam name="KK">Type of the key type of keys. It is recommended to use the default value.</typeparam>
		/// <typeparam name="VK">Type of the key type of values. It is recommended to use the default value.</typeparam>
		template<
			typename KT,
			typename VT,
			typename KK=typename KeyType<KT>::Type,
			typename VK=typename KeyType<VT>::Type
		>
		class Group : public EnumerableBase<Pair<KT, VT>>
		{
		public:
			typedef SortedList<KT, KK>		KeyContainer;
			typedef List<VT, VK>			ValueContainer;
		protected:
			class Enumerator : public Object, public virtual IEnumerator<Pair<KT, VT>>
			{
			private:
				const Group<KT, VT, KK, VK>*		container;
				vint								keyIndex;
				vint								valueIndex;
				Pair<KT, VT>						current;

				void UpdateCurrent()
				{
					if(keyIndex<container->Count())
					{
						const ValueContainer& values=container->GetByIndex(keyIndex);
						if(valueIndex<values.Count())
						{
							current.key=container->Keys().Get(keyIndex);
							current.value=values.Get(valueIndex);
						}
					}
				}
			public:
				Enumerator(const Group<KT, VT, KK, VK>* _container, vint _keyIndex=-1, vint _valueIndex=-1)
				{
					container=_container;
					keyIndex=_keyIndex;
					valueIndex=_valueIndex;
				}
				
				IEnumerator<Pair<KT, VT>>* Clone()const override
				{
					return new Enumerator(container, keyIndex, valueIndex);
				}

				const Pair<KT, VT>& Current()const override
				{
					return current;
				}

				vint Index()const override
				{
					if(0<=keyIndex && keyIndex<container->Count())
					{
						vint index=0;
						for(vint i=0;i<keyIndex;i++)
						{
							index+=container->GetByIndex(i).Count();
						}
						return index+valueIndex;
					}
					else
					{
						return -1;
					}
				}

				bool Next() override
				{
					if(keyIndex==-1)
					{
						keyIndex=0;
					}
					while(keyIndex<container->Count())
					{
						valueIndex++;
						const ValueContainer& values=container->GetByIndex(keyIndex);
						if(valueIndex<values.Count())
						{
							UpdateCurrent();
							return true;
						}
						else
						{
							keyIndex++;
							valueIndex=-1;
						}
					}
					return false;
				}

				void Reset() override
				{
					keyIndex=-1;
					valueIndex=-1;
					UpdateCurrent();
				}

				bool Evaluated()const override
				{
					return true;
				}
			};

			KeyContainer					keys;
			List<ValueContainer*>			values;
		public:
			/// <summary>Create an empty group.</summary>
			Group() = default;

			~Group()
			{
				Clear();
			}

			Group(const Group<KT, VT, KK, VK>&) = delete;
			Group(Group<KT, VT, KK, VK> && _move)
				: keys(std::move(_move.keys))
				, values(std::move(_move.values))
			{
			}

			Group<KT, VT, KK, VK>& operator=(const Group<KT, VT, KK, VK>&) = delete;
			Group<KT, VT, KK, VK>& operator=(Group<KT, VT, KK, VK> && _move)
			{
				Clear();
				keys = std::move(_move.keys);
				values = std::move(_move.values);
				return*this;
			}

			IEnumerator<Pair<KT, VT>>* CreateEnumerator()const
			{
				return new Enumerator(this);
			}
			
			/// <summary>Get all keys.</summary>
			/// <returns>All keys.</returns>
			const KeyContainer& Keys()const
			{
				return keys;
			}
			
			/// <summary>Get the number of keys.</summary>
			/// <returns>The number of keys.</returns>
			vint Count()const
			{
				return keys.Count();
			}
			
			/// <summary>Get all values associated to a specified key.</summary>
			/// <returns>All associated values. It will crash if the key does not exist.</returns>
			/// <param name="key">The key to find.</param>
			const ValueContainer& Get(const KK& key)const
			{
				return *values.Get(keys.IndexOf(key));
			}
			
			/// <summary>Get all values associated to a key at a specified index in <see cref="Keys"/>.</summary>
			/// <returns>All associaged values. It will crash if the index is out of range.</returns>
			/// <param name="index">The position of the key.</param>
			const ValueContainer& GetByIndex(vint index)const
			{
				return *values.Get(index);
			}

			/// <summary>Get all values associated to a specified key.</summary>
			/// <returns>All associated values. It will crash if the key does not exist.</returns>
			/// <param name="key">The key to find.</param>
			const ValueContainer& operator[](const KK& key)const
			{
				return *values.Get(keys.IndexOf(key));
			}

			/// <summary>Test if there is any value associated to a specified key or not.</summary>
			/// <returns>Returns true there is at least one value associated to this key.</returns>
			/// <param name="key">The key to find.</param>
			bool Contains(const KK& key)const
			{
				return keys.Contains(key);
			}
			
			/// <summary>Test if a value is associated to a specified key or not.</summary>
			/// <returns>Returns true if the specified value is associated to the specified key.</returns>
			/// <param name="key">The key to find.</param>
			/// <param name="value">The value to find.</param>
			bool Contains(const KK& key, const VK& value)const
			{
				vint index=keys.IndexOf(key);
				if(index!=-1)
				{
					return values.Get(index)->Contains(value);
				}
				else
				{
					return false;
				}
			}
			
			/// <summary>
			/// Add a key with an associated value.
			/// If the key already exists, the value will be associated to the key with other values.
			/// If this value has already been associated to the key, it will still be duplicated.
			/// </summary>
			/// <returns>Returns true if the pair is added.</returns>
			/// <param name="value">The pair of key and value to add.</param>
			bool Add(const Pair<KT, VT>& value)
			{
				return Add(value.key, value.value);
			}

			/// <summary>
			/// Add a key with an associated value.
			/// If the key already exists, the value will be associated to the key with other values.
			/// If this value has already been associated to the key, it will still be duplicated.
			/// </summary>
			/// <returns>Returns true if the pair is added.</returns>
			/// <param name="value">The pair of key and value to add.</param>
			bool Add(Pair<KT, VT>&& value)
			{
				return Add(std::move(value.key), std::move(value.value));
			}

			/// <summary>
			/// Add a key with an associated value.
			/// If the key already exists, the value will be associated to the key with other values.
			/// If this value has already been associated to the key, it will still be duplicated.
			/// </summary>
			/// <typeparam name="TKeyItem">The type of the new key.</typeparam>
			/// <typeparam name="TValueItem">The type of the new value.</typeparam>
			/// <returns>Returns true if the key and the value are added.</returns>
			/// <param name="key">The key to add.</param>
			/// <param name="value">The value to add.</param>
			template<typename TKeyItem, typename TValueItem>
			bool Add(TKeyItem&& key, TValueItem&& value)
			{
				using TKeyAccept = memory_management::AcceptType<KT, TKeyItem&&>;
				using TKeyForward = memory_management::ForwardType<KT, TKeyItem&&>;
				TKeyAccept keyAccept = memory_management::RefOrConvert<KT>(std::forward<TKeyItem&&>(key));

				ValueContainer* target = nullptr;
				vint index = keys.IndexOf(KeyType<KT>::GetKeyValue(keyAccept));
				if (index == -1)
				{
					target = new ValueContainer;
					values.Insert(keys.Add(std::forward<TKeyForward>(keyAccept)), target);
				}
				else
				{
					target = values[index];
				}
				target->Add(std::forward<TValueItem&&>(value));
				return true;
			}

			bool Add(const KT& key, const VT& value) { return Add<const KT&, const VT&>(key, value); }
			bool Add(const KT& key, VT&& value) { return Add<const KT&, VT>(key, std::move(value)); }
			bool Add(KT&& key, const VT& value) { return Add<KT, const VT&>(std::move(key), value); }
			bool Add(KT&& key, VT&& value) { return Add<KT, VT>(std::move(key), std::move(value)); }
			
			/// <summary>Remove a key with all associated values.</summary>
			/// <returns>Returns true if the key and all associated values are removed.</returns>
			/// <param name="key">The key to find.</param>
			bool Remove(const KK& key)
			{
				vint index=keys.IndexOf(key);
				if(index!=-1)
				{
					keys.RemoveAt(index);
					List<VT, VK>* target=values[index];
					values.RemoveAt(index);
					delete target;
					return true;
				}
				else
				{
					return false;
				}
			}
			
			/// <summary>Remove a value associated to a specified key.</summary>
			/// <returns>
			/// Returns true if the value is removed.
			/// If this value is associated to the key for jultiple times, only the first one will be removed.
			/// If this value is associated to the key for jultiple times, only the first one will be removed.
			/// </returns>
			/// <param name="key">The key to find.</param>
			/// <param name="value">The value to remove.</param>
			bool Remove(const KK& key, const VK& value)
			{
				vint index=keys.IndexOf(key);
				if(index!=-1)
				{
					List<VT, VK>* target=values[index];
					target->Remove(value);
					if(target->Count()==0)
					{
						keys.RemoveAt(index);
						values.RemoveAt(index);
						delete target;
					}
					return true;
				}
				else
				{
					return false;
				}
			}
			
			/// <summary>Remove everything.</summary>
			/// <returns>Returns true if all keys and values are removed.</returns>
			bool Clear()
			{
				for(vint i=0;i<values.Count();i++)
				{
					delete values[i];
				}
				keys.Clear();
				values.Clear();
				return true;
			}
		};

/***********************************************************************
GroupInnerJoin
***********************************************************************/

		/// <summary>Perform inner join on two groups.</summary>
		/// <typeparam name="TKey">The type of keys in two groups.</typeparam>
		/// <typeparam name="TValueFirst">The type of values in the first group.</typeparam>
		/// <typeparam name="TValueSecond">The type of values in the second group.</typeparam>
		/// <typeparam name="TDiscardFirst">The type of the first callback.</typeparam>
		/// <typeparam name="TDiscardSecond">The type of the second callback.</typeparam>
		/// <typeparam name="TAccept">The type of the third callback.</typeparam>
		/// <param name="first">The first group.</param>
		/// <param name="second">The second group.</param>
		/// <param name="discardFirst">
		/// Callback that is called when a value in the first group is discarded.
		/// This happens for values associated to a key in the first group, that no value is assocated to the same key in the second group.
		/// The first argument is the key, the second argument is the discarded value list in the first group.
		/// </param>
		/// <param name="discardSecond">
		/// Callback that is called when a value in the second group is discarded.
		/// This happens for values associated to a key in the second group, that no value is assocated to the same key in the first group.
		/// The first argument is the key, the second argument is the discarded value list in the first group.
		/// </param>
		/// <param name="accept">
		/// Callback that is called when a match of values in both groups are found.
		/// This happens for any key that, values are associated to this key in both group.
		/// The first argument is the key, the second argument is the associated value list in the first group, the third argument list is the associated value in the second group.
		/// </param>
		/// <remarks>
		/// This function does not change data in provided groups.
		/// </remarks>
		/// <example><![CDATA[
		/// int main()
		/// {
		///     auto printList = [](const List<WString>& values)
		///     {
		///         return L"[" + From(values).Aggregate([](const WString& a, const WString& b) { return a + L", " + b; }) + L"]";
		///     };
		/// 
		///     Group<vint, WString> as, bs;
		///     as.Add(1 ,L"A"); as.Add(1 ,L"B"); as.Add(2 ,L"C"); as.Add(2 ,L"D");
		///     bs.Add(1 ,L"X"); bs.Add(1 ,L"Y"); bs.Add(3 ,L"Z"); bs.Add(3 ,L"W");
		///     GroupInnerJoin(
		///         as,
		///         bs,
		///         [&](vint key, const List<WString>& values) { Console::WriteLine(L"Discarded in as: " + itow(key) + printList(values)); },
		///         [&](vint key, const List<WString>& values) { Console::WriteLine(L"Discarded in bs: " + itow(key) + printList(values)); },
		///         [&](vint key, const List<WString>& values1, const List<WString>& values2) { Console::WriteLine(L"Accepted: " + itow(key) + printList(values1) + printList(values2)); }
		///         );
		/// }
		/// ]]></example>
		template<
			typename TKey,
			typename TValueFirst,
			typename TValueSecond,
			typename TDiscardFirst,		// TKey * [TValueFirst] -> void
			typename TDiscardSecond,	// TKey * [TValueSecond] -> void
			typename TAccept			// TKey * [TValueFirst] * [TValueSecond] -> void
		>
		void GroupInnerJoin(
			const Group<TKey, TValueFirst>& first,
			const Group<TKey, TValueSecond>& second,
			const TDiscardFirst& discardFirst,
			const TDiscardSecond& discardSecond,
			const TAccept& accept
			)
		{
			vint firstIndex = 0;
			vint secondIndex = 0;
			vint firstCount = first.Keys().Count();
			vint secondCount = second.Keys().Count();
			while (true)
			{
				if (firstIndex < firstCount)
				{
					auto firstKey = first.Keys()[firstIndex];
					const List<TValueFirst>& firstValues = first.GetByIndex(firstIndex);

					if (secondIndex < secondCount)
					{
						auto secondKey = second.Keys()[secondIndex];
						const List<TValueSecond>& secondValues = second.GetByIndex(secondIndex);

						if (firstKey < secondKey)
						{
							discardFirst(firstKey, firstValues);
							firstIndex++;
						}
						else if (firstKey > secondKey)
						{
							discardSecond(secondKey, secondValues);
							secondIndex++;
						}
						else
						{
							accept(firstKey, firstValues, secondValues);
							firstIndex++;
							secondIndex++;
						}
					}
					else
					{
						discardFirst(firstKey, firstValues);
						firstIndex++;
					}
				}
				else
				{
					if (secondIndex < secondCount)
					{
						auto secondKey = second.Keys()[secondIndex];
						const List<TValueSecond>& secondValues = second.GetByIndex(secondIndex);

						discardSecond(secondKey, secondValues);
						secondIndex++;
					}
					else
					{
						break;
					}
				}
			}
		}

/***********************************************************************
Random Access
***********************************************************************/
		namespace randomaccess_internal
		{
			template<typename KT, typename VT, typename KK, typename VK>
			struct RandomAccessable<Dictionary<KT, VT, KK, VK>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = false;
			};
		
			template<typename KT, typename VT, typename KK, typename VK>
			struct RandomAccess<Dictionary<KT, VT, KK, VK>>
			{
				static vint GetCount(const Dictionary<KT, VT, KK, VK>& t)
				{
					return t.Count();
				}

				static Pair<KT, VT> GetValue(const Dictionary<KT, VT, KK, VK>& t, vint index)
				{
					return Pair<KT, VT>(t.Keys().Get(index), t.Values().Get(index));
				}

				static void AppendValue(Dictionary<KT, VT, KK, VK>& t, const Pair<KT, VT>& value)
				{
					t.Set(value.key, value.value);
				}
			};
		}
	}
}

#endif