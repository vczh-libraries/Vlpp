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

#define DECL_TYPE_INFO(TYPENAME) template<>struct TypeInfo<TYPENAME>{static const wchar_t* TypeName; static const wchar_t* CppFullTypeName;};
#define IMPL_VL_TYPE_INFO(TYPENAME) const wchar_t* TypeInfo<TYPENAME>::TypeName = L ## #TYPENAME; const wchar_t* TypeInfo<TYPENAME>::CppFullTypeName = L"vl::" L ## #TYPENAME;
#define IMPL_CPP_TYPE_INFO(TYPENAME) const wchar_t* TypeInfo<TYPENAME>::TypeName = L ## #TYPENAME; const wchar_t* TypeInfo<TYPENAME>::CppFullTypeName = L ## #TYPENAME;
#define IMPL_TYPE_INFO_RENAME(TYPENAME, EXPECTEDNAME) const wchar_t* TypeInfo<TYPENAME>::TypeName = L ## #EXPECTEDNAME; const wchar_t* TypeInfo<TYPENAME>::CppFullTypeName = L ## #TYPENAME;

			template<typename T>
			struct TypeInfo
			{
			};

			template<typename T>
			ITypedValueSerializer<T>* GetValueSerializer()
			{
				return dynamic_cast<ITypedValueSerializer<T>*>(GetValueSerializer(TypeInfo<T>::TypeName));
			}

			template<typename T>
			ITypeDescriptor* GetTypeDescriptor()
			{
				return GetTypeDescriptor(TypeInfo<T>::TypeName);
			}

/***********************************************************************
GeneralValueSerializer
***********************************************************************/

			template<typename T>
			class GeneralValueSerializer : public Object, public ITypedValueSerializer<T>
			{
			protected:
				ITypeDescriptor*							ownedTypeDescriptor;

				virtual T									GetDefaultValue() = 0;
				virtual bool								Serialize(const T& input, WString& output)=0;
				virtual bool								Deserialize(const WString& input, T& output)=0;
			public:
				typedef T ValueType;

				GeneralValueSerializer(ITypeDescriptor* _ownedTypeDescriptor)
					:ownedTypeDescriptor(_ownedTypeDescriptor)
				{
				}

				ITypeDescriptor* GetOwnerTypeDescriptor()
				{
					return ownedTypeDescriptor;
				}

				bool Validate(const WString& text)
				{
					T output;
					return Deserialize(text, output);
				}

				bool Parse(const WString& input, Value& output)
				{
					T value;
					if(Deserialize(input, value))
					{
						WString text;
						Serialize(value, text);
						output = Value::From(text, ownedTypeDescriptor);
						return true;
					}
					return false;
				}

				WString GetDefaultText()override
				{
					T defaultValue = GetDefaultValue();
					WString output;
					Serialize(defaultValue, output);
					return output;
				}

				bool HasCandidate()override
				{
					return false;
				}

				vint GetCandidateCount()override
				{
					return 0;
				}

				WString GetCandidate(vint index)override
				{
					return L"";
				}

				bool CanMergeCandidate()override
				{
					return false;
				}

				bool Serialize(const T& input, Value& output)
				{
					WString text;
					if(Serialize(input, text))
					{
						output=Value::From(text, ownedTypeDescriptor);
						return true;
					}
					return false;
				}

				bool Deserialize(const Value& input, T& output)
				{
					if(input.GetValueType()!=Value::Text)
					{
						return false;
					}
					return Deserialize(input.GetText(), output);
				}
			};

/***********************************************************************
TypedValueSerializer
***********************************************************************/

			template<typename T>
			struct TypedValueSerializerProvider
			{
			};

			template<typename T>
			class TypedValueSerializer : public GeneralValueSerializer<T>
			{
			protected:
				T											defaultValue;

				T GetDefaultValue()
				{
					return defaultValue;
				}

				bool Serialize(const T& input, WString& output)override
				{
					return TypedValueSerializerProvider<T>::Serialize(input, output);
				}

				bool Deserialize(const WString& input, T& output)override
				{
					return TypedValueSerializerProvider<T>::Deserialize(input, output);
				}
			public:
				TypedValueSerializer(ITypeDescriptor* _ownedTypeDescriptor, const T& _defaultValue)
					:GeneralValueSerializer<T>(_ownedTypeDescriptor)
					, defaultValue(_defaultValue)
				{
				}
			};

			template<typename T>
			class TypedDefaultValueSerializer : public TypedValueSerializer<T>
			{
			public:
				TypedDefaultValueSerializer(ITypeDescriptor* _ownedTypeDescriptor)
					:TypedValueSerializer<T>(_ownedTypeDescriptor, TypedValueSerializerProvider<T>::GetDefaultValue())
				{
				}
			};

/***********************************************************************
EnumValueSerializer
***********************************************************************/

			template<typename T, bool CanMerge>
			struct EnumValueSerializerProvider
			{
			};

			template<typename T>
			struct EnumValueSerializerProvider<T, true>
			{
				static bool Serialize(collections::Dictionary<WString, T>& candidates, const T& input, WString& output)
				{
					WString result;
					for(vint i=0;i<candidates.Count();i++)
					{
						if(candidates.Values().Get(i)&input)
						{
							if(result!=L"") result+=L"|";
							result+=candidates.Keys()[i];
						}
					}
					output=result;
					return true;
				}

				static bool Deserialize(collections::Dictionary<WString, T>& candidates, const WString& input, T& output)
				{
					T result=(T)0;
					const wchar_t* reading=input.Buffer();
					while(*reading)
					{
						const wchar_t* sep=wcschr(reading, L'|');
						if(!sep) sep=reading+wcslen(reading);
						WString item(reading, vint(sep-reading));
						reading=*sep?sep+1:sep;

						vint index=candidates.Keys().IndexOf(item);
						if(index==-1) return false;
						result=(T)(result|candidates.Values().Get(index));
					}
					output=result;
					return true;
				}
			};

			template<typename T>
			struct EnumValueSerializerProvider<T, false>
			{
				static bool Serialize(collections::Dictionary<WString, T>& candidates, const T& input, WString& output)
				{
					for(vint i=0;i<candidates.Count();i++)
					{
						if(candidates.Values().Get(i)==input)
						{
							output=candidates.Keys()[i];
							return true;
						}
					}
					return false;
				}

				static bool Deserialize(collections::Dictionary<WString, T>& candidates, const WString& input, T& output)
				{
					vint index=candidates.Keys().IndexOf(input);
					if(index==-1) return false;
					output=candidates.Values().Get(index);
					return true;
				}
			};

			template<typename T, bool CanMerge>
			class EnumValueSerializer : public GeneralValueSerializer<T>
			{
			protected:
				T											defaultValue;
				collections::Dictionary<WString, T>			candidates;

				T GetDefaultValue()override
				{
					return defaultValue;
				}

				bool Serialize(const T& input, WString& output)override
				{
					return EnumValueSerializerProvider<T, CanMerge>::Serialize(candidates, input, output);
				}

				bool Deserialize(const WString& input, T& output)override
				{
					return EnumValueSerializerProvider<T, CanMerge>::Deserialize(candidates, input, output);
				}
			public:
				EnumValueSerializer(ITypeDescriptor* _ownedTypeDescriptor, const T& _defaultValue)
					:GeneralValueSerializer<T>(_ownedTypeDescriptor)
					, defaultValue(_defaultValue)
				{
				}

				bool HasCandidate()override
				{
					return true;
				}

				vint GetCandidateCount()override
				{
					return candidates.Count();
				}

				WString GetCandidate(vint index)override
				{
					return candidates.Keys()[index];
				}

				bool CanMergeCandidate()override
				{
					return CanMerge;
				}
			};

/***********************************************************************
SerializableTypeDescriptor
***********************************************************************/

			class SerializableTypeDescriptorBase : public Object, public ITypeDescriptor
			{
			protected:
				Ptr<IValueSerializer>						serializer;
				WString										typeName;
				WString										cppFullTypeName;
			public:
				SerializableTypeDescriptorBase(const WString& _typeName, const WString& _cppFullTypeName, Ptr<IValueSerializer> _serializer);
				~SerializableTypeDescriptorBase();

				const WString&								GetTypeName()override;
				const WString&								GetCppFullTypeName()override;
				IValueSerializer*							GetValueSerializer()override;
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

			template<typename TSerializer>
			class SerializableTypeDescriptor : public SerializableTypeDescriptorBase
			{
			public:
				SerializableTypeDescriptor()
					:SerializableTypeDescriptorBase(TypeInfo<typename TSerializer::ValueType>::TypeName, TypeInfo<typename TSerializer::ValueType>::CppFullTypeName, 0)
				{
					serializer=new TSerializer(this);
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
				static vint8_t		Abs(vint8_t value)				{ return abs(value); }
				static vint16_t		Abs(vint16_t value)				{ return abs(value); }
				static vint32_t		Abs(vint32_t value)				{ return abs(value); }
				static vint64_t		Abs(vint64_t value)				{ return abs(value); }
				static float		Abs(float value)				{ return abs(value); }
				static double		Abs(double value)				{ return abs(value); }

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
			DECL_TYPE_INFO(DateTime)
			DECL_TYPE_INFO(Locale)

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

			DECL_TYPE_INFO(IValueSerializer)
			DECL_TYPE_INFO(ITypeInfo)
			DECL_TYPE_INFO(ITypeInfo::Decorator)
			DECL_TYPE_INFO(IMemberInfo)
			DECL_TYPE_INFO(IEventHandler)
			DECL_TYPE_INFO(IEventInfo)
			DECL_TYPE_INFO(IPropertyInfo)
			DECL_TYPE_INFO(IParameterInfo)
			DECL_TYPE_INFO(IMethodInfo)
			DECL_TYPE_INFO(IMethodGroupInfo)
			DECL_TYPE_INFO(ITypeDescriptor)

			template<>
			struct TypedValueSerializerProvider<vuint8_t>
			{
				static vuint8_t GetDefaultValue();
				static bool Serialize(const vuint8_t& input, WString& output);
				static bool Deserialize(const WString& input, vuint8_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vuint16_t>
			{
				static vuint16_t GetDefaultValue();
				static bool Serialize(const vuint16_t& input, WString& output);
				static bool Deserialize(const WString& input, vuint16_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vuint32_t>
			{
				static vuint32_t GetDefaultValue();
				static bool Serialize(const vuint32_t& input, WString& output);
				static bool Deserialize(const WString& input, vuint32_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vuint64_t>
			{
				static vuint64_t GetDefaultValue();
				static bool Serialize(const vuint64_t& input, WString& output);
				static bool Deserialize(const WString& input, vuint64_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vint8_t>
			{
				static vint8_t GetDefaultValue();
				static bool Serialize(const vint8_t& input, WString& output);
				static bool Deserialize(const WString& input, vint8_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vint16_t>
			{
				static vint16_t GetDefaultValue();
				static bool Serialize(const vint16_t& input, WString& output);
				static bool Deserialize(const WString& input, vint16_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vint32_t>
			{
				static vint32_t GetDefaultValue();
				static bool Serialize(const vint32_t& input, WString& output);
				static bool Deserialize(const WString& input, vint32_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<vint64_t>
			{
				static vint64_t GetDefaultValue();
				static bool Serialize(const vint64_t& input, WString& output);
				static bool Deserialize(const WString& input, vint64_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<float>
			{
				static float GetDefaultValue();
				static bool Serialize(const float& input, WString& output);
				static bool Deserialize(const WString& input, float& output);
			};

			template<>
			struct TypedValueSerializerProvider<double>
			{
				static double GetDefaultValue();
				static bool Serialize(const double& input, WString& output);
				static bool Deserialize(const WString& input, double& output);
			};

			template<>
			struct TypedValueSerializerProvider<wchar_t>
			{
				static wchar_t GetDefaultValue();
				static bool Serialize(const wchar_t& input, WString& output);
				static bool Deserialize(const WString& input, wchar_t& output);
			};

			template<>
			struct TypedValueSerializerProvider<WString>
			{
				static WString GetDefaultValue();
				static bool Serialize(const WString& input, WString& output);
				static bool Deserialize(const WString& input, WString& output);
			};

			template<>
			struct TypedValueSerializerProvider<Locale>
			{
				static Locale GetDefaultValue();
				static bool Serialize(const Locale& input, WString& output);
				static bool Deserialize(const WString& input, Locale& output);
			};

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

			extern bool										LoadPredefinedTypes();
		}
	}
}

#endif