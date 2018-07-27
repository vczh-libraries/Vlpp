#include "ParsingXml.h"
#include "../../Stream/MemoryStream.h"
#include "../../Collections/OperationForEach.h"

namespace vl
{
	namespace parsing
	{
		namespace xml
		{
			using namespace stream;
			using namespace collections;
			using namespace regex;

/***********************************************************************
Unescaping Function Foward Declarations
***********************************************************************/

			void XmlMergeTextFragment(vl::collections::List<vl::Ptr<XmlNode>>& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				vint begin=-1;
				vint end=-1;
				for(vint i=value.Count()-1;i>=-1;i--)
				{
					if(i==-1)
					{
						if(end!=-1) begin=0;
					}
					else if(value[i].Cast<XmlText>())
					{
						if(end==-1) end=i;
					}
					else
					{
						if(end!=-1) begin=i+1;
					}
					if(begin!=-1 && end!=-1)
					{
						vint tokenBegin=value[begin].Cast<XmlText>()->content.tokenIndex;
						vint tokenEnd=value[end].Cast<XmlText>()->content.tokenIndex;
						while(tokenBegin>0)
						{
							if(tokens.Get(tokenBegin-1).token==(vint)XmlParserTokenIndex::SPACE || tokens.Get(tokenBegin-1).token==-1)
							{
								tokenBegin--;
							}
							else
							{
								break;
							}
						}
						while(tokenEnd<tokens.Count()-1)
						{
							if(tokens.Get(tokenEnd+1).token==(vint)XmlParserTokenIndex::SPACE || tokens.Get(tokenEnd+1).token==-1)
							{
								tokenEnd++;
							}
							else
							{
								break;
							}
						}

						const RegexToken& beginToken=tokens.Get(tokenBegin);
						const RegexToken& endToken=tokens.Get(tokenEnd);
						const wchar_t* textBegin=beginToken.reading;
						const wchar_t* textEnd=endToken.reading+endToken.length;
						WString text(textBegin, vint(textEnd-textBegin));
						ParsingTextRange range(&beginToken, &endToken);

						Ptr<XmlText> xmlText=new XmlText;
						xmlText->codeRange=range;
						xmlText->content.codeRange=range;
						xmlText->content.value=XmlUnescapeValue(text);

						value.RemoveRange(begin, end-begin+1);
						value.Insert(begin, xmlText);

						begin=-1;
						end=-1;
					}
				}
			}

			void XmlUnescapeAttributeValue(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				value.value=XmlUnescapeValue(value.value.Sub(1, value.value.Length()-2));
			}

			void XmlUnescapeCData(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				value.value=XmlUnescapeCData(value.value);
			}

			void XmlUnescapeComment(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				value.value=XmlUnescapeComment(value.value);
			}

/***********************************************************************
XmlPrintVisitor
***********************************************************************/

			class XmlPrintVisitor : public Object, public XmlNode::IVisitor
			{
			public:
				TextWriter&					writer;

				XmlPrintVisitor(TextWriter& _writer)
					:writer(_writer)
				{
				}

				void Visit(XmlText* node)
				{
					writer.WriteString(XmlEscapeValue(node->content.value));
				}

				void Visit(XmlCData* node)
				{
					writer.WriteString(XmlEscapeCData(node->content.value));
				}

				void Visit(XmlAttribute* node)
				{
					writer.WriteString(node->name.value);
					writer.WriteString(L"=\"");
					writer.WriteString(XmlEscapeValue(node->value.value));
					writer.WriteString(L"\"");
				}

				void Visit(XmlComment* node)
				{
					writer.WriteString(XmlEscapeComment(node->content.value));
				}

				void Visit(XmlElement* node)
				{
					writer.WriteChar(L'<');
					writer.WriteString(node->name.value);
					FOREACH(Ptr<XmlAttribute>, att, node->attributes)
					{
						writer.WriteChar(L' ');
						att->Accept(this);
					}
					if(node->subNodes.Count()==0)
					{
						writer.WriteString(L"/>");
					}
					else
					{
						writer.WriteChar(L'>');
						FOREACH(Ptr<XmlNode>, subNode, node->subNodes)
						{
							subNode->Accept(this);
						}
						writer.WriteString(L"</");
						writer.WriteString(node->name.value);
						writer.WriteChar(L'>');
					}
				}

				void Visit(XmlInstruction* node)
				{
					writer.WriteString(L"<?");
					writer.WriteString(node->name.value);
					FOREACH(Ptr<XmlAttribute>, att, node->attributes)
					{
						writer.WriteChar(L' ');
						att->Accept(this);
					}
					writer.WriteString(L"?>");
				}

				void Visit(XmlDocument* node)
				{
					FOREACH(Ptr<XmlNode>, prolog, node->prologs)
					{
						prolog->Accept(this);
					}
					node->rootElement->Accept(this);
				}
			};

/***********************************************************************
API
***********************************************************************/

			WString XmlEscapeValue(const WString& value)
			{
				WString result;
				const wchar_t* reading=value.Buffer();
				while(wchar_t c=*reading++)
				{
					switch(c)
					{
					case L'<':
						result+=L"&lt;";
						break;
					case L'>':
						result+=L"&gt;";
						break;
					case L'&':
						result+=L"&amp;";
						break;
					case L'\'':
						result+=L"&apos;";
						break;
					case L'\"':
						result+=L"&quot;";
						break;
					default:
						result+=c;
					}
				}
				return result;
			}

			WString XmlUnescapeValue(const WString& value)
			{
				WString result;
				const wchar_t* reading=value.Buffer();
				while(*reading)
				{
					if(wcsncmp(reading, L"&lt;", 4)==0)
					{
						result+=L'<';
						reading+=4;
					}
					else if(wcsncmp(reading, L"&gt;", 4)==0)
					{
						result+=L'>';
						reading+=4;
					}
					else if(wcsncmp(reading, L"&amp;", 5)==0)
					{
						result+=L'&';
						reading+=5;
					}
					else if(wcsncmp(reading, L"&apos;", 6)==0)
					{
						result+=L'\'';
						reading+=6;
					}
					else if(wcsncmp(reading, L"&quot;", 6)==0)
					{
						result+=L'\"';
						reading+=6;
					}
					else
					{
						result+=*reading++;
					}
				}
				return result;
			}

			WString XmlEscapeCData(const WString& value)
			{
				return L"<![CDATA["+value+L"]]>";
			}

			WString XmlUnescapeCData(const WString& value)
			{
				return value.Sub(9, value.Length()-12);
			}

			WString XmlEscapeComment(const WString& value)
			{
				return L"<!--"+value+L"-->";
			}

			WString XmlUnescapeComment(const WString& value)
			{
				return value.Sub(4, value.Length()-7);
			}

			void XmlPrint(Ptr<XmlNode> node, stream::TextWriter& writer)
			{
				XmlPrintVisitor visitor(writer);
				node->Accept(&visitor);
			}

			void XmlPrintContent(Ptr<XmlElement> element, stream::TextWriter& writer)
			{
				XmlPrintVisitor visitor(writer);
				FOREACH(Ptr<XmlNode>, node, element->subNodes)
				{
					node->Accept(&visitor);
				}
			}

			WString XmlToString(Ptr<XmlNode> node)
			{
				return GenerateToStream([&](StreamWriter& writer)
				{
					XmlPrint(node, writer);
				});
			}

/***********************************************************************
Linq To Xml
***********************************************************************/

			Ptr<XmlAttribute> XmlGetAttribute(Ptr<XmlElement> element, const WString& name)
			{
				return XmlGetAttribute(element.Obj(), name);
			}

			Ptr<XmlElement> XmlGetElement(Ptr<XmlElement> element, const WString& name)
			{
				return XmlGetElement(element.Obj(), name);
			}

			collections::LazyList<Ptr<XmlElement>> XmlGetElements(Ptr<XmlElement> element)
			{
				return XmlGetElements(element.Obj());
			}

			collections::LazyList<Ptr<XmlElement>> XmlGetElements(Ptr<XmlElement> element, const WString& name)
			{
				return XmlGetElements(element.Obj(), name);
			}

			WString XmlGetValue(Ptr<XmlElement> element)
			{
				return XmlGetValue(element.Obj());
			}

			Ptr<XmlAttribute> XmlGetAttribute(XmlElement* element, const WString& name)
			{
				FOREACH(Ptr<XmlAttribute>, att, element->attributes)
				{
					if(att->name.value==name)
					{
						return att;
					}
				}
				return 0;
			}

			Ptr<XmlElement> XmlGetElement(XmlElement* element, const WString& name)
			{
				FOREACH(Ptr<XmlNode>, node, element->subNodes)
				{
					Ptr<XmlElement> subElement=node.Cast<XmlElement>();
					if(subElement && subElement->name.value==name)
					{
						return subElement;
					}
				}
				return 0;
			}

			collections::LazyList<Ptr<XmlElement>> XmlGetElements(XmlElement* element)
			{
				return From(element->subNodes)
					.FindType<XmlElement>();
			}

			collections::LazyList<Ptr<XmlElement>> XmlGetElements(XmlElement* element, const WString& name)
			{
				return From(element->subNodes)
					.FindType<XmlElement>()
					.Where([name](Ptr<XmlElement> e){return e->name.value==name;});
			}

			WString XmlGetValue(XmlElement* element)
			{
				WString result;
				FOREACH(Ptr<XmlNode>, node, element->subNodes)
				{
					if(Ptr<XmlText> text=node.Cast<XmlText>())
					{
						result+=text->content.value;
					}
					else if(Ptr<XmlCData> text=node.Cast<XmlCData>())
					{
						result+=text->content.value;
					}
				}
				return result;
			}

/***********************************************************************
XmlElementWriter
***********************************************************************/

			XmlElementWriter::XmlElementWriter(Ptr<XmlElement> _element, const XmlElementWriter* _previousWriter)
				:element(_element)
				,previousWriter(_previousWriter)
			{
			}

			XmlElementWriter::~XmlElementWriter()
			{
			}

			const XmlElementWriter& XmlElementWriter::Attribute(const WString& name, const WString& value)const
			{
				Ptr<XmlAttribute> node=new XmlAttribute;
				node->name.value=name;
				node->value.value=value;
				element->attributes.Add(node);
				return *this;
			}

			XmlElementWriter XmlElementWriter::Element(const WString& name)const
			{
				Ptr<XmlElement> node=new XmlElement;
				node->name.value=name;
				element->subNodes.Add(node);
				return XmlElementWriter(node, this);
			}

			const XmlElementWriter& XmlElementWriter::End()const
			{
				return *previousWriter;
			}

			const XmlElementWriter& XmlElementWriter::Text(const WString& value)const
			{
				Ptr<XmlText> node=new XmlText;
				node->content.value=value;
				element->subNodes.Add(node);
				return *this;
			}

			const XmlElementWriter& XmlElementWriter::CData(const WString& value)const
			{
				Ptr<XmlCData> node=new XmlCData;
				node->content.value=value;
				element->subNodes.Add(node);
				return *this;
			}

			const XmlElementWriter& XmlElementWriter::Comment(const WString& value)const
			{
				Ptr<XmlComment> node=new XmlComment;
				node->content.value=value;
				element->subNodes.Add(node);
				return *this;
			}
		}
	}
}
