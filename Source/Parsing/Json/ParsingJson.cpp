#include "ParsingJson.h"
#include "../../Stream/MemoryStream.h"
#include "../../Collections/OperationForEach.h"

namespace vl
{
	namespace parsing
	{
		namespace json
		{
			using namespace stream;
			using namespace collections;

/***********************************************************************
Unescaping Function Foward Declarations
***********************************************************************/

			void JsonUnescapingString(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				value.value = GenerateToStream([&](StreamWriter& writer)
				{
					JsonUnescapeString(value.value.Sub(1, value.value.Length() - 2), writer);
				});
			}

/***********************************************************************
JsonPrintVisitor
***********************************************************************/

			class JsonPrintVisitor : public Object, public JsonNode::IVisitor
			{
			public:
				TextWriter&					writer;

				JsonPrintVisitor(TextWriter& _writer)
					:writer(_writer)
				{
				}

				void Visit(JsonLiteral* node)
				{
					switch(node->value)
					{
					case JsonLiteral::JsonValue::True:
						writer.WriteString(L"true");
						break;
					case JsonLiteral::JsonValue::False:
						writer.WriteString(L"false");
						break;
					case JsonLiteral::JsonValue::Null:
						writer.WriteString(L"null");
						break;
					}
				}

				void Visit(JsonString* node)
				{
					writer.WriteChar(L'\"');
					JsonEscapeString(node->content.value, writer);
					writer.WriteChar(L'\"');
				}

				void Visit(JsonNumber* node)
				{
					writer.WriteString(node->content.value);
				}

				void Visit(JsonArray* node)
				{
					writer.WriteChar(L'[');
					FOREACH_INDEXER(Ptr<JsonNode>, item, i, node->items)
					{
						if(i>0) writer.WriteChar(L',');
						item->Accept(this);
					}
					writer.WriteChar(L']');
				}

				void Visit(JsonObjectField* node)
				{
					writer.WriteChar(L'\"');
					JsonEscapeString(node->name.value, writer);
					writer.WriteString(L"\":");
					node->value->Accept(this);
				}

				void Visit(JsonObject* node)
				{
					writer.WriteChar(L'{');
					FOREACH_INDEXER(Ptr<JsonObjectField>, field, i, node->fields)
					{
						if(i>0) writer.WriteChar(L',');
						field->Accept(this);
					}
					writer.WriteChar(L'}');
				}
			};

/***********************************************************************
API
***********************************************************************/

			void JsonEscapeString(const WString& text, stream::TextWriter& writer)
			{
				const wchar_t* reading=text.Buffer();
				while(wchar_t c=*reading++)
				{
					switch(c)
					{
					case L'\"': writer.WriteString(L"\\\""); break;
					case L'\\': writer.WriteString(L"\\\\"); break;
					case L'/': writer.WriteString(L"\\/"); break;
					case L'\b': writer.WriteString(L"\\b"); break;
					case L'\f': writer.WriteString(L"\\f"); break;
					case L'\n': writer.WriteString(L"\\n"); break;
					case L'\r': writer.WriteString(L"\\r"); break;
					case L'\t': writer.WriteString(L"\\t"); break;
					default: writer.WriteChar(c);
					}
				}
			}

			vuint16_t GetHex(wchar_t c)
			{
				if(L'0'<=c && c<=L'9')
				{
					return c-L'0';
				}
				else if(L'A'<=c && c<=L'F')
				{
					return c-L'A';
				}
				else if(L'a'<=c && c<=L'f')
				{
					return c-L'a';
				}
				else
				{
					return 0;
				}
			}

			void JsonUnescapeString(const WString& text, stream::TextWriter& writer)
			{
				const wchar_t* reading=text.Buffer();
				while(wchar_t c=*reading++)
				{
					if(c==L'\\' && *reading)
					{
						switch(c=*reading++)
						{
						case L'b': writer.WriteChar(L'\b'); break;
						case L'f': writer.WriteChar(L'\f'); break;
						case L'n': writer.WriteChar(L'\n'); break;
						case L'r': writer.WriteChar(L'\r'); break;
						case L't': writer.WriteChar(L'\t'); break;
						case L'u':
							{
								wchar_t h1, h2, h3, h4;
								if((h1=reading[0]) && (h2=reading[1]) && (h3=reading[2]) && (h4=reading[3]))
								{
									reading+=4;
									wchar_t h=(wchar_t)(vuint16_t)(
										(GetHex(h1)<<12) +
										(GetHex(h2)<<8) +
										(GetHex(h3)<<4) +
										(GetHex(h4)<<0)
										);
									writer.WriteChar(h);
								}
							}
							break;
						default: writer.WriteChar(c);
						}
					}
					else
					{
						writer.WriteChar(c);
					}
				}
			}

			void JsonPrint(Ptr<JsonNode> node, stream::TextWriter& writer)
			{
				JsonPrintVisitor visitor(writer);
				node->Accept(&visitor);
			}

			WString JsonToString(Ptr<JsonNode> node)
			{
				return GenerateToStream([&](StreamWriter& writer)
				{
					JsonPrint(node, writer);
				});
			}
		}
	}
}
