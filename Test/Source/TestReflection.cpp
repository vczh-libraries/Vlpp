#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Stream/FileStream.h"
#include "../../Source/Stream/Accessor.h"
#include "../../Source/Stream/CharFormat.h"
#include "../../Source/Collections/Operation.h"
#include "../../Source/Parsing/Xml/ParsingXml.h"
#include "../../Source/Parsing/Json/ParsingJson.h"
#include <limits>
#include <float.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::stream;
using namespace vl::parsing::xml;
using namespace vl::parsing::json;

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

#ifndef VCZH_DEBUG_NO_REFLECTION

namespace test
{
	template<typename T, typename TValue, vint Count, WString(*ToString)(TValue)>
	void TestNumber(TValue(&values)[Count], TValue min, TValue max, bool testGetText)
	{
		ITypeDescriptor* type=GetTypeDescriptor<T>();
		auto serializer = type->GetSerializableType();
		TEST_ASSERT(type);
		TEST_ASSERT(serializer);
		for(auto i:values)
		{
			if(min<=i && i<=max)
			{
				Value value;
				{
					TEST_ASSERT(serializer->Deserialize(ToString(i), value));
					TEST_ASSERT(value.GetValueType()==Value::BoxedValue);
					TEST_ASSERT(value.GetTypeDescriptor()==type);
					TEST_ASSERT(value.GetRawPtr()==0);
					TEST_ASSERT(value.GetSharedPtr().Obj()==0);

					WString output;
					TEST_ASSERT(serializer->Serialize(value, output));
					if (testGetText)
					{
						TEST_ASSERT(output == ToString(i));
					}
				}
				{
					T n = UnboxValue<T>(value);
#ifdef VCZH_MSVC
					TEST_ASSERT(n==(T)i);
#endif
				}
			}
			else
			{
				Value value;
				{
					TEST_ASSERT(serializer->Deserialize(ToString(i), value)==false);
					TEST_ASSERT(value.GetValueType()==Value::Null);
					TEST_ASSERT(value.GetTypeDescriptor()==0);
					TEST_ASSERT(value.GetRawPtr()==0);
					TEST_ASSERT(value.GetSharedPtr().Obj()==0);
				}
			}
		}
	}

	template<typename T, vint LegalCount, vint IllegalCount>
	void TestLiteral(WString legalsText[], WString illegalsText[], T legals[])
	{
		ITypeDescriptor* type=GetTypeDescriptor<T>();
		auto serializer = type->GetSerializableType();
		TEST_ASSERT(type);
		TEST_ASSERT(serializer);

		for(vint x=0;x<LegalCount;x++)
		{
			WString i=legalsText[x];
			T j=legals[x];
			Value value;
			{
				TEST_ASSERT(serializer->Deserialize(i, value));
				TEST_ASSERT(value.GetValueType() == Value::BoxedValue);
				TEST_ASSERT(value.GetTypeDescriptor() == type);
				TEST_ASSERT(value.GetRawPtr() == 0);
				TEST_ASSERT(value.GetSharedPtr().Obj() == 0);
			}
			{
				WString output;
				TEST_ASSERT(serializer->Serialize(value, output));
				TEST_ASSERT(i == output);
			}
			{
				T n = UnboxValue<T>(value);
				TEST_ASSERT(n == j);
			}
		}
		
		for(vint x=0;x<IllegalCount;x++)
		{
			WString i=illegalsText[x];
			Value value;
			{
				TEST_ASSERT(serializer->Deserialize(i, value) == false);
				TEST_ASSERT(value.GetValueType() == Value::Null);
				TEST_ASSERT(value.GetTypeDescriptor() == 0);
				TEST_ASSERT(value.GetRawPtr() == 0);
				TEST_ASSERT(value.GetSharedPtr().Obj() == 0);
			}
		}
	}

	template<typename T>
	void TestInt()
	{
		vint64_t values[]=
		{
			_I64_MIN,
			_I64_MIN+1,
			_I32_MIN,
			_I32_MIN+1,
			_I16_MIN,
			_I16_MIN+1,
			_I8_MIN,
			_I8_MIN+1,
			-1,
			0,
			1,
			_I8_MAX-1,
			_I8_MAX,
			_I16_MAX-1,
			_I16_MAX,
			_I32_MAX-1,
			_I32_MAX,
			_I64_MAX-1,
			_I64_MAX,
		};
		TestNumber<T, vint64_t, sizeof(values)/sizeof(*values), &i64tow>(values, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), true);
	}
	
	template<typename T>
	void TestUInt()
	{
		vuint64_t values[]=
		{
			0,
			1,
			_UI8_MAX-1,
			_UI8_MAX,
			_UI16_MAX-1,
			_UI16_MAX,
			_UI32_MAX-1,
			_UI32_MAX,
			_UI64_MAX-1,
			_UI64_MAX,
		};
		TestNumber<T, vuint64_t, sizeof(values)/sizeof(*values), &u64tow>(values, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), true);
	}

	template<typename T>
	void TestFloat()
	{
		double values[]=
		{
			-DBL_MAX,
			-FLT_MAX,
			-FLT_MIN,
			-DBL_MIN,
			-1,
			0,
			1,
			DBL_MIN,
			FLT_MIN,
			FLT_MAX,
			DBL_MAX,
		};
		TestNumber<T, double, sizeof(values)/sizeof(*values), &ftow>(values, -std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), false);
	}

	void TestBool()
	{
		WString legalsText[]={L"true", L"false"};
		WString illegalsText[]={L"TRUE", L"True", L"FALSE", L"False", L""};
		bool legals[]={true, false};
		TestLiteral<bool, sizeof(legalsText)/sizeof(*legalsText), sizeof(illegalsText)/sizeof(*illegalsText)>(legalsText, illegalsText, legals);
	}

	void TestString()
	{
		WString legals[]={L"a", L"b", L"c"};
		TestLiteral<WString, sizeof(legals)/sizeof(*legals), 0>(legals, 0, legals);
	}

	void TestDateTime()
	{
		WString legalsText[]={L"2000-01-01 00:00:00.000", L"2012-12-21 13:30:45.123", L"2013-06-11 14:30:00.000"};
		WString illegalsText[]={L"2000-01-01", L"00:00:00.000", L"2000-01-01 00:00:00"};
		DateTime legals[]={DateTime::FromDateTime(2000, 1, 1), DateTime::FromDateTime(2012, 12, 21, 13, 30, 45, 123), DateTime::FromDateTime(2013, 6, 11, 14, 30, 0)};
		TestLiteral<DateTime, sizeof(legalsText)/sizeof(*legalsText), sizeof(illegalsText)/sizeof(*illegalsText)>(legalsText, illegalsText, legals);
	}

	void TestLocale()
	{
		WString legalsText[]={L"zh-CN", L"en-US"};
		Locale legals[]={Locale(L"zh-CN"), Locale(L"en-US")};
		TestLiteral<Locale, sizeof(legalsText)/sizeof(*legalsText), 0>(legalsText, 0, legals);
	}
}
using namespace test;

TEST_CASE(TestReflectionPredefinedType)
{
	TEST_ASSERT(LoadPredefinedTypes());
	TestInt<vint8_t>();
	TestInt<vint16_t>();
	TestInt<vint32_t>();
	TestInt<vint64_t>();
	TestUInt<vuint8_t>();
	TestUInt<vuint16_t>();
	TestUInt<vuint32_t>();
	TestUInt<vuint64_t>();
	TestFloat<float>();
	TestFloat<double>();
	TestBool();
	TestString();
	TestDateTime();
	TestLocale();
	TEST_ASSERT(ResetGlobalTypeManager());
}

#endif

namespace test
{
	enum Season
	{
		Spring,
		Summer,
		Autumn,
		Winter,
	};

	enum ResetOption
	{
		ResetNone=0,
		ResetA=1,
		ResetB=2,
	};

	struct Point
	{
		vint x;
		vint y;
	};

	struct Size
	{
		vint cx;
		vint cy;
	};

	struct Rect
	{
		Point point;
		Size size;
	};

	struct RectPair
	{
		Rect a;
		Rect b;
	};

	class Base : public Object, public Description<Base>
	{
	public:
		vint a;
		Season season;
		Base():a(0), season(Spring){}
		Base(vint _a):a(_a){}
		static Ptr<Base> Create(vint _a, vint _b){return new Base(_a+_b);}
	};

	class Derived : public Base, public Description<Derived>
	{
	private:
		vint b;
	public:
		Derived():b(0){}
		Derived(vint _a, vint _b):Base(_a),b(_b){}
		static Ptr<Derived> Create(){return new Derived();}
		static Ptr<Derived> Create(vint _a, vint _b){return new Derived(_a, _b);}

		vint GetB(){return b;}
		void SetB(vint value){b=value;}
		void Reset(){a=0; b=0;}
		void Reset(vint _a, vint _b){a=_a; b=_b;}
		void Reset2(ResetOption opt){if(opt&ResetA) a=0; if(opt&ResetB) b=0;}
		static void Reset3(Derived* _this, Derived* derived){_this->a=derived->a; _this->b=derived->b;}
		
		Nullable<WString> c;
		Nullable<WString> GetC(){return c;}
		void SetC(Nullable<WString> value){c=value;}
	};

	class BaseSummer : public Description<BaseSummer>
	{
	protected:
		Array<Ptr<Base>>		bases;
	public:

		const Array<Ptr<Base>>& GetBases()
		{
			return bases;
		}

		void SetBases(const Array<Ptr<Base>>& _bases)
		{
			CopyFrom(bases, _bases);
		}

		vint Sum()
		{
			return From(bases)
				.Select([](const Ptr<Base>& base){return base->a;})
				.Aggregate((vint)0, [](vint a, vint b){return a+b;});
		}

		List<Ptr<Base>>			bases3;

		List<Ptr<Base>>& GetBases2()
		{
			return bases3;
		}

		void SetBases2(List<Ptr<Base>>& _bases)
		{
			CopyFrom(bases3, _bases);
		}

		vint Sum2()
		{
			return From(bases3)
				.Select([](const Ptr<Base>& base){return base->a;})
				.Aggregate((vint)0, [](vint a, vint b){return a+b;});
		}

		Func<vint(vint)> Sum3(Func<vint(vint)> f1, Func<vint(vint)> f2)
		{
			return [=](vint i){return f1(i)+f2(i);};
		}
	};

	class DictionaryHolder : public Description<DictionaryHolder>
	{
	public:
		Dictionary<vint, WString>				maps;
		Dictionary<Ptr<Base>, Ptr<Base>>		maps2;

		const Dictionary<vint, WString>& GetMaps()
		{ 
			return maps;
		}

		void SetMaps(const Dictionary<vint, WString>& value)
		{
			CopyFrom(maps, value);
		}
	};

	class EventRaiser : public Description<EventRaiser>
	{
	protected:
		vint									value;
	public:
		Event<void(vint, vint)>					ValueChanged;

		EventRaiser()
			:value(0)
		{
		}

		vint GetValue()
		{
			return value;
		}

		void SetValue(vint newValue)
		{
			vint oldValue = value;
			value = newValue;
			ValueChanged(oldValue, value);
		}
	};

#ifndef VCZH_DEBUG_NO_REFLECTION

	class Agg : public Description<Agg>
	{
	public:
		DescriptableObject* Root()
		{
			return GetAggregationRoot();
		}
	};

	class AggParentShared : public Agg, public AggregatableDescription<AggParentShared>
	{
	public:
		~AggParentShared()
		{
			FinalizeAggregation();
		}
	};

	class AggParentRaw : public Agg, public AggregatableDescription<AggParentRaw>
	{
	public:
		~AggParentRaw()
		{
			FinalizeAggregation();
		}
	};

	class AggParentBase : public Agg, public AggregatableDescription<AggParentBase>
	{
	public:
		AggParentBase()
		{
			Ptr<DescriptableObject> shared = new AggParentShared;
			auto raw = new AggParentRaw;

			InitializeAggregation(2);
			SetAggregationParent(0, shared);
			SetAggregationParent(1, raw);
		}

		~AggParentBase()
		{
			FinalizeAggregation();
		}

		AggParentShared* GetParentShared()
		{
			return dynamic_cast<AggParentShared*>(GetAggregationParent(0));
		}

		AggParentRaw* GetParentRaw()
		{
			return dynamic_cast<AggParentRaw*>(GetAggregationParent(1));
		}
	};

	class AggParentDerived : public Agg, public Description<AggParentDerived>
	{
	public:
		AggParentDerived()
		{
			auto base = new AggParentBase;

			InitializeAggregation(1);
			SetAggregationParent(0, base);
		}

		AggParentBase* GetParentBase()
		{
			return dynamic_cast<AggParentBase*>(GetAggregationParent(0));
		}
	};

	class MyList : public List<int>
	{
	};

	class HintTester :public Description<HintTester>
	{
	public:
		LazyList<int> GetLazyList(LazyList<int> x) { return x; }
		const Array<int>& GetArray(Array<int>& x) { return x; }
		const List<int>& GetList(List<int>& x) { return x; }
		const SortedList<int>& GetSortedList(SortedList<int>& x) { return x; }
		const ObservableList<vint>& GetReadableObservableList() { throw nullptr; }
		ObservableList<vint>& GetObservableList() { throw nullptr; }
		const Dictionary<int, int>& GetDictionary(Dictionary<int, int>& x) { return x; }
		const MyList& GetMyList(MyList& x) { return x; }
		Func<int(int)> GetFunc(Func<int(int)> x) { return x; }
		Ptr<HintTester> GetHintTester(Ptr<HintTester> x) { return x; }
		vint GetInt(vint x) { return x; }
	};

#endif
}
using namespace test;

#ifndef VCZH_DEBUG_NO_REFLECTION

#define _ ,

#define TYPE_LIST(F)\
	F(test::Season)\
	F(test::ResetOption)\
	F(test::Base)\
	F(test::Derived)\
	F(test::BaseSummer)\
	F(test::DictionaryHolder)\
	F(test::EventRaiser)\
	F(test::Point)\
	F(test::Size)\
	F(test::Rect)\
	F(test::RectPair)\
	F(test::Agg)\
	F(test::AggParentShared)\
	F(test::AggParentRaw)\
	F(test::AggParentBase)\
	F(test::AggParentDerived)\
	F(test::HintTester)\

BEGIN_TYPE_INFO_NAMESPACE

	TYPE_LIST(DECL_TYPE_INFO)
	TYPE_LIST(IMPL_CPP_TYPE_INFO)

	BEGIN_ENUM_ITEM(test::Season)
		ENUM_ITEM(Spring)
		ENUM_ITEM(Summer)
		ENUM_ITEM(Autumn)
		ENUM_ITEM(Winter)
	END_ENUM_ITEM(test::Season)

	BEGIN_ENUM_ITEM_MERGABLE(test::ResetOption)
		ENUM_ITEM(ResetNone)
		ENUM_ITEM(ResetA)
		ENUM_ITEM(ResetB)
	END_ENUM_ITEM(test::ResetOption)

	BEGIN_CLASS_MEMBER(test::Base)
		CLASS_MEMBER_FIELD(a)
		CLASS_MEMBER_FIELD(season)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<Base>(), NO_PARAMETER)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<Base>(vint), {L"_a"})
		CLASS_MEMBER_EXTERNALCTOR(Ptr<Base>(vint, vint), {L"_a" _ L"_b"}, test::Base::Create)
	END_CLASS_MEMBER(test::Base)

	BEGIN_CLASS_MEMBER(test::Derived)
		CLASS_MEMBER_BASE(Base)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<Derived>(), NO_PARAMETER)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<Derived>(vint _ vint), {L"_a" _ L"_b"})

		CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Create, NO_PARAMETER, Ptr<Derived>(*)())
		CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Create, {L"_a" _ L"_b"}, Ptr<Derived>(*)(vint _ vint))

		CLASS_MEMBER_METHOD(GetB, NO_PARAMETER)
		CLASS_MEMBER_METHOD(SetB, {L"value"})
		CLASS_MEMBER_PROPERTY(b, GetB, SetB)

		CLASS_MEMBER_METHOD_OVERLOAD(Reset, NO_PARAMETER, void(Derived::*)())
		CLASS_MEMBER_METHOD_OVERLOAD(Reset, {L"_a" _ L"_b"}, void(Derived::*)(vint _ vint))
		CLASS_MEMBER_METHOD_RENAME(Reset, Reset2, {L"opt"})
		CLASS_MEMBER_EXTERNALMETHOD(Reset, {L"derived"}, void(Derived::*)(Derived*), test::Derived::Reset3)

		CLASS_MEMBER_FIELD(c)
		CLASS_MEMBER_PROPERTY_FAST(C)
	END_CLASS_MEMBER(test::Derived)

	BEGIN_STRUCT_MEMBER(test::Point)
		STRUCT_MEMBER(x)
		STRUCT_MEMBER(y)
	END_STRUCT_MEMBER(test::Point)

	BEGIN_STRUCT_MEMBER(test::Size)
		STRUCT_MEMBER(cx)
		STRUCT_MEMBER(cy)
	END_STRUCT_MEMBER(test::Size)

	BEGIN_STRUCT_MEMBER(test::Rect)
		STRUCT_MEMBER(point)
		STRUCT_MEMBER(size)
	END_STRUCT_MEMBER(test::Rect)

	BEGIN_STRUCT_MEMBER(test::RectPair)
		STRUCT_MEMBER(a)
		STRUCT_MEMBER(b)
	END_STRUCT_MEMBER(test::RectPair)

	Ptr<IValueReadonlyList> BaseSummer_GetBases(BaseSummer* thisObject)
	{
		return new ValueReadonlyListWrapper<const Array<Ptr<Base>>*>(&thisObject->GetBases());
	}

	void BaseSummer_SetBases(BaseSummer* thisObject, Ptr<IValueReadonlyList> bases)
	{
		Array<Ptr<Base>> baseArray;
		CopyFrom(baseArray, GetLazyList<Ptr<Base>>(bases));
		thisObject->SetBases(baseArray);
	}

	BEGIN_CLASS_MEMBER(BaseSummer)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<BaseSummer>(), NO_PARAMETER)
		CLASS_MEMBER_METHOD(Sum, NO_PARAMETER)
		CLASS_MEMBER_METHOD(Sum2, NO_PARAMETER)
		CLASS_MEMBER_METHOD(Sum3, {L"f1" _ L"f2"})
		CLASS_MEMBER_METHOD(GetBases, NO_PARAMETER)
		CLASS_MEMBER_METHOD(SetBases, {L"bases"})
		CLASS_MEMBER_METHOD(GetBases2, NO_PARAMETER)
		CLASS_MEMBER_METHOD(SetBases2, {L"bases"})
		CLASS_MEMBER_FIELD(bases3)
	END_CLASS_MEMBER(BaseSummer)

	BEGIN_CLASS_MEMBER(DictionaryHolder)
		CLASS_MEMBER_FIELD(maps)
		CLASS_MEMBER_FIELD(maps2)
		CLASS_MEMBER_PROPERTY_FAST(Maps)
	END_CLASS_MEMBER(DictionaryHolder)

	BEGIN_CLASS_MEMBER(EventRaiser)
		CLASS_MEMBER_CONSTRUCTOR(Ptr<EventRaiser>(), NO_PARAMETER)

		CLASS_MEMBER_EVENT(ValueChanged)
		CLASS_MEMBER_PROPERTY_EVENT_FAST(Value, ValueChanged)
	END_CLASS_MEMBER(EventRaiser)

	BEGIN_CLASS_MEMBER(test::Agg)
	END_CLASS_MEMBER(test::Agg)

	BEGIN_CLASS_MEMBER(test::AggParentShared)
	END_CLASS_MEMBER(test::AggParentShared)

	BEGIN_CLASS_MEMBER(test::AggParentRaw)
	END_CLASS_MEMBER(test::AggParentRaw)

	BEGIN_CLASS_MEMBER(test::AggParentBase)
	END_CLASS_MEMBER(test::AggParentBase)

	BEGIN_CLASS_MEMBER(test::AggParentDerived)
	END_CLASS_MEMBER(test::AggParentDerived)

	BEGIN_CLASS_MEMBER(test::HintTester)
		CLASS_MEMBER_METHOD(GetLazyList, {L"x"})
		CLASS_MEMBER_METHOD(GetArray, { L"x" })
		CLASS_MEMBER_METHOD(GetList, { L"x" })
		CLASS_MEMBER_METHOD(GetSortedList, { L"x" })
		CLASS_MEMBER_METHOD(GetReadableObservableList, NO_PARAMETER)
		CLASS_MEMBER_METHOD(GetObservableList, NO_PARAMETER)
		CLASS_MEMBER_METHOD(GetDictionary, { L"x" })
		CLASS_MEMBER_METHOD(GetMyList, { L"x" })
		CLASS_MEMBER_METHOD(GetFunc, { L"x" })
		CLASS_MEMBER_METHOD(GetHintTester, { L"x" })
		CLASS_MEMBER_METHOD(GetInt, { L"x" })
	END_CLASS_MEMBER(test::HintTester)

	class TestTypeLoader : public Object, public ITypeLoader
	{
	public:
		void Load(ITypeManager* manager)override
		{
			TYPE_LIST(ADD_TYPE_INFO)
		}
		
		void Unload(ITypeManager* manager)override
		{
		}
	};

END_TYPE_INFO_NAMESPACE

#undef TYPE_LIST

#endif

TEST_CASE(TestDescriptableObjectReferenceCounterOperator)
{
	TEST_ASSERT((!AcceptValue<typename RequiresConvertable<vint, DescriptableObject>::YesNoType>::Result));
	TEST_ASSERT((AcceptValue<typename RequiresConvertable<DescriptableObject, DescriptableObject>::YesNoType>::Result));
	TEST_ASSERT((AcceptValue<typename RequiresConvertable<IDescriptable, DescriptableObject>::YesNoType>::Result));
	TEST_ASSERT((AcceptValue<typename RequiresConvertable<Base, DescriptableObject>::YesNoType>::Result));
	TEST_ASSERT((AcceptValue<typename RequiresConvertable<Derived, DescriptableObject>::YesNoType>::Result));

	Base* raw=new Base;
	volatile vint* counter=ReferenceCounterOperator<Base>::CreateCounter(raw);
	TEST_ASSERT(0==*counter);
	{
		Ptr<Base> ptr1=raw;
		TEST_ASSERT(1==*counter);
		{
			Ptr<Base> ptr2=raw;
			TEST_ASSERT(2==*counter);
			{
				Ptr<Base> ptr3=raw;
				TEST_ASSERT(3==*counter);
			}
			TEST_ASSERT(2==*counter);
		}
		TEST_ASSERT(1==*counter);
	}
}

#ifndef VCZH_DEBUG_NO_REFLECTION

TEST_CASE(TestBoxUnbox)
{
	{
		auto value = BoxValue<vint>(0);
		TEST_ASSERT(UnboxValue<vint>(value) == 0);
	}
	{
		auto value = BoxValue<WString>(L"abc");
		TEST_ASSERT(UnboxValue<WString>(value) == L"abc");
	}
	{
		Ptr<Base> base = MakePtr<Base>();
		{
			auto value = BoxValue<Base*>(base.Obj());
			TEST_ASSERT(UnboxValue<Base*>(value) == base.Obj());
			TEST_ASSERT(UnboxValue<Ptr<Base>>(value) == base);
		}
		{
			auto value = BoxValue<Ptr<Base>>(base);
			TEST_ASSERT(UnboxValue<Base*>(value) == base.Obj());
			TEST_ASSERT(UnboxValue<Ptr<Base>>(value) == base);
		}
	}
	{
		List<vint> numbers;
		numbers.Add(1);
		numbers.Add(2);
		numbers.Add(3);

		auto value = BoxParameter<List<vint>>(numbers);
		List<vint> numbers2;
		UnboxParameter(value, numbers2);

		TEST_ASSERT(numbers2.Count() == 3);
		TEST_ASSERT(numbers2[0] == 1);
		TEST_ASSERT(numbers2[1] == 2);
		TEST_ASSERT(numbers2[2] == 3);
	}
}

namespace reflection_test
{
	void TestReflectionBuilder()
	{
		FileStream fileStream(GetTestOutputPath() + L"Reflection.txt", FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf16);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);
		LogTypeManager(writer);

		TEST_ASSERT(GetTypeDescriptor<Value>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object);
		TEST_ASSERT(GetTypeDescriptor<vuint8_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vuint16_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vuint32_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vuint64_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vint8_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vint16_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vint32_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<vint64_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<float>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<double>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<wchar_t>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<WString>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<Locale>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<bool>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<DateTime>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct);
		TEST_ASSERT(GetTypeDescriptor<VoidValue>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Primitive);
		TEST_ASSERT(GetTypeDescriptor<IDescriptable>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::IDescriptable);
		TEST_ASSERT(GetTypeDescriptor<DescriptableObject>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class);
		TEST_ASSERT(GetTypeDescriptor<IValueEnumerator>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueEnumerable>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueReadonlyList>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueList>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueObservableList>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueReadonlyDictionary>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueDictionary>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueInterfaceProxy>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueFunctionProxy>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueSubscription>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueCallStack>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueException>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IValueType>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IEnumType>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<ISerializableType>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<ITypeInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<ITypeInfo::Decorator>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::NormalEnum);
		TEST_ASSERT(GetTypeDescriptor<IMemberInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IEventHandler>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IEventInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IPropertyInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IParameterInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IMethodInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<IMethodGroupInfo>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
		TEST_ASSERT(GetTypeDescriptor<ITypeDescriptor>()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface);
	}

	int MyFunc(int a, int b)
	{
		return a + b;
	}

	void TestReflectionInvoke()
	{
		{
			Value base=Value::Create(L"test::Base");
			TEST_ASSERT(base.GetTypeDescriptor());
			TEST_ASSERT(base.GetTypeDescriptor()->GetTypeName()==L"test::Base");
			TEST_ASSERT(UnboxValue<vint>(base.GetProperty(L"a"))==0);

			base.SetProperty(L"a", BoxValue<vint>(100));
			TEST_ASSERT(UnboxValue<vint>(base.GetProperty(L"a"))==100);

			Value base2=Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(200)));
			TEST_ASSERT(base2.GetTypeDescriptor());
			TEST_ASSERT(base2.GetTypeDescriptor()->GetTypeName()==L"test::Base");
			TEST_ASSERT(UnboxValue<vint>(base2.GetProperty(L"a"))==200);

			Value base3=Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(100), BoxValue<vint>(200)));
			TEST_ASSERT(base3.GetTypeDescriptor());
			TEST_ASSERT(base3.GetTypeDescriptor()->GetTypeName()==L"test::Base");
			TEST_ASSERT(UnboxValue<vint>(base3.GetProperty(L"a"))==300);
		}
		{
			Value derived=Value::Create(L"test::Derived");
			TEST_ASSERT(derived.GetTypeDescriptor());
			TEST_ASSERT(derived.GetTypeDescriptor()->GetTypeName()==L"test::Derived");
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==0);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==0);

			derived.SetProperty(L"a", BoxValue<vint>(100));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==100);
			derived.SetProperty(L"b", BoxValue<vint>(200));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==200);
		}
		{
			Value derived=Value::Create(L"test::Derived", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			TEST_ASSERT(derived.GetTypeDescriptor());
			TEST_ASSERT(derived.GetTypeDescriptor()->GetTypeName()==L"test::Derived");
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);

			derived.Invoke(L"Reset");
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==0);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==0);

			derived.Invoke(L"Reset", (Value_xs(), BoxValue<vint>(30), BoxValue<vint>(40)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==30);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==40);

			Ptr<Derived> d=UnboxValue<Ptr<Derived>>(derived);
			TEST_ASSERT(d->a==30);
			TEST_ASSERT(d->GetB()==40);
		
			Value derived2=Value::InvokeStatic(L"test::Derived", L"Create", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			derived2.Invoke(L"Reset", (Value_xs(), derived));
			TEST_ASSERT(UnboxValue<vint>(derived2.GetProperty(L"a"))==30);
			TEST_ASSERT(UnboxValue<vint>(derived2.GetProperty(L"b"))==40);
		}
	}

	void TestReflectionInvokeIndirect()
	{
		Value derived;
		{
			auto type = GetTypeDescriptor<Derived>();
			auto ctors = type->GetConstructorGroup();
			for (vint i = 0; i < ctors->GetMethodCount(); i++)
			{
				auto ctor = ctors->GetMethod(i);
				if (ctor->GetParameterCount() == 2)
				{
					auto proxy = ctor->CreateFunctionProxy(Value());

					auto xs = IValueList::Create();
					xs->Add(BoxValue<vint>(1));
					xs->Add(BoxValue<vint>(2));
					derived = proxy.Invoke(L"Invoke", (Value_xs(), Value::From(xs)));

					TEST_ASSERT(!derived.IsNull());
					TEST_ASSERT(UnboxValue<vint>(derived.Invoke(L"GetB", (Value_xs()))) == 2);
					break;
				}
			}
		}
		{
			auto proxy = derived.GetTypeDescriptor()->GetMethodGroupByName(L"SetB", true)->GetMethod(0)->CreateFunctionProxy(derived);
			{
				auto xs = IValueList::Create();
				xs->Add(BoxValue<vint>(3));
				proxy.Invoke(L"Invoke", (Value_xs(), Value::From(xs)));
			}
			TEST_ASSERT(UnboxValue<vint>(derived.Invoke(L"GetB", (Value_xs()))) == 3);
		}
	}

	void TestReflectionEvent()
	{
		Value eventRaiser = Value::Create(L"test::EventRaiser");
		vint oldValue = 0;
		vint newValue = 0;
		auto eventHandler = eventRaiser.AttachEvent(
			L"ValueChanged",
			BoxParameter<Func<void(vint, vint)>>(LAMBDA([&](vint _oldValue, vint _newValue)
			{
				oldValue = _oldValue;
				newValue = _newValue;
			})));
		TEST_ASSERT(eventHandler->IsAttached() == true);

		TEST_ASSERT(UnboxValue<vint>(eventRaiser.GetProperty(L"Value")) == 0);
		TEST_ASSERT(oldValue == 0);
		TEST_ASSERT(newValue == 0);

		eventRaiser.SetProperty(L"Value", BoxValue<vint>(100));
		TEST_ASSERT(UnboxValue<vint>(eventRaiser.GetProperty(L"Value")) == 100);
		TEST_ASSERT(oldValue == 0);
		TEST_ASSERT(newValue == 100);

		eventRaiser.SetProperty(L"Value", BoxValue<vint>(200));
		TEST_ASSERT(UnboxValue<vint>(eventRaiser.GetProperty(L"Value")) == 200);
		TEST_ASSERT(oldValue == 100);
		TEST_ASSERT(newValue == 200);

		TEST_ASSERT(eventRaiser.DetachEvent(L"ValueChanged", eventHandler) == true);
		TEST_ASSERT(eventHandler->IsAttached() == false);

		eventRaiser.SetProperty(L"Value", BoxValue<vint>(300));
		TEST_ASSERT(UnboxValue<vint>(eventRaiser.GetProperty(L"Value")) == 300);
		TEST_ASSERT(oldValue == 100);
		TEST_ASSERT(newValue == 200);

		TEST_ASSERT(eventRaiser.DetachEvent(L"ValueChanged", eventHandler) == false);
	}

	void TestReflectionEnum()
	{
		{
			Value base=Value::Create(L"test::Base");
			TEST_ASSERT(UnboxValue<Season>(base.GetProperty(L"season"))==Spring);

			base.SetProperty(L"season", BoxValue<Season>(Winter));
			TEST_ASSERT(UnboxValue<Season>(base.GetProperty(L"season"))==Winter);
		}
		{
			Value derived=Value::Create(L"test::Derived", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);

			derived.Invoke(L"Reset", (Value_xs(), BoxValue<test::ResetOption>(ResetNone)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);
		}
		{
			Value derived=Value::Create(L"test::Derived", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);

			derived.Invoke(L"Reset", (Value_xs(), BoxValue<test::ResetOption>(ResetA)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==0);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);
		}
		{
			Value derived=Value::Create(L"test::Derived", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);

			derived.Invoke(L"Reset", (Value_xs(), BoxValue<test::ResetOption>(ResetB)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==0);
		}
		{
			Value derived=Value::Create(L"test::Derived", (Value_xs(), BoxValue<vint>(10), BoxValue<vint>(20)));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==10);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==20);

			derived.Invoke(L"Reset", (Value_xs(), BoxValue<ResetOption>((ResetOption)(ResetA|ResetB))));
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"a"))==0);
			TEST_ASSERT(UnboxValue<vint>(derived.GetProperty(L"b"))==0);
		} 
	}

	void TestReflectionNullable()
	{
		{
			Value derived=Value::Create(L"test::Derived");
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"c"))==false);
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"C"))==false);

			derived.SetProperty(L"c", BoxValue(Nullable<WString>(L"vczh")));
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"c")).Value()==L"vczh");
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"C")).Value()==L"vczh");

			derived.SetProperty(L"C", BoxValue(Nullable<WString>()));
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"c"))==false);
			TEST_ASSERT(UnboxValue<Nullable<WString>>(derived.GetProperty(L"C"))==false);
		}
	}

	void TestReflectionStruct()
	{
		{
			Point point={10, 20};
			Value value=BoxValue<Point>(point);

			point=UnboxValue<Point>(value);
			TEST_ASSERT(point.x==10);
			TEST_ASSERT(point.y==20);
		}
		{
			Size size={10, 20};
			Value value=BoxValue<Size>(size);

			size=UnboxValue<Size>(value);
			TEST_ASSERT(size.cx==10);
			TEST_ASSERT(size.cy==20);
		}
		{
			Rect rect={{10, 20}, {30, 40}};
			Value value=BoxValue<Rect>(rect);

			rect=UnboxValue<Rect>(value);
			TEST_ASSERT(rect.point.x==10);
			TEST_ASSERT(rect.point.y==20);
			TEST_ASSERT(rect.size.cx==30);
			TEST_ASSERT(rect.size.cy==40);
		}
		{
			Rect a={{1, 2}, {3, 4}};
			Rect b = { { 10, 20 }, { 30, 40 } };
			RectPair rp={ a, b };
			Value value=BoxValue<RectPair>(rp);

			rp=UnboxValue<RectPair>(value);
			TEST_ASSERT(rp.a.point.x==1);
			TEST_ASSERT(rp.a.point.y==2);
			TEST_ASSERT(rp.a.size.cx==3);
			TEST_ASSERT(rp.a.size.cy==4);
			TEST_ASSERT(rp.b.point.x==10);
			TEST_ASSERT(rp.b.point.y==20);
			TEST_ASSERT(rp.b.size.cx==30);
			TEST_ASSERT(rp.b.size.cy==40);
		}
		{
			Point point = { 10, 20 };
			Value value = BoxValue<Point>(point);
			Value a = value;
			Value b;
			b = value;

			value.SetProperty(L"x", BoxValue<vint>(1));
			value.SetProperty(L"y", BoxValue<vint>(2));
			
			auto pa = UnboxValue<Point>(a);
			TEST_ASSERT(pa.x == 10);
			TEST_ASSERT(pa.y == 20);

			auto pb = UnboxValue<Point>(b);
			TEST_ASSERT(pb.x == 10);
			TEST_ASSERT(pb.y == 20);
		}
	}

	void TestReflectionList()
	{
		{
			Value bases=Value::Create(L"system::List");
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(1)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(2)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(3)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(4)))));

			Value baseSummer=Value::Create(L"test::BaseSummer");
			baseSummer.Invoke(L"SetBases", (Value_xs(), bases));
			TEST_ASSERT(UnboxValue<vint>(baseSummer.Invoke(L"Sum"))==10);

			Value baseArray=baseSummer.Invoke(L"GetBases");
			TEST_ASSERT(UnboxValue<vint>(baseArray.GetProperty(L"Count"))==4);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(0))).GetProperty(L"a"))==1);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(1))).GetProperty(L"a"))==2);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(2))).GetProperty(L"a"))==3);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(3))).GetProperty(L"a"))==4);
		}
		{
			Value bases=Value::Create(L"system::List");
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(1)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(2)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(3)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(4)))));

			Value baseSummer=Value::Create(L"test::BaseSummer");
			baseSummer.Invoke(L"SetBases2", (Value_xs(), bases));
			TEST_ASSERT(UnboxValue<vint>(baseSummer.Invoke(L"Sum2"))==10);

			Value baseArray=baseSummer.Invoke(L"GetBases2");
			TEST_ASSERT(UnboxValue<vint>(baseArray.GetProperty(L"Count"))==4);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(0))).GetProperty(L"a"))==1);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(1))).GetProperty(L"a"))==2);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(2))).GetProperty(L"a"))==3);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(3))).GetProperty(L"a"))==4);
		}
		{
			Value bases=Value::Create(L"system::List");
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(1)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(2)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(3)))));
			bases.Invoke(L"Add", (Value_xs(), Value::Create(L"test::Base", (Value_xs(), BoxValue<vint>(4)))));

			Value baseSummer=Value::Create(L"test::BaseSummer");
			baseSummer.SetProperty(L"bases3", bases);
			TEST_ASSERT(UnboxValue<vint>(baseSummer.Invoke(L"Sum2"))==10);

			Value baseArray=baseSummer.GetProperty(L"bases3");
			TEST_ASSERT(UnboxValue<vint>(baseArray.GetProperty(L"Count"))==4);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(0))).GetProperty(L"a"))==1);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(1))).GetProperty(L"a"))==2);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(2))).GetProperty(L"a"))==3);
			TEST_ASSERT(UnboxValue<vint>(baseArray.Invoke(L"Get", (Value_xs(), BoxValue<vint>(3))).GetProperty(L"a"))==4);
		}
		{
			Value baseSummer=Value::Create(L"test::BaseSummer");
			Value f1=BoxParameter<Func<vint(vint)>>(LAMBDA([](vint i){return i+1;}));
			Value f2=BoxParameter<Func<vint(vint)>>(LAMBDA([](vint i){return i+2;}));
			Value f=baseSummer.Invoke(L"Sum3", (Value_xs(), f1, f2));
			Func<vint(vint)> fx;
			UnboxParameter<Func<vint(vint)>>(f, fx);
			TEST_ASSERT(fx(10)==23);
		}
	}

	void TestReflectionDictionary()
	{
		Value map=Value::Create(L"system::Dictionary");
		map.Invoke(L"Set", (Value_xs(), BoxValue<vint>(1), BoxValue<vint>(1)));
		map.Invoke(L"Set", (Value_xs(), BoxValue<vint>(2), BoxValue<vint>(4)));
		map.Invoke(L"Set", (Value_xs(), BoxValue<vint>(3), BoxValue<vint>(9)));

		TEST_ASSERT(3==UnboxValue<vint>(map.GetProperty(L"Count")));
		TEST_ASSERT(1==UnboxValue<vint>(map.GetProperty(L"Keys").Invoke(L"Get", (Value_xs(), BoxValue<vint>(0)))));
		TEST_ASSERT(2==UnboxValue<vint>(map.GetProperty(L"Keys").Invoke(L"Get", (Value_xs(), BoxValue<vint>(1)))));
		TEST_ASSERT(3==UnboxValue<vint>(map.GetProperty(L"Keys").Invoke(L"Get", (Value_xs(), BoxValue<vint>(2)))));
		TEST_ASSERT(1==UnboxValue<vint>(map.GetProperty(L"Values").Invoke(L"Get", (Value_xs(), BoxValue<vint>(0)))));
		TEST_ASSERT(4==UnboxValue<vint>(map.GetProperty(L"Values").Invoke(L"Get", (Value_xs(), BoxValue<vint>(1)))));
		TEST_ASSERT(9==UnboxValue<vint>(map.GetProperty(L"Values").Invoke(L"Get", (Value_xs(), BoxValue<vint>(2)))));

		map.Invoke(L"Remove", (Value_xs(), BoxValue<vint>(2)));
		TEST_ASSERT(2==UnboxValue<vint>(map.GetProperty(L"Count")));
		TEST_ASSERT(1==UnboxValue<vint>(map.GetProperty(L"Keys").Invoke(L"Get", (Value_xs(), BoxValue<vint>(0)))));
		TEST_ASSERT(3==UnboxValue<vint>(map.GetProperty(L"Keys").Invoke(L"Get", (Value_xs(), BoxValue<vint>(1)))));
		TEST_ASSERT(1==UnboxValue<vint>(map.GetProperty(L"Values").Invoke(L"Get", (Value_xs(), BoxValue<vint>(0)))));
		TEST_ASSERT(9==UnboxValue<vint>(map.GetProperty(L"Values").Invoke(L"Get", (Value_xs(), BoxValue<vint>(1)))));

		map.Invoke(L"Clear", Value_xs());
		TEST_ASSERT(0==UnboxValue<vint>(map.GetProperty(L"Count")));
	}

	void TestSharedRawPtrConverting()
	{
		Base* b1=new Base;
		volatile vint* rc=ReferenceCounterOperator<Base>::CreateCounter(b1);
		TEST_ASSERT(*rc==0);

		Ptr<Base> b2=b1;
		TEST_ASSERT(*rc==1);

		Value v1=BoxValue(b1);
		TEST_ASSERT(v1.GetValueType()==Value::RawPtr);
		TEST_ASSERT(*rc==1);

		Value v2=BoxValue(b2);
		TEST_ASSERT(v2.GetValueType()==Value::SharedPtr);
		TEST_ASSERT(*rc==2);

		Base* b3=UnboxValue<Base*>(v2);
		TEST_ASSERT(b3==b1);
		TEST_ASSERT(*rc==2);

		Ptr<Base> b4=UnboxValue<Ptr<Base>>(v1);
		TEST_ASSERT(b4==b1);
		TEST_ASSERT(*rc==3);
	}

	void TestSharedRawPtrDestructing()
	{
		{
			Ptr<Base> b=new Base;
			Ptr<Object> o=b;
			b=0;
			o=0;
		}
		{
			//Base* b=new Base;
			//volatile vint* rc=ReferenceCounterOperator<Base>::CreateCounter(b);
			//TEST_ASSERT(*rc==0);

			//Ptr<Object> o=b;
			//TEST_ASSERT(*rc==1);
		}
	}

	class InterfaceProxy : public Object, public IValueInterfaceProxy
	{
	public:
		IMethodInfo* lastMethodInfo = nullptr;

		Value Invoke(IMethodInfo* methodInfo, Ptr<IValueList> arguments)
		{
			lastMethodInfo = methodInfo;
			return Value();
		}
	};

	void TestInterfaceProxy()
	{
		auto mock = MakePtr<InterfaceProxy>();
		Ptr<IValueEnumerable> proxy = ValueInterfaceProxy<IValueEnumerable>::Create(mock);
		proxy->CreateEnumerator();

		auto td = GetTypeDescriptor<IValueEnumerable>();
		auto methodInfo = td->GetMethodGroupByName(L"CreateEnumerator", false)->GetMethod(0);
		TEST_ASSERT(mock->lastMethodInfo == methodInfo);
	}

	void TestDescriptableObjectAggregation()
	{
		{
			auto derived = new AggParentDerived;
			auto base = derived->GetParentBase();
			auto shared = base->GetParentShared();
			auto raw = base->GetParentRaw();

			TEST_ASSERT(derived);
			TEST_ASSERT(base);
			TEST_ASSERT(shared);
			TEST_ASSERT(raw);

			TEST_ASSERT(derived->Root() == nullptr);
			TEST_ASSERT(base->Root() == derived);
			TEST_ASSERT(shared->Root() == derived);
			TEST_ASSERT(raw->Root() == derived);

			auto derivedRC = ReferenceCounterOperator<AggParentDerived>::CreateCounter(derived);
			auto baseRC = ReferenceCounterOperator<AggParentBase>::CreateCounter(base);
			auto sharedRC = ReferenceCounterOperator<AggParentShared>::CreateCounter(shared);
			auto rawRC = ReferenceCounterOperator<AggParentRaw>::CreateCounter(raw);

			TEST_ASSERT(*derivedRC == 0);
			TEST_ASSERT(derivedRC == baseRC);
			TEST_ASSERT(derivedRC == sharedRC);
			TEST_ASSERT(derivedRC == rawRC);

			Ptr<Agg> derivedPtr = derived;
			TEST_ASSERT(*derivedRC == 1);
			{
				Ptr<Agg> basePtr = base;
				TEST_ASSERT(*derivedRC == 2);
				Ptr<Agg> sharedPtr = shared;
				TEST_ASSERT(*derivedRC == 3);
				Ptr<Agg> rawPtr = raw;
				TEST_ASSERT(*derivedRC == 4);
			}
			TEST_ASSERT(*derivedRC == 1);
		}
		{
			auto derived = new AggParentDerived;
			delete derived;
		}
		{
			auto derived = new AggParentDerived;
			delete derived->GetParentBase();
		}
		{
			auto derived = new AggParentDerived;
			delete derived->GetParentBase()->GetParentShared();
		}
		{
			auto derived = new AggParentDerived;
			delete derived->GetParentBase()->GetParentRaw();
		}
		{
			auto derived = new AggParentDerived;
			Ptr<Agg> agg = derived;
		}
		{
			auto derived = new AggParentDerived;
			Ptr<Agg> agg = derived->GetParentBase();
		}
		{
			auto derived = new AggParentDerived;
			Ptr<Agg> agg = derived->GetParentBase()->GetParentShared();
		}
		{
			auto derived = new AggParentDerived;
			Ptr<Agg> agg = derived->GetParentBase()->GetParentRaw();
		}
	}

	void TestDescriptableObjectAggregationCast()
	{
		{
			auto derived = new AggParentDerived;
			auto base = derived->GetParentBase();
			auto shared = base->GetParentShared();
			auto raw = base->GetParentRaw();

			TEST_ASSERT(derived->SafeAggregationCast<AggParentDerived>() == derived);
			TEST_ASSERT(derived->SafeAggregationCast<AggParentBase>() == base);
			TEST_ASSERT(derived->SafeAggregationCast<AggParentShared>() == shared);
			TEST_ASSERT(derived->SafeAggregationCast<AggParentRaw>() == raw);

			TEST_ASSERT(base->SafeAggregationCast<AggParentDerived>() == derived);
			TEST_ASSERT(base->SafeAggregationCast<AggParentBase>() == base);
			TEST_ASSERT(base->SafeAggregationCast<AggParentShared>() == shared);
			TEST_ASSERT(derived->SafeAggregationCast<AggParentRaw>() == raw);

			TEST_ASSERT(shared->SafeAggregationCast<AggParentDerived>() == derived);
			TEST_ASSERT(shared->SafeAggregationCast<AggParentBase>() == base);
			TEST_ASSERT(shared->SafeAggregationCast<AggParentShared>() == shared);
			TEST_ASSERT(shared->SafeAggregationCast<AggParentRaw>() == raw);

			TEST_ASSERT(raw->SafeAggregationCast<AggParentDerived>() == derived);
			TEST_ASSERT(raw->SafeAggregationCast<AggParentBase>() == base);
			TEST_ASSERT(raw->SafeAggregationCast<AggParentShared>() == shared);
			TEST_ASSERT(raw->SafeAggregationCast<AggParentRaw>() == raw);

			TEST_ERROR(derived->SafeAggregationCast<Agg>());
			TEST_ERROR(base->SafeAggregationCast<Agg>());
			TEST_ERROR(shared->SafeAggregationCast<Agg>());
			TEST_ERROR(raw->SafeAggregationCast<Agg>());

			delete derived;
		}
		{
			auto derived = new AggParentDerived;
			auto base = derived->GetParentBase();
			auto shared = base->GetParentShared();
			auto raw = base->GetParentRaw();

			{
				auto value = Value::From(derived);
				TEST_ASSERT(UnboxValue<AggParentDerived*>(value) == derived);
				TEST_ASSERT(UnboxValue<AggParentBase*>(value) == base);
				TEST_ASSERT(UnboxValue<AggParentShared*>(value) == shared);
				TEST_ASSERT(UnboxValue<AggParentRaw*>(value) == raw);
				TEST_ERROR(UnboxValue<Agg*>(value));
			}
			{
				auto value = Value::From(base);
				TEST_ASSERT(UnboxValue<AggParentDerived*>(value) == derived);
				TEST_ASSERT(UnboxValue<AggParentBase*>(value) == base);
				TEST_ASSERT(UnboxValue<AggParentShared*>(value) == shared);
				TEST_ASSERT(UnboxValue<AggParentRaw*>(value) == raw);
				TEST_ERROR(UnboxValue<Agg*>(value));
			}
			{
				auto value = Value::From(shared);
				TEST_ASSERT(UnboxValue<AggParentDerived*>(value) == derived);
				TEST_ASSERT(UnboxValue<AggParentBase*>(value) == base);
				TEST_ASSERT(UnboxValue<AggParentShared*>(value) == shared);
				TEST_ASSERT(UnboxValue<AggParentRaw*>(value) == raw);
				TEST_ERROR(UnboxValue<Agg*>(value));
			}
			{
				auto value = Value::From(raw);
				TEST_ASSERT(UnboxValue<AggParentDerived*>(value) == derived);
				TEST_ASSERT(UnboxValue<AggParentBase*>(value) == base);
				TEST_ASSERT(UnboxValue<AggParentShared*>(value) == shared);
				TEST_ASSERT(UnboxValue<AggParentRaw*>(value) == raw);
				TEST_ERROR(UnboxValue<Agg*>(value));
			}

			delete derived;
		}
		{
			auto derived = new AggParentDerived;
			auto base = derived->GetParentBase();
			auto shared = base->GetParentShared();
			auto raw = base->GetParentRaw();

			Ptr<Agg> ptr = derived;
			{
				auto value = Value::From(ptr);
				TEST_ASSERT(UnboxValue<Ptr<AggParentDerived>>(value) == derived);
				TEST_ASSERT(UnboxValue<Ptr<AggParentBase>>(value) == base);
				TEST_ASSERT(UnboxValue<Ptr<AggParentShared>>(value) == shared);
				TEST_ASSERT(UnboxValue<Ptr<AggParentRaw>>(value) == raw);
				TEST_ERROR(UnboxValue<Ptr<Agg>>(value));
			}
			{
				auto value = Value::From(ptr);
				TEST_ASSERT(UnboxValue<Ptr<AggParentDerived>>(value) == derived);
				TEST_ASSERT(UnboxValue<Ptr<AggParentBase>>(value) == base);
				TEST_ASSERT(UnboxValue<Ptr<AggParentShared>>(value) == shared);
				TEST_ASSERT(UnboxValue<Ptr<AggParentRaw>>(value) == raw);
				TEST_ERROR(UnboxValue<Ptr<Agg>>(value));
			}
			{
				auto value = Value::From(ptr);
				TEST_ASSERT(UnboxValue<Ptr<AggParentDerived>>(value) == derived);
				TEST_ASSERT(UnboxValue<Ptr<AggParentBase>>(value) == base);
				TEST_ASSERT(UnboxValue<Ptr<AggParentShared>>(value) == shared);
				TEST_ASSERT(UnboxValue<Ptr<AggParentRaw>>(value) == raw);
				TEST_ERROR(UnboxValue<Ptr<Agg>>(value));
			}
			{
				auto value = Value::From(ptr);
				TEST_ASSERT(UnboxValue<Ptr<AggParentDerived>>(value) == derived);
				TEST_ASSERT(UnboxValue<Ptr<AggParentBase>>(value) == base);
				TEST_ASSERT(UnboxValue<Ptr<AggParentShared>>(value) == shared);
				TEST_ASSERT(UnboxValue<Ptr<AggParentRaw>>(value) == raw);
				TEST_ERROR(UnboxValue<Ptr<Agg>>(value));
			}
		}
	}

	void TestDescriptableObjectIsAggregation()
	{
		TEST_ASSERT(GetTypeDescriptor<AggParentShared>()->IsAggregatable() == true);
		TEST_ASSERT(GetTypeDescriptor<AggParentRaw>()->IsAggregatable() == true);
		TEST_ASSERT(GetTypeDescriptor<AggParentBase>()->IsAggregatable() == true);
		TEST_ASSERT(GetTypeDescriptor<AggParentDerived>()->IsAggregatable() == false);
	}

	void TestTypeInfoFriendlyName()
	{
		{
			auto typeInfo = TypeInfoRetriver<void>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"system::Void");
		}
		{
			auto typeInfo = TypeInfoRetriver<Nullable<vint32_t>>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"system::Int32?");
		}
		{
			auto typeInfo = TypeInfoRetriver<Base*>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"test::Base*");
		}
		{
			auto typeInfo = TypeInfoRetriver<Ptr<Base>>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"test::Base^");
		}
		{
			auto typeInfo = TypeInfoRetriver<List<Ptr<Base>>>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"system::List<test::Base^>^");
		}
		{
			auto typeInfo = TypeInfoRetriver<Func<void(vint32_t)>>::CreateTypeInfo();
			TEST_ASSERT(typeInfo->GetTypeFriendlyName() == L"system::Function<system::Void, system::Int32>^");
		}
	}

	void TestCpp()
	{
		{
			auto td = GetTypeDescriptor<void>();
			TEST_ASSERT(CppExists(td));
			TEST_ASSERT(CppGetFullName(td) == L"void");
		}
		{
			auto td = GetTypeDescriptor<vint32_t>();
			TEST_ASSERT(CppExists(td));
			TEST_ASSERT(CppGetFullName(td) == L"::vl::vint32_t");
		}
		{
			auto td = GetTypeDescriptor<IValueEnumerable>();
			TEST_ASSERT(CppExists(td));
			TEST_ASSERT(CppGetFullName(td) == L"::vl::reflection::description::IValueEnumerable");
		}
		{
			auto td = GetTypeDescriptor<Base>();
			TEST_ASSERT(CppExists(td));
			TEST_ASSERT(CppGetFullName(td) == L"::test::Base");
		}

		{
			auto td = GetTypeDescriptor<Point>();
			auto prop = td->GetPropertyByName(L"x", false);
			TEST_ASSERT(CppExists(prop));
			TEST_ASSERT(CppGetReferenceTemplate(prop) == L"$This.$Name");
		}
		{
			auto td = GetTypeDescriptor<Base>();
			auto prop = td->GetPropertyByName(L"a", false);
			TEST_ASSERT(CppExists(prop));
			TEST_ASSERT(CppGetReferenceTemplate(prop) == L"$This->$Name");
		}
		{
			auto td = GetTypeDescriptor<Derived>();
			auto prop = td->GetPropertyByName(L"b", false);
			TEST_ASSERT(CppExists(prop));
		}
		{
			auto td = GetTypeDescriptor<XmlElement>();
			auto prop = td->GetPropertyByName(L"name", false);
			TEST_ASSERT(CppExists(prop));
			TEST_ASSERT(CppGetReferenceTemplate(prop) == L"$This->$Name.value");
		}

		{
			auto td = GetTypeDescriptor<Base>();
			{
				auto ctor = td->GetConstructorGroup()->GetMethod(0);
				TEST_ASSERT(ctor->GetParameterCount() == 0);
				TEST_ASSERT(CppExists(ctor));
				TEST_ASSERT(CppGetInvokeTemplate(ctor) == L"new $Type($Arguments)");
			}
			{
				auto ctor = td->GetConstructorGroup()->GetMethod(1);
				TEST_ASSERT(ctor->GetParameterCount() == 1);
				TEST_ASSERT(CppExists(ctor));
				TEST_ASSERT(CppGetInvokeTemplate(ctor) == L"new $Type($Arguments)");
			}
			{
				auto ctor = td->GetConstructorGroup()->GetMethod(2);
				TEST_ASSERT(ctor->GetParameterCount() == 2);
				TEST_ASSERT(CppExists(ctor));
				TEST_ASSERT(CppGetInvokeTemplate(ctor) == L"::test::Base::Create($Arguments)");
			}
		}

		{
			auto td = GetTypeDescriptor<Derived>();
			{
				auto method = td->GetMethodGroupByName(L"Create", false)->GetMethod(0);
				TEST_ASSERT(method->GetParameterCount() == 0);
				TEST_ASSERT(CppExists(method));
				TEST_ASSERT(CppGetInvokeTemplate(method) == L"$Type::$Name($Arguments)");
			}
			{
				auto method = td->GetMethodGroupByName(L"Reset", false)->GetMethod(0);
				TEST_ASSERT(method->GetParameterCount() == 0);
				TEST_ASSERT(CppExists(method));
				TEST_ASSERT(CppGetInvokeTemplate(method) == L"$This->$Name($Arguments)");
			}
			{
				auto method = td->GetMethodGroupByName(L"Reset", false)->GetMethod(1);
				TEST_ASSERT(method->GetParameterCount() == 2);
				TEST_ASSERT(CppExists(method));
				TEST_ASSERT(CppGetInvokeTemplate(method) == L"$This->$Name($Arguments)");
			}
			{
				auto method = td->GetMethodGroupByName(L"Reset", false)->GetMethod(2);
				TEST_ASSERT(method->GetParameterCount() == 1);
				TEST_ASSERT(CppExists(method));
				TEST_ASSERT(CppGetInvokeTemplate(method) == L"$This->Reset2($Arguments)");
			}
			{
				auto method = td->GetMethodGroupByName(L"Reset", false)->GetMethod(3);
				TEST_ASSERT(method->GetParameterCount() == 1);
				TEST_ASSERT(CppExists(method));
				TEST_ASSERT(CppGetInvokeTemplate(method) == L"::test::Derived::Reset3($This, $Arguments)");
			}
		}
		{
			auto td = GetTypeDescriptor<ITypeDescriptor>();
			auto method = td->GetMethodGroupByName(L"GetTypeDescriptorCount", false)->GetMethod(0);
			TEST_ASSERT(CppExists(method));
			TEST_ASSERT(CppGetInvokeTemplate(method) == L"::vl::reflection::description::ITypeDescriptor_GetTypeDescriptorCount($Arguments)");
		}
		{
			auto td = GetTypeDescriptor<XmlElement>();
			auto method = td->GetMethodGroupByName(L"get_name", false)->GetMethod(0);
			TEST_ASSERT(!CppExists(method));
		}
	}

	template<typename TReturn, typename TArgument>
	void TestHint(const WString& member, TypeInfoHint hint, bool testParameter = true)
	{
		auto td = GetTypeDescriptor<HintTester>();
		auto method = td->GetMethodGroupByName(member, false)->GetMethod(0);
		TEST_ASSERT(method->GetReturn()->GetTypeDescriptor() == GetTypeDescriptor<TReturn>());
		TEST_ASSERT(method->GetReturn()->GetHint() == hint);

		if (testParameter)
		{
			TEST_ASSERT(method->GetParameterCount() == 1);
			TEST_ASSERT(method->GetParameter(0)->GetType()->GetTypeDescriptor() == GetTypeDescriptor<TArgument>());
			TEST_ASSERT(method->GetParameter(0)->GetType()->GetHint() == hint);
		}
	}

	void TestHint()
	{
		TestHint<IValueEnumerable, IValueEnumerable>(L"GetLazyList", TypeInfoHint::LazyList);
		TestHint<IValueReadonlyList, IValueList>(L"GetArray", TypeInfoHint::Array);
		TestHint<IValueReadonlyList, IValueList>(L"GetList", TypeInfoHint::List);
		TestHint<IValueReadonlyList, IValueList>(L"GetSortedList", TypeInfoHint::SortedList);
		TestHint<IValueReadonlyList, void>(L"GetReadableObservableList", TypeInfoHint::ObservableList, false);
		TestHint<IValueObservableList, void>(L"GetObservableList", TypeInfoHint::ObservableList, false);
		TestHint<IValueReadonlyDictionary, IValueDictionary>(L"GetDictionary", TypeInfoHint::Dictionary);
		TestHint<IValueReadonlyList, IValueList>(L"GetMyList", TypeInfoHint::NativeCollectionReference);
		TestHint<IValueFunctionProxy, IValueFunctionProxy>(L"GetFunc", TypeInfoHint::Normal);
		TestHint<HintTester, HintTester>(L"GetHintTester", TypeInfoHint::Normal);
		TestHint<vint, vint>(L"GetInt", TypeInfoHint::Normal);
	}
}
using namespace reflection_test;

#define TEST_CASE_REFLECTION(NAME)\
	TEST_CASE(NAME)\
	{\
		TEST_ASSERT(LoadPredefinedTypes());\
		TEST_ASSERT(XmlLoadTypes());\
		TEST_ASSERT(JsonLoadTypes());\
		TEST_ASSERT(GetGlobalTypeManager()->AddTypeLoader(new TestTypeLoader));\
		TEST_ASSERT(GetGlobalTypeManager()->Load());\
		{\
			NAME();\
		}\
		TEST_ASSERT(ResetGlobalTypeManager());\
	}\

TEST_CASE_REFLECTION(TestReflectionBuilder)
TEST_CASE_REFLECTION(TestReflectionInvoke)
TEST_CASE_REFLECTION(TestReflectionInvokeIndirect)
TEST_CASE_REFLECTION(TestReflectionEvent)
TEST_CASE_REFLECTION(TestReflectionEnum)
TEST_CASE_REFLECTION(TestReflectionNullable)
TEST_CASE_REFLECTION(TestReflectionStruct)
TEST_CASE_REFLECTION(TestReflectionList)
TEST_CASE_REFLECTION(TestReflectionDictionary)
TEST_CASE_REFLECTION(TestSharedRawPtrConverting)
TEST_CASE_REFLECTION(TestSharedRawPtrDestructing)
TEST_CASE_REFLECTION(TestInterfaceProxy)
TEST_CASE_REFLECTION(TestDescriptableObjectAggregation)
TEST_CASE_REFLECTION(TestDescriptableObjectAggregationCast)
TEST_CASE_REFLECTION(TestDescriptableObjectIsAggregation)
TEST_CASE_REFLECTION(TestTypeInfoFriendlyName)
TEST_CASE_REFLECTION(TestCpp)
TEST_CASE_REFLECTION(TestHint)

#endif
