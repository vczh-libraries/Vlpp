#include "ParserGen.h"

/***********************************************************************
WriteGetParserTextBuffer
***********************************************************************/

void WriteGetParserTextBuffer(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteString(L"vl::WString ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"GetParserTextBuffer()");

	writer.WriteString(prefix);
	writer.WriteLine(L"{");

	writer.WriteString(prefix);
	writer.WriteLine(L"\treturn parserTextBuffer;");

	writer.WriteString(prefix);
	writer.WriteLine(L"}");
	writer.WriteLine(L"");
}

/***********************************************************************
WriteParserText
***********************************************************************/

void WriteParserText(const WString& parserText, TextWriter& writer)
{
	writer.WriteLine(L"const wchar_t parserTextBuffer[] = ");
	{
		StringReader reader(parserText);
		while(!reader.IsEnd())
		{
			writer.WriteString(L"L\"\\r\\n\" ");
			WString line=reader.ReadLine();
			WriteCppString(line, writer);
			writer.WriteLine(L"");
		}
	}
	writer.WriteLine(L";");
}

/***********************************************************************
WriteParserText
***********************************************************************/

void WriteSerializedTable(Ptr<ParsingTable> table, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	vint lengthBeforeCompressing = 0;
	MemoryStream stream;
	{
		LzwEncoder encoder;
		EncoderStream encoderStream(stream, encoder);
		table->Serialize(encoderStream);
		lengthBeforeCompressing = (vint)encoderStream.Position();
	}
	stream.SeekFromBegin(0);
	vint length = (vint)stream.Size();
	const vint block = 1024;
	vint remain = length % block;
	vint rows = length / block + (remain ? 1 : 0);
	
	writer.WriteLine(L"const vint parserBufferLength = " + itow(length) + L"; // " + itow(lengthBeforeCompressing) + L" bytes before compressing");
	writer.WriteLine(L"const vint parserBufferBlock = " + itow(block) + L";");
	writer.WriteLine(L"const vint parserBufferRemain = " + itow(remain) + L";");
	writer.WriteLine(L"const vint parserBufferRows = " + itow(rows) + L";");
	writer.WriteLine(L"const char* parserBuffer[] = {");
	
	char buffer[block];
	const wchar_t* hex = L"0123456789ABCDEF";
	for (vint i = 0; i < rows; i++)
	{
		vint size = i == rows - 1 ? remain : block;
		stream.Read(buffer, size);
		writer.WriteString(L"\"");
		for (vint j = 0; j < size; j++)
		{
			vuint8_t byte = buffer[j];
			writer.WriteString(L"\\x");
			writer.WriteChar(hex[byte / 16]);
			writer.WriteChar(hex[byte % 16]);
		}
		writer.WriteLine(L"\",");
	}
	writer.WriteLine(L"};");

	writer.WriteLine(L"");
	writer.WriteLine(prefix+L"void " + codeClassPrefix + L"GetParserBuffer(vl::stream::MemoryStream& stream)");
	writer.WriteLine(prefix + L"{");
	writer.WriteLine(prefix + L"\tvl::stream::MemoryStream compressedStream;");
	writer.WriteLine(prefix + L"\tfor (vint i = 0; i < parserBufferRows; i++)");
	writer.WriteLine(prefix + L"\t{");
	writer.WriteLine(prefix + L"\t\tvint size = i == parserBufferRows - 1 ? parserBufferRemain : parserBufferBlock;");
	writer.WriteLine(prefix + L"\t\tcompressedStream.Write((void*)parserBuffer[i], size);");
	writer.WriteLine(prefix + L"\t}");
	writer.WriteLine(prefix + L"\tcompressedStream.SeekFromBegin(0);");
	writer.WriteLine(prefix + L"\tvl::stream::LzwDecoder decoder;");
	writer.WriteLine(prefix + L"\tvl::stream::DecoderStream decoderStream(compressedStream, decoder);");
	writer.WriteLine(prefix + L"\tvl::collections::Array<vl::vuint8_t> buffer(65536);");
	writer.WriteLine(prefix + L"\twhile (true)");
	writer.WriteLine(prefix + L"\t{");
	writer.WriteLine(prefix + L"\t\tvl::vint size = decoderStream.Read(&buffer[0], 65536);");
	writer.WriteLine(prefix + L"\t\tif (size == 0) break;");
	writer.WriteLine(prefix + L"\t\tstream.Write(&buffer[0], size);");
	writer.WriteLine(prefix + L"\t}");
	writer.WriteLine(prefix + L"\tstream.SeekFromBegin(0);");
	writer.WriteLine(prefix + L"}");
}