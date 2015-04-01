/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parser::ParsingXml

***********************************************************************/

#ifndef VCZH_PARSING_XML_PARSINGXML
#define VCZH_PARSING_XML_PARSINGXML

#include "ParsingXml_Parser.h"
#include "../../Collections/Operation.h"

namespace vl
{
	namespace parsing
	{
		namespace xml
		{
			extern WString							XmlEscapeValue(const WString& value);
			extern WString							XmlUnescapeValue(const WString& value);
			extern WString							XmlEscapeCData(const WString& value);
			extern WString							XmlUnescapeCData(const WString& value);
			extern WString							XmlEscapeComment(const WString& value);
			extern WString							XmlUnescapeComment(const WString& value);
			extern void								XmlPrint(Ptr<XmlNode> node, stream::TextWriter& writer);
			extern void								XmlPrintContent(Ptr<XmlElement> element, stream::TextWriter& writer);
			extern WString							XmlToString(Ptr<XmlNode> node);

			extern Ptr<XmlAttribute>							XmlGetAttribute(Ptr<XmlElement> element, const WString& name);
			extern Ptr<XmlElement>								XmlGetElement(Ptr<XmlElement> element, const WString& name);
			extern collections::LazyList<Ptr<XmlElement>>		XmlGetElements(Ptr<XmlElement> element);
			extern collections::LazyList<Ptr<XmlElement>>		XmlGetElements(Ptr<XmlElement> element, const WString& name);
			extern WString										XmlGetValue(Ptr<XmlElement> element);

			extern Ptr<XmlAttribute>							XmlGetAttribute(XmlElement* element, const WString& name);
			extern Ptr<XmlElement>								XmlGetElement(XmlElement* element, const WString& name);
			extern collections::LazyList<Ptr<XmlElement>>		XmlGetElements(XmlElement* element);
			extern collections::LazyList<Ptr<XmlElement>>		XmlGetElements(XmlElement* element, const WString& name);
			extern WString										XmlGetValue(XmlElement* element);

			class XmlElementWriter : public Object
			{
			protected:
				Ptr<XmlElement>					element;
				const XmlElementWriter*			previousWriter;

			public:
				XmlElementWriter(Ptr<XmlElement> _element, const XmlElementWriter* _previousWriter=0);
				~XmlElementWriter();

				const XmlElementWriter&			Attribute(const WString& name, const WString& value)const;
				XmlElementWriter				Element(const WString& name)const;
				const XmlElementWriter&			End()const;
				const XmlElementWriter&			Text(const WString& value)const;
				const XmlElementWriter&			CData(const WString& value)const;
				const XmlElementWriter&			Comment(const WString& value)const;
			};
		}
	}
}

#endif