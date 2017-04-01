#include "GuiTypeDescriptorReflection.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			using namespace collections;

#ifndef VCZH_DEBUG_NO_REFLECTION

/***********************************************************************
LogTypeManager (enum)
***********************************************************************/

			void LogTypeManager_Enum(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				writer.WriteLine((type->GetTypeDescriptorFlags() == TypeDescriptorFlags::FlagEnum ? L"flags " : L"enum ") + type->GetTypeName());
				writer.WriteLine(L"{");

				auto enumType = type->GetEnumType();
				for (vint j = 0; j < enumType->GetItemCount(); j++)
				{
					writer.WriteLine(L"    " + enumType->GetItemName(j) + L" = " + u64tow(enumType->GetItemValue(j)) + L",");
				}

				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager (struct)
***********************************************************************/

			void LogTypeManager_Struct(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				writer.WriteLine(L"struct " + type->GetTypeName());
				writer.WriteLine(L"{");
				for (vint j = 0; j<type->GetPropertyCount(); j++)
				{
					IPropertyInfo* info = type->GetProperty(j);
					writer.WriteLine(L"    " + info->GetReturn()->GetTypeFriendlyName() + L" " + info->GetName() + L";");
				}
				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager (data)
***********************************************************************/

			void LogTypeManager_Data(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				writer.WriteLine(L"primitive " + type->GetTypeName() + L";");
			}

/***********************************************************************
LogTypeManager (class)
***********************************************************************/

			void LogTypeManager_PrintEvents(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				bool printed = false;
				for (vint j = 0; j<type->GetEventCount(); j++)
				{
					printed = true;
					IEventInfo* info = type->GetEvent(j);
					writer.WriteString(L"    event " + info->GetHandlerType()->GetTypeFriendlyName() + L" " + info->GetName() + L"{");
					if (info->GetObservingPropertyCount()>0)
					{
						writer.WriteString(L" observing ");
						vint count = +info->GetObservingPropertyCount();
						for (vint i = 0; i<count; i++)
						{
							if (i>0) writer.WriteString(L", ");
							writer.WriteString(info->GetObservingProperty(i)->GetName());
						}
						writer.WriteString(L";");
					}
					writer.WriteLine(L"};");
				}
				if (printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintProperties(stream::TextWriter& writer, ITypeDescriptor* type, List<IMethodInfo*>& propertyAccessors)
			{
				bool printed = false;
				for (vint j = 0; j<type->GetPropertyCount(); j++)
				{
					printed = true;
					IPropertyInfo* info = type->GetProperty(j);
					writer.WriteString(L"    property " + info->GetReturn()->GetTypeFriendlyName() + L" " + info->GetName() + L"{");
					if (info->GetGetter())
					{
						propertyAccessors.Add(info->GetGetter());
						writer.WriteString(L" getter " + info->GetGetter()->GetName() + L";");
					}
					if (info->GetSetter())
					{
						propertyAccessors.Add(info->GetSetter());
						writer.WriteString(L" setter " + info->GetSetter()->GetName() + L";");
					}
					if (info->GetValueChangedEvent())
					{
						writer.WriteString(L" raising " + info->GetValueChangedEvent()->GetName() + L";");
					}
					writer.WriteLine(L"}");
				}
				if (printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintMethods(stream::TextWriter& writer, ITypeDescriptor* type, const List<IMethodInfo*>& propertyAccessors, bool isPropertyAccessor)
			{
				bool printed = false;
				for (vint j = 0; j<type->GetMethodGroupCount(); j++)
				{
					IMethodGroupInfo* group = type->GetMethodGroup(j);
					for (vint k = 0; k<group->GetMethodCount(); k++)
					{
						IMethodInfo* info = group->GetMethod(k);
						if (propertyAccessors.Contains(info) == isPropertyAccessor)
						{
							printed = true;
							writer.WriteString(WString(L"    ") + (info->IsStatic() ? L"static " : L"") + (isPropertyAccessor ? L"accessor " : L"function ") + info->GetReturn()->GetTypeFriendlyName());
							writer.WriteString(L" " + info->GetName() + L"(");
							for (vint l = 0; l<info->GetParameterCount(); l++)
							{
								if (l>0) writer.WriteString(L", ");
								IParameterInfo* parameter = info->GetParameter(l);
								writer.WriteString(parameter->GetType()->GetTypeFriendlyName() + L" " + parameter->GetName());
							}
							writer.WriteLine(L");");
						}
					}
				}
				if (printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintConstructors(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				if (IMethodGroupInfo* group = type->GetConstructorGroup())
				{
					for (vint k = 0; k<group->GetMethodCount(); k++)
					{
						IMethodInfo* info = group->GetMethod(k);
						writer.WriteString(L"    constructor " + info->GetReturn()->GetTypeFriendlyName());
						writer.WriteString(L" " + info->GetName() + L"(");
						for (vint l = 0; l<info->GetParameterCount(); l++)
						{
							if (l>0) writer.WriteString(L", ");
							IParameterInfo* parameter = info->GetParameter(l);
							writer.WriteString(parameter->GetType()->GetTypeFriendlyName() + L" " + parameter->GetName());
						}
						writer.WriteLine(L");");
					}
				}
			}

			void LogTypeManager_Class(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				bool acceptProxy = false;
				bool isInterface = (type->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined;
				writer.WriteString((isInterface ? L"interface " : L"class ") + type->GetTypeName());
				for (vint j = 0; j<type->GetBaseTypeDescriptorCount(); j++)
				{
					writer.WriteString(j == 0 ? L" : " : L", ");
					writer.WriteString(type->GetBaseTypeDescriptor(j)->GetTypeName());
				}
				writer.WriteLine(L"");
				writer.WriteLine(L"{");

				List<IMethodInfo*> propertyAccessors;
				LogTypeManager_PrintEvents(writer, type);
				LogTypeManager_PrintProperties(writer, type, propertyAccessors);
				LogTypeManager_PrintMethods(writer, type, propertyAccessors, false);
				LogTypeManager_PrintMethods(writer, type, propertyAccessors, true);
				LogTypeManager_PrintConstructors(writer, type);

				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager
***********************************************************************/

			bool IsInterfaceType(ITypeDescriptor* typeDescriptor, bool& acceptProxy)
			{
				bool containsConstructor = false;
				if (IMethodGroupInfo* group = typeDescriptor->GetConstructorGroup())
				{
					containsConstructor = group->GetMethodCount() > 0;
					if (group->GetMethodCount() == 1)
					{
						if (IMethodInfo* info = group->GetMethod(0))
						{
							if (info->GetParameterCount() == 1 && info->GetParameter(0)->GetType()->GetTypeDescriptor()->GetTypeName() == TypeInfo<IValueInterfaceProxy>::content.typeName)
							{
								acceptProxy = true;
								return true;
							}
						}
					}
				}

				if (!containsConstructor)
				{
					if (typeDescriptor->GetTypeName() == TypeInfo<IDescriptable>::content.typeName)
					{
						return true;
					}
					else
					{
						for (vint i = 0; i < typeDescriptor->GetBaseTypeDescriptorCount(); i++)
						{
							bool _acceptProxy = false;
							if (!IsInterfaceType(typeDescriptor->GetBaseTypeDescriptor(i), _acceptProxy))
							{
								return false;
							}
						}
						const wchar_t* name = typeDescriptor->GetTypeName().Buffer();
						while (const wchar_t* next = ::wcschr(name, L':'))
						{
							name = next + 1;
						}
						return name[0] == L'I' && (L'A' <= name[1] && name[1] <= L'Z');
					}
				}
				return false;
			}

			void LogTypeManager(stream::TextWriter& writer)
			{
				for (vint i = 0; i < GetGlobalTypeManager()->GetTypeDescriptorCount(); i++)
				{
					ITypeDescriptor* type = GetGlobalTypeManager()->GetTypeDescriptor(i);

					switch (type->GetTypeDescriptorFlags())
					{
					case TypeDescriptorFlags::Object:
					case TypeDescriptorFlags::IDescriptable:
					case TypeDescriptorFlags::Class:
					case TypeDescriptorFlags::Interface:
						LogTypeManager_Class(writer, type);
						break;
					case TypeDescriptorFlags::FlagEnum:
					case TypeDescriptorFlags::NormalEnum:
						LogTypeManager_Enum(writer, type);
						break;
					case TypeDescriptorFlags::Primitive:
						LogTypeManager_Data(writer, type);
						break;
					case TypeDescriptorFlags::Struct:
						LogTypeManager_Struct(writer, type);
						break;
					default:;
					}
					writer.WriteLine(L"");
				}
			}

#endif
		}
	}
}
