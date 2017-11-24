#include "GuiTypeDescriptorMacros.h"
#include "GuiTypeDescriptorReflection.h"
#include "../Threading.h"

namespace vl
{
	using namespace collections;

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
IValueEnumerable
***********************************************************************/

			Ptr<IValueEnumerable> IValueEnumerable::Create(collections::LazyList<Value> values)
			{
				Ptr<IEnumerable<Value>> enumerable = new LazyList<Value>(values);
				return new ValueEnumerableWrapper<Ptr<IEnumerable<Value>>>(enumerable);
			}

/***********************************************************************
IValueList
***********************************************************************/

			Ptr<IValueList> IValueList::Create()
			{
				return Create(LazyList<Value>());
			}

			Ptr<IValueList> IValueList::Create(Ptr<IValueReadonlyList> values)
			{
				return Create(GetLazyList<Value>(values));
			}

			Ptr<IValueList> IValueList::Create(collections::LazyList<Value> values)
			{
				Ptr<List<Value>> list = new List<Value>;
				CopyFrom(*list.Obj(), values);
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

/***********************************************************************
IObservableList
***********************************************************************/

			class ReversedObservableList : public ObservableListBase<Value>
			{
			protected:

				void NotifyUpdateInternal(vint start, vint count, vint newCount)override
				{
					if (observableList)
					{
						observableList->ItemChanged(start, count, newCount);
					}
				}
			public:
				IValueObservableList*		observableList = nullptr;
			};

			Ptr<IValueObservableList> IValueObservableList::Create()
			{
				return Create(LazyList<Value>());
			}

			Ptr<IValueObservableList> IValueObservableList::Create(Ptr<IValueReadonlyList> values)
			{
				return Create(GetLazyList<Value>(values));
			}

			Ptr<IValueObservableList> IValueObservableList::Create(collections::LazyList<Value> values)
			{
				auto list = MakePtr<ReversedObservableList>();
				CopyFrom(*list.Obj(), values);
				auto wrapper = MakePtr<ValueObservableListWrapper<Ptr<ReversedObservableList>>>(list);
				list->observableList = wrapper.Obj();
				return wrapper;
			}

/***********************************************************************
IValueDictionary
***********************************************************************/

			Ptr<IValueDictionary> IValueDictionary::Create()
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(Ptr<IValueReadonlyDictionary> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), GetLazyList<Value, Value>(values));
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(collections::LazyList<collections::Pair<Value, Value>> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), values);
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

/***********************************************************************
IValueException
***********************************************************************/

			class DefaultValueException : public Object, public IValueException
			{
			protected:
				WString				message;

			public:
				DefaultValueException(const WString& _message)
					:message(_message)
				{
				}

#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				WString GetMessage()override
				{
					return message;
				}
#pragma pop_macro("GetMessage")

				bool GetFatal()override
				{
					return false;
				}

				Ptr<IValueReadonlyList> GetCallStack()override
				{
					return nullptr;
				}
			};

			Ptr<IValueException> IValueException::Create(const WString& message)
			{
				return new DefaultValueException(message);
			}
		}
	}
}
