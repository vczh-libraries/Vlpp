/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

XML Representation for Code Generation:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORPREDEFINED
#define VCZH_REFLECTION_GUITYPEDESCRIPTORPREDEFINED

#include <math.h>
#include "GuiTypeDescriptor.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
TypeInfo
***********************************************************************/

#define DECL_TYPE_INFO(TYPENAME) template<>struct TypeInfo<TYPENAME>{ static const TypeInfoContent content; };
#define IMPL_VL_TYPE_INFO(TYPENAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #TYPENAME, nullptr, TypeInfoContent::VlppType };
#define IMPL_CPP_TYPE_INFO(TYPENAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #TYPENAME, nullptr, TypeInfoContent::CppType };
#define IMPL_TYPE_INFO_RENAME(TYPENAME, EXPECTEDNAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #EXPECTEDNAME, L ## #TYPENAME, TypeInfoContent::Renamed };

			struct TypeInfoContent
			{
				enum TypeInfoCppName
				{
					VlppType,			// vl::<type-name>
					CppType,			// <type-name>
					Renamed,			// CppFullTypeName
				};

				const wchar_t*		typeName;
				const wchar_t*		cppFullTypeName;
				TypeInfoCppName		cppName;
			};

			template<typename T>
			struct TypeInfo
			{
			};

			template<typename T>
			ITypeDescriptor* GetTypeDescriptor()
			{
				return GetTypeDescriptor(TypeInfo<T>::content.typeName);
			}

			template<typename T>
			struct TypedValueSerializerProvider
			{
			};

/***********************************************************************
SerializableTypeDescriptor
***********************************************************************/

			class TypeDescriptorImplBase : public Object, public ITypeDescriptor, private ITypeDescriptor::ICpp
			{
			private:
				TypeDescriptorFlags							typeDescriptorFlags;
				const TypeInfoContent*						typeInfoContent;
				WString										typeName;
				WString										cppFullTypeName;

				const WString&								GetFullName()override;

			protected:
				const TypeInfoContent*						GetTypeInfoContentInternal();

			public:
				TypeDescriptorImplBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent);
				~TypeDescriptorImplBase();

				ITypeDescriptor::ICpp*						GetCpp()override;
				TypeDescriptorFlags							GetTypeDescriptorFlags()override;
				const WString&								GetTypeName()override;
			};

			class ValueTypeDescriptorBase : public TypeDescriptorImplBase
			{
			protected:
				bool										loaded;
				Ptr<IValueType>								valueType;
				Ptr<IEnumType>								enumType;
				Ptr<ISerializableType>						serializableType;

				virtual void								LoadInternal();;
				void										Load();
			public:
				ValueTypeDescriptorBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent);
				~ValueTypeDescriptorBase();

				bool										IsAggregatable()override;
				IValueType*									GetValueType()override;
				IEnumType*									GetEnumType()override;
				ISerializableType*							GetSerializableType()override;

				vint										GetBaseTypeDescriptorCount()override;
				ITypeDescriptor*							GetBaseTypeDescriptor(vint index)override;
				bool										CanConvertTo(ITypeDescriptor* targetType)override;
				vint										GetPropertyCount()override;
				IPropertyInfo*								GetProperty(vint index)override;
				bool										IsPropertyExists(const WString& name, bool inheritable)override;
				IPropertyInfo*								GetPropertyByName(const WString& name, bool inheritable)override;
				vint										GetEventCount()override;
				IEventInfo*									GetEvent(vint index)override;
				bool										IsEventExists(const WString& name, bool inheritable)override;
				IEventInfo*									GetEventByName(const WString& name, bool inheritable)override;
				vint										GetMethodGroupCount()override;
				IMethodGroupInfo*							GetMethodGroup(vint index)override;
				bool										IsMethodGroupExists(const WString& name, bool inheritable)override;
				IMethodGroupInfo*							GetMethodGroupByName(const WString& name, bool inheritable)override;
				IMethodGroupInfo*							GetConstructorGroup()override;
			};

			template<typename T, TypeDescriptorFlags TDFlags>
			class TypedValueTypeDescriptorBase : public ValueTypeDescriptorBase
			{
			public:
				TypedValueTypeDescriptorBase()
					:ValueTypeDescriptorBase(TDFlags, &TypeInfo<T>::content)
				{
				}
			};

/***********************************************************************
Predefined Libraries
***********************************************************************/

			class Sys : public Description<Sys>
			{
			public:
				static vint			Len(const WString& value)							{ return value.Length(); }
				static WString		Left(const WString& value, vint length)				{ return value.Left(length); }
				static WString		Right(const WString& value, vint length)			{ return value.Right(length); }
				static WString		Mid(const WString& value, vint start, vint length)	{ return value.Sub(start, length); }
				static vint			Find(const WString& value, const WString& substr)	{ return INVLOC.FindFirst(value, substr, Locale::Normalization::None).key; }
			};

			class Math : public Description<Math>
			{
			public:
				static vint8_t		Abs(vint8_t value)				{ return value > 0 ? value : -value; }
				static vint16_t		Abs(vint16_t value)				{ return value > 0 ? value : -value; }
				static vint32_t		Abs(vint32_t value)				{ return value > 0 ? value : -value; }
				static vint64_t		Abs(vint64_t value)				{ return value > 0 ? value : -value; }
				static float		Abs(float value)				{ return value > 0 ? value : -value; }
				static double		Abs(double value)				{ return value > 0 ? value : -value; }

				static vint8_t		Max(vint8_t a, vint8_t b)		{ return a > b ? a : b; }
				static vint16_t		Max(vint16_t a, vint16_t b)		{ return a > b ? a : b; }
				static vint32_t		Max(vint32_t a, vint32_t b)		{ return a > b ? a : b; }
				static vint64_t		Max(vint64_t a, vint64_t b)		{ return a > b ? a : b; }
				static float		Max(float a, float b)			{ return a > b ? a : b; }
				static double		Max(double a, double b)			{ return a > b ? a : b; }

				static vint8_t		Min(vint8_t a, vint8_t b)		{ return a < b ? a : b; }
				static vint16_t		Min(vint16_t a, vint16_t b)		{ return a < b ? a : b; }
				static vint32_t		Min(vint32_t a, vint32_t b)		{ return a < b ? a : b; }
				static vint64_t		Min(vint64_t a, vint64_t b)		{ return a < b ? a : b; }
				static float		Min(float a, float b)			{ return a < b ? a : b; }
				static double		Min(double a, double b)			{ return a < b ? a : b; }

				static double		Sin(double value)				{ return sin(value); }
				static double		Cos(double value)				{ return cos(value); }
				static double		Tan(double value)				{ return tan(value); }
				static double		ASin(double value)				{ return asin(value); }
				static double		ACos(double value)				{ return acos(value); }
				static double		ATan(double value)				{ return atan(value); }
				static double		ATan2(double x, double y)		{ return atan2(y, x); }

				static double		Exp(double value)				{ return exp(value);  }
				static double		LogN(double value)				{ return log(value); }
				static double		Log10(double value)				{ return log10(value); }
				static double		Log(double value, double base)	{ return log(value) / log(base); }
				static double		Pow(double value, double power)	{ return pow(value, power); }
				static double		Ceil(double value)				{ return ceil(value); }
				static double		Floor(double value)				{ return floor(value); }
				static double		Round(double value)				{ return round(value); }
				static double		Trunc(double value)				{ return trunc(value); }
				static vint64_t		CeilI(double value)				{ return (vint64_t)ceil(value); }
				static vint64_t		FloorI(double value)			{ return (vint64_t)floor(value); }
				static vint64_t		RoundI(double value)			{ return (vint64_t)round(value); }
				static vint64_t		TruncI(double value)			{ return (vint64_t)trunc(value); }
			};

/***********************************************************************
Predefined Types
***********************************************************************/

			struct VoidValue{};

			DECL_TYPE_INFO(Sys)
			DECL_TYPE_INFO(Math)
			
			DECL_TYPE_INFO(void)
			DECL_TYPE_INFO(VoidValue)
			DECL_TYPE_INFO(IDescriptable)
			DECL_TYPE_INFO(DescriptableObject)
			DECL_TYPE_INFO(Value)
			DECL_TYPE_INFO(vuint8_t)
			DECL_TYPE_INFO(vuint16_t)
			DECL_TYPE_INFO(vuint32_t)
			DECL_TYPE_INFO(vuint64_t)
			DECL_TYPE_INFO(vint8_t)
			DECL_TYPE_INFO(vint16_t)
			DECL_TYPE_INFO(vint32_t)
			DECL_TYPE_INFO(vint64_t)
			DECL_TYPE_INFO(float)
			DECL_TYPE_INFO(double)
			DECL_TYPE_INFO(bool)
			DECL_TYPE_INFO(wchar_t)
			DECL_TYPE_INFO(WString)
			DECL_TYPE_INFO(Locale)
			DECL_TYPE_INFO(DateTime)

			DECL_TYPE_INFO(IValueEnumerator)
			DECL_TYPE_INFO(IValueEnumerable)
			DECL_TYPE_INFO(IValueReadonlyList)
			DECL_TYPE_INFO(IValueList)
			DECL_TYPE_INFO(IValueObservableList)
			DECL_TYPE_INFO(IValueReadonlyDictionary)
			DECL_TYPE_INFO(IValueDictionary)
			DECL_TYPE_INFO(IValueInterfaceProxy)
			DECL_TYPE_INFO(IValueFunctionProxy)
			DECL_TYPE_INFO(IValueListener)
			DECL_TYPE_INFO(IValueSubscription)
			DECL_TYPE_INFO(IValueCallStack)
			DECL_TYPE_INFO(IValueException)

			DECL_TYPE_INFO(IBoxedValue)
			DECL_TYPE_INFO(IValueType::CompareResult)
			DECL_TYPE_INFO(IValueType)
			DECL_TYPE_INFO(IEnumType)
			DECL_TYPE_INFO(ISerializableType)
			DECL_TYPE_INFO(ITypeInfo)
			DECL_TYPE_INFO(ITypeInfo::Decorator)
			DECL_TYPE_INFO(IMemberInfo)
			DECL_TYPE_INFO(IEventHandler)
			DECL_TYPE_INFO(IEventInfo)
			DECL_TYPE_INFO(IPropertyInfo)
			DECL_TYPE_INFO(IParameterInfo)
			DECL_TYPE_INFO(IMethodInfo)
			DECL_TYPE_INFO(IMethodGroupInfo)
			DECL_TYPE_INFO(TypeDescriptorFlags)
			DECL_TYPE_INFO(ITypeDescriptor)


#define DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(TYPENAME)\
			template<>\
			struct TypedValueSerializerProvider<TYPENAME>\
			{\
				static TYPENAME GetDefaultValue();\
				static bool Serialize(const TYPENAME& input, WString& output);\
				static bool Deserialize(const WString& input, TYPENAME& output);\
				static IValueType::CompareResult Compare(const TYPENAME& a, const TYPENAME& b);\
			};\

			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint8_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint16_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint32_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint64_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint8_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint16_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint32_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint64_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(float)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(double)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(bool)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(wchar_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(WString)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(Locale)

#undef DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER

/***********************************************************************
Helper Functions
***********************************************************************/

			extern vint										ITypeDescriptor_GetTypeDescriptorCount();
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(vint index);
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(const WString& name);
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(const Value& value);

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

			extern bool										LoadPredefinedTypes();
		}
	}
}

#endif
