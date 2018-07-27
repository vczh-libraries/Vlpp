#include "CompressionStream.h"
#include "Accessor.h"
#include "MemoryWrapperStream.h"
#include "../Collections/List.h"

namespace vl
{
	namespace stream
	{
		using namespace collections;
		using namespace lzw;

/***********************************************************************
LzwBase
***********************************************************************/

		void LzwBase::UpdateIndexBits()
		{
			if (nextIndex >=2 && (nextIndex & (nextIndex - 1)) == 0)
			{
				indexBits++;
			}
		}

		lzw::Code* LzwBase::CreateCode(lzw::Code* prefix, vuint8_t byte)
		{
			if (nextIndex < MaxDictionarySize)
			{
				Code* code = codeAllocator.Create();
				code->byte = byte;
				code->code = nextIndex;
				code->parent = prefix;
				code->size = prefix->size + 1;
				prefix->children.Set(byte, code, mapAllocator);
				nextIndex++;

				return code;
			}
			else
			{
				return 0;
			}
		}

		LzwBase::LzwBase()
			:codeAllocator(65536)
			, mapAllocator(1048576)
		{
			root = codeAllocator.Create();

			for (vint i = 0; i < 256; i++)
			{
				UpdateIndexBits();
				CreateCode(root, (vuint8_t)i);
			}
		}

		LzwBase::LzwBase(bool (&existingBytes)[256])
		{
			root = codeAllocator.Create();
			for (vint i = 0; i < 256; i++)
			{
				if (existingBytes[i])
				{
					UpdateIndexBits();
					CreateCode(root, (vuint8_t)i);
				}
			}

			if (indexBits < 8)
			{
				eofIndex = nextIndex++;
			}
		}

		LzwBase::~LzwBase()
		{
		}

/***********************************************************************
LzwEncoder
***********************************************************************/

		void LzwEncoder::Flush()
		{
			vint written = 0;
			vint bufferUsedSize = bufferUsedBits / 8;
			if (bufferUsedBits % 8 != 0)
			{
				bufferUsedSize++;
			}
			while (written < bufferUsedSize)
			{
				vint size = stream->Write(buffer + written, bufferUsedSize - written);
				CHECK_ERROR(size != 0, L"LzwEncoder::Flush()#Failed to flush the lzw buffer.");
				written += size;
			}
			bufferUsedBits = 0;
		}

		vuint8_t highMarks[9] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };
		vuint8_t lowMarks[9] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

		void LzwEncoder::WriteNumber(vint number, vint bitSize)
		{
			vint bitStart = 0;
			vint bitStep = 8 - bufferUsedBits % 8;
			if (bitStep > bitSize)
			{
				bitStep = bitSize;
			}
			while (bitStart < bitSize)
			{
				if(bufferUsedBits == BufferSize * 8)
				{
					Flush();
				}

				vint writeStart = bufferUsedBits % 8;
				vint byteIndex = bufferUsedBits / 8;
				vuint8_t byte = buffer[byteIndex];
				byte &= highMarks[writeStart];

				vuint8_t content = (vuint8_t)((number >> bitStart)&lowMarks[bitStep]) << (8 - writeStart - bitStep);
				byte |= content;

				buffer[byteIndex] = byte;
				bufferUsedBits += bitStep;

				bitStart += bitStep;
				vint remain = bitSize - bitStart;
				bitStep = remain < 8 ? remain : 8;
			}
		}

		LzwEncoder::LzwEncoder()
		{
			prefix = root;
		}

		LzwEncoder::LzwEncoder(bool (&existingBytes)[256])
			:LzwBase(existingBytes)
		{
			prefix = root;
		}

		LzwEncoder::~LzwEncoder()
		{
		}

		void LzwEncoder::Setup(IStream* _stream)
		{
			stream = _stream;
		}

		void LzwEncoder::Close()
		{
			if (prefix != root)
			{
				WriteNumber(prefix->code, indexBits);
				prefix = root;
			}

			vint remain = 8 - bufferUsedBits % 8;
			if (remain != 8 && remain >= indexBits)
			{
				CHECK_ERROR(eofIndex != -1, L"LzwEncoder::Close()#Internal error.");
				WriteNumber(eofIndex, indexBits);
			}
			Flush();
		}

		vint LzwEncoder::Write(void* _buffer, vint _size)
		{
			vuint8_t* bytes = (vuint8_t*)_buffer;
			for (vint i = 0; i < _size; i++)
			{
				vuint8_t byte = bytes[i];
				Code* next = prefix->children.Get(byte);
				if (next)
				{
					prefix = next;
				}
				else
				{
					WriteNumber(prefix->code, indexBits);

					if (nextIndex < MaxDictionarySize)
					{
						UpdateIndexBits();
						CreateCode(prefix, byte);
					}
					prefix = root->children.Get(byte);
				}
			}
			return _size;
		}

/***********************************************************************
LzwDecoder
***********************************************************************/

		bool LzwDecoder::ReadNumber(vint& number, vint bitSize)
		{
			number = 0;
			if (inputBufferSize == -1)
			{
				return false;
			}

			vint remainBits = inputBufferSize * 8 - inputBufferUsedBits;
			vint writtenBits = 0;
			vint bitStep = 8 - inputBufferUsedBits % 8;
			if (bitStep > bitSize)
			{
				bitStep = bitSize;
			}
			while (writtenBits < bitSize)
			{
				if (remainBits == 0)
				{
					inputBufferSize = stream->Read(inputBuffer, BufferSize);
					if (inputBufferSize == 0)
					{
						inputBufferSize = -1;
						return false;
					}
					remainBits = inputBufferSize * 8;
					inputBufferUsedBits = 0;
				}

				vuint8_t byte = inputBuffer[inputBufferUsedBits / 8];
				byte >>= (8 - inputBufferUsedBits % 8 - bitStep);
				byte &= lowMarks[bitStep];
				number |= byte << writtenBits;

				inputBufferUsedBits += bitStep;
				remainBits -= bitStep;
				writtenBits += bitStep;
				vint remain = bitSize - writtenBits;
				bitStep = remain < 8 ? remain : 8;
			}

			return true;
		}

		void LzwDecoder::PrepareOutputBuffer(vint size)
		{
			if (outputBuffer.Count() < size)
			{
				outputBuffer.Resize(size);
			}
			outputBufferSize = size;
		}

		void LzwDecoder::ExpandCodeToOutputBuffer(lzw::Code* code)
		{
			vuint8_t* outputByte = &outputBuffer[0] + code->size;
			Code* current = code;
			while (current != root)
			{
				*(--outputByte) = current->byte;
				current = current->parent;
			}
			outputBufferUsedBytes = 0;
		}

		LzwDecoder::LzwDecoder()
		{
			for (vint i = 0; i < 256; i++)
			{
				dictionary.Add(root->children.Get((vuint8_t)i));
			}
		}

		LzwDecoder::LzwDecoder(bool (&existingBytes)[256])
			:LzwBase(existingBytes)
		{
			for (vint i = 0; i < 256; i++)
			{
				if (existingBytes[i])
				{
					dictionary.Add(root->children.Get((vuint8_t)i));
				}
			}
			if (eofIndex != -1)
			{
				dictionary.Add(0);
			}
		}

		LzwDecoder::~LzwDecoder()
		{
		}

		void LzwDecoder::Setup(IStream* _stream)
		{
			stream = _stream;
		}

		void LzwDecoder::Close()
		{
		}

		vint LzwDecoder::Read(void* _buffer, vint _size)
		{
			vint written = 0;
			vuint8_t* bytes = (vuint8_t*)_buffer;
			while (written < _size)
			{
				vint expect = _size - written;
				vint remain = outputBufferSize - outputBufferUsedBytes;
				if (remain == 0)
				{
					vint index = 0;
					if (!ReadNumber(index, indexBits) || index == eofIndex)
					{
						break;
					}

					Code* prefix = 0;
					if (index == dictionary.Count())
					{
						prefix = lastCode;
						PrepareOutputBuffer(prefix->size + 1);
						ExpandCodeToOutputBuffer(prefix);
						outputBuffer[outputBufferSize - 1] = outputBuffer[0];
					}
					else
					{
						prefix = dictionary[index];
						PrepareOutputBuffer(prefix->size);
						ExpandCodeToOutputBuffer(prefix);
					}
					
					if (nextIndex < MaxDictionarySize)
					{
						if (lastCode)
						{
							dictionary.Add(CreateCode(lastCode, outputBuffer[0]));
						}
						UpdateIndexBits();
					}
					lastCode = dictionary[index];
				}
				else
				{
					if (remain > expect)
					{
						remain = expect;
					}
					memcpy(bytes + written, &outputBuffer[outputBufferUsedBytes], remain);

					outputBufferUsedBytes += remain;
					written += remain;
				}
			}
			return written;
		}

/***********************************************************************
Helper Functions
***********************************************************************/

		vint CopyStream(stream::IStream& inputStream, stream::IStream& outputStream)
		{
			vint totalSize = 0;
			while (true)
			{
				char buffer[1024];
				vint copied = inputStream.Read(buffer, (vint)sizeof(buffer));
				if (copied == 0)
				{
					break;
				}
				totalSize += outputStream.Write(buffer, copied);
			}
			return totalSize;
		}

		const vint CompressionFragmentSize = 1048576;

		void CompressStream(stream::IStream& inputStream, stream::IStream& outputStream)
		{
			Array<char> buffer(CompressionFragmentSize);
			while (true)
			{
				vint size = inputStream.Read(&buffer[0], buffer.Count());
				if (size == 0) break;

				MemoryStream compressedStream;
				{
					LzwEncoder encoder;
					EncoderStream encoderStream(compressedStream, encoder);
					encoderStream.Write(&buffer[0], size);
				}

				compressedStream.SeekFromBegin(0);
				{
					{
						vint32_t bufferSize = (vint32_t)size;
						outputStream.Write(&bufferSize, (vint)sizeof(bufferSize));
					}
					{
						vint32_t compressedSize = (vint32_t)compressedStream.Size();
						outputStream.Write(&compressedSize, (vint)sizeof(compressedSize));
					}
					CopyStream(compressedStream, outputStream);
				}
			}
		}

		void DecompressStream(stream::IStream& inputStream, stream::IStream& outputStream)
		{
			vint totalSize = 0;
			vint totalWritten = 0;
			while (true)
			{
				vint32_t bufferSize = 0;
				if (inputStream.Read(&bufferSize, (vint)sizeof(bufferSize)) != sizeof(bufferSize))
				{
					break;
				}

				vint32_t compressedSize = 0;
				CHECK_ERROR(inputStream.Read(&compressedSize, (vint)sizeof(compressedSize)) == sizeof(compressedSize), L"vl::stream::DecompressStream(MemoryStream&, MemoryStream&)#Incomplete input");

				Array<char> buffer(compressedSize);
				CHECK_ERROR(inputStream.Read(&buffer[0], compressedSize) == compressedSize, L"vl::stream::DecompressStream(MemoryStream&, MemoryStream&)#Incomplete input");

				MemoryWrapperStream compressedStream(&buffer[0], compressedSize);
				LzwDecoder decoder;
				DecoderStream decoderStream(compressedStream, decoder);
				totalWritten += CopyStream(decoderStream, outputStream);
				totalSize += bufferSize;
			}
			CHECK_ERROR(outputStream.Size() == totalSize, L"vl::stream::DecompressStream(MemoryStream&, MemoryStream&)#Incomplete input");
		}
	}
}