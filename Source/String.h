/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::String

Classes:
	AString										: Mbcs using the code page of the current locale
	WString										: UTF-16 (for Windows), or UTF-32 (for Linux and macOS)
***********************************************************************/

#ifndef VCZH_STRING
#define VCZH_STRING

#include <memory.h>
#include "Basic.h"

namespace vl
{
	/// <summary>A type representing a string.</summary>
	/// <typeparam name="T">Type of a character.</typeparam>
	template<typename T>
	class ObjectString : public Object
	{
	private:
		static const T				zero;

		mutable T*					buffer;
		mutable volatile vint*		counter;
		mutable vint				start;
		mutable vint				length;
		mutable vint				realLength;

		static vint CalculateLength(const T* buffer)
		{
			vint result=0;
			while(*buffer++)result++;
			return result;
		}

		static vint Compare(const T* bufA, const ObjectString<T>& strB)
		{
			const T* bufB=strB.buffer+strB.start;
			const T* bufAOld=bufA;
			vint length=strB.length;
			while(length-- && *bufA)
			{
				vint diff=*bufA++-*bufB++;
				if(diff!=0)
				{
					return diff;
				}
			};
			return CalculateLength(bufAOld)-strB.length;
		}

	public:

		static vint Compare(const ObjectString<T>& strA, const ObjectString<T>& strB)
		{
			const T* bufA=strA.buffer+strA.start;
			const T* bufB=strB.buffer+strB.start;
			vint length=strA.length<strB.length?strA.length:strB.length;
			while(length--)
			{
				vint diff=*bufA++-*bufB++;
				if(diff!=0)
				{
					return diff;
				}
			};
			return strA.length-strB.length;
		}

	private:

		void Inc()const
		{
			if(counter)
			{
				INCRC(counter);
			}
		}

		void Dec()const
		{
			if(counter)
			{
				if(DECRC(counter)==0)
				{
					delete[] buffer;
					delete counter;
				}
			}
		}

		ObjectString(const ObjectString<T>& string, vint _start, vint _length)
		{
			if(_length<=0)
			{
				buffer=(T*)&zero;
				counter=0;
				start=0;
				length=0;
				realLength=0;
			}
			else
			{
				buffer=string.buffer;
				counter=string.counter;
				start=string.start+_start;
				length=_length;
				realLength=string.realLength;
				Inc();
			}
		}

		ObjectString(const ObjectString<T>& dest, const ObjectString<T>& source, vint index, vint count)
		{
			if(index==0 && count==dest.length && source.length==0)
			{
				buffer=(T*)&zero;
				counter=0;
				start=0;
				length=0;
				realLength=0;
			}
			else
			{
				counter=new vint(1);
				start=0;
				length=dest.length-count+source.length;
				realLength=length;
				buffer=new T[length+1];
				memcpy(buffer, dest.buffer+dest.start, sizeof(T)*index);
				memcpy(buffer+index, source.buffer+source.start, sizeof(T)*source.length);
				memcpy(buffer+index+source.length, (dest.buffer+dest.start+index+count), sizeof(T)*(dest.length-index-count));
				buffer[length]=0;
			}
		}
	public:
		static ObjectString<T>	Empty;

		/// <summary>Create an empty string.</summary>
		ObjectString()
		{
			buffer=(T*)&zero;
			counter=0;
			start=0;
			length=0;
			realLength=0;
		}

		/// <summary>Create a string continaing one character.</summary>
		/// <param name="_char">The character.</param>
		ObjectString(const T& _char)
		{
			counter=new vint(1);
			start=0;
			length=1;
			buffer=new T[2];
			buffer[0]=_char;
			buffer[1]=0;
			realLength=length;
		}

		/// <summary>Copy a string.</summary>
		/// <param name="_buffer">Memory to copy. It does not have to contain the zero terminator.</param>
		/// <param name="_length">Size of the content in characters.</param>
		ObjectString(const T* _buffer, vint _length)
		{
			if(_length<=0)
			{
				buffer=(T*)&zero;
				counter=0;
				start=0;
				length=0;
				realLength=0;
			}
			else
			{
				buffer=new T[_length+1];
				memcpy(buffer, _buffer, _length*sizeof(T));
				buffer[_length]=0;
				counter=new vint(1);
				start=0;
				length=_length;
				realLength=_length;
			}
		}
		
		/// <summary>Copy a string.</summary>
		/// <param name="_buffer">Memory to copy. It should have to contain the zero terminator.</param>
		/// <param name="copy">Set to true to copy the memory. Set to false to use the memory directly.</param>
		ObjectString(const T* _buffer, bool copy = true)
		{
			CHECK_ERROR(_buffer!=0, L"ObjectString<T>::ObjectString(const T*, bool)#Cannot construct a string from nullptr.");
			if(copy)
			{
				counter=new vint(1);
				start=0;
				length=CalculateLength(_buffer);
				buffer=new T[length+1];
				memcpy(buffer, _buffer, sizeof(T)*(length+1));
				realLength=length;
			}
			else
			{
				buffer=(T*)_buffer;
				counter=0;
				start=0;
				length=CalculateLength(_buffer);
				realLength=length;
			}
		}
		
		/// <summary>Copy a string.</summary>
		/// <param name="string">The string to copy.</param>
		ObjectString(const ObjectString<T>& string)
		{
			buffer=string.buffer;
			counter=string.counter;
			start=string.start;
			length=string.length;
			realLength=string.realLength;
			Inc();
		}
		
		/// <summary>Move a string.</summary>
		/// <param name="string">The string to move.</param>
		ObjectString(ObjectString<T>&& string)
		{
			buffer=string.buffer;
			counter=string.counter;
			start=string.start;
			length=string.length;
			realLength=string.realLength;
			
			string.buffer=(T*)&zero;
			string.counter=0;
			string.start=0;
			string.length=0;
			string.realLength=0;
		}

		~ObjectString()
		{
			Dec();
		}

		/// <summary>Get the zero-terminated buffer in the string. Copying parts of a string does not necessarily create a new buffer, so in some situation the string will not actually points to a zero-terminated buffer. In this case, this function will copy the content to a new buffer with a zero terminator and return.</summary>
		/// <returns>Returns the buffer.</returns>
		const T* Buffer()const
		{
			if(start+length!=realLength)
			{
				T* newBuffer=new T[length+1];
				memcpy(newBuffer, buffer+start, sizeof(T)*length);
				newBuffer[length]=0;
				Dec();
				buffer=newBuffer;
				counter=new vint(1);
				start=0;
				realLength=length;
			}
			return buffer+start;
		}

		ObjectString<T>& operator=(const ObjectString<T>& string)
		{
			if(this!=&string)
			{
				Dec();
				buffer=string.buffer;
				counter=string.counter;
				start=string.start;
				length=string.length;
				realLength=string.realLength;
				Inc();
			}
			return *this;
		}

		ObjectString<T>& operator=(ObjectString<T>&& string)
		{
			if(this!=&string)
			{
				Dec();
				buffer=string.buffer;
				counter=string.counter;
				start=string.start;
				length=string.length;
				realLength=string.realLength;
			
				string.buffer=(T*)&zero;
				string.counter=0;
				string.start=0;
				string.length=0;
				string.realLength=0;
			}
			return *this;
		}

		ObjectString<T>& operator+=(const ObjectString<T>& string)
		{
			return *this=*this+string;
		}

		ObjectString<T> operator+(const ObjectString<T>& string)const
		{
			return ObjectString<T>(*this, string, length, 0);
		}

		bool operator==(const ObjectString<T>& string)const
		{
			return Compare(*this, string)==0;
		}

		bool operator!=(const ObjectString<T>& string)const
		{
			return Compare(*this, string)!=0;
		}

		bool operator>(const ObjectString<T>& string)const
		{
			return Compare(*this, string)>0;
		}

		bool operator>=(const ObjectString<T>& string)const
		{
			return Compare(*this, string)>=0;
		}

		bool operator<(const ObjectString<T>& string)const
		{
			return Compare(*this, string)<0;
		}

		bool operator<=(const ObjectString<T>& string)const
		{
			return Compare(*this, string)<=0;
		}

		bool operator==(const T* buffer)const
		{
			return Compare(buffer, *this)==0;
		}

		bool operator!=(const T* buffer)const
		{
			return Compare(buffer, *this)!=0;
		}

		bool operator>(const T* buffer)const
		{
			return Compare(buffer, *this)<0;
		}

		bool operator>=(const T* buffer)const
		{
			return Compare(buffer, *this)<=0;
		}

		bool operator<(const T* buffer)const
		{
			return Compare(buffer, *this)>0;
		}

		bool operator<=(const T* buffer)const
		{
			return Compare(buffer, *this)>=0;
		}

		T operator[](vint index)const
		{
			CHECK_ERROR(index>=0 && index<length, L"ObjectString:<T>:operator[](vint)#Argument index not in range.");
			return buffer[start+index];
		}

		/// <summary>Get the size of the string in characters.</summary>
		/// <returns>The size.</returns>
		vint Length()const
		{
			return length;
		}

		/// <summary>Find a character.</summary>
		/// <returns>The position of the character. Returns -1 if it doesn not exist.</returns>
		/// <param name="c">The character to find.</param>
		vint IndexOf(T c)const
		{
			const T* reading=buffer+start;
			for(vint i=0;i<length;i++)
			{
				if(reading[i]==c)
					return i;
			}
			return -1;
		}

		/// <summary>Copy the beginning of the string.</summary>
		/// <returns>The copied string.</returns>
		/// <param name="count">Size of characters to copy.</param>
		ObjectString<T> Left(vint count)const
		{
			CHECK_ERROR(count>=0 && count<=length, L"ObjectString<T>::Left(vint)#Argument count not in range.");
			return ObjectString<T>(*this, 0, count);
		}
		
		/// <summary>Copy the ending of the string.</summary>
		/// <returns>The copied string.</returns>
		/// <param name="count">Size of characters to copy.</param>
		ObjectString<T> Right(vint count)const
		{
			CHECK_ERROR(count>=0 && count<=length, L"ObjectString<T>::Right(vint)#Argument count not in range.");
			return ObjectString<T>(*this, length-count, count);
		}
		
		/// <summary>Copy the middle of the string.</summary>
		/// <returns>The copied string.</returns>
		/// <param name="index">Position of characters to copy.</param>
		/// <param name="count">Size of characters to copy.</param>
		ObjectString<T> Sub(vint index, vint count)const
		{
			CHECK_ERROR(index>=0 && index<=length, L"ObjectString<T>::Sub(vint, vint)#Argument index not in range.");
			CHECK_ERROR(index+count>=0 && index+count<=length, L"ObjectString<T>::Sub(vint, vint)#Argument count not in range.");
			return ObjectString<T>(*this, index, count);
		}

		/// <summary>Copy the beginning and the end of the string.</summary>
		/// <returns>The copied string.</returns>
		/// <param name="index">Position of characters NOT to copy.</param>
		/// <param name="count">Size of characters NOT to copy.</param>
		ObjectString<T> Remove(vint index, vint count)const
		{
			CHECK_ERROR(index>=0 && index<length, L"ObjectString<T>::Remove(vint, vint)#Argument index not in range.");
			CHECK_ERROR(index+count>=0 && index+count<=length, L"ObjectString<T>::Remove(vint, vint)#Argument count not in range.");
			return ObjectString<T>(*this, ObjectString<T>(), index, count);
		}

		/// <summary>Make a new string by inserting a string in this string.</summary>
		/// <returns>The copied string.</returns>
		/// <param name="index">Position of characters to insert.</param>
		/// <param name="string">The string to be inserted in this string.</param>
		ObjectString<T> Insert(vint index, const ObjectString<T>& string)const
		{
			CHECK_ERROR(index>=0 && index<=length, L"ObjectString<T>::Insert(vint)#Argument count not in range.");
			return ObjectString<T>(*this, string, index, 0);
		}

		friend bool operator<(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)<0;
		}

		friend bool operator<=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)<=0;
		}

		friend bool operator>(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)>0;
		}

		friend bool operator>=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)>=0;
		}

		friend bool operator==(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)==0;
		}

		friend bool operator!=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right)!=0;
		}

		friend ObjectString<T> operator+(const T* left, const ObjectString<T>& right)
		{
			return ObjectString<T>(left, false)+right;
		}
	};

	template<typename T>
	ObjectString<T> ObjectString<T>::Empty=ObjectString<T>();
	template<typename T>
	const T ObjectString<T>::zero=0;

	/// <summary>Ansi string.</summary>
	typedef ObjectString<char>		AString;
	/// <summary>Unicode string.</summary>
	typedef ObjectString<wchar_t>	WString;

	/// <summary>Convert a string to an signed integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vint					atoi_test(const AString& string, bool& success);
	/// <summary>Convert a string to an signed integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vint					wtoi_test(const WString& string, bool& success);
	/// <summary>Convert a string to an signed 64-bits integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vint64_t				atoi64_test(const AString& string, bool& success);
	/// <summary>Convert a string to an signed 64-bits integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vint64_t				wtoi64_test(const WString& string, bool& success);
	/// <summary>Convert a string to an unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vuint				atou_test(const AString& string, bool& success);
	/// <summary>Convert a string to an unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vuint				wtou_test(const WString& string, bool& success);
	/// <summary>Convert a string to a 64-bits unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vuint64_t			atou64_test(const AString& string, bool& success);
	/// <summary>Convert a string to a 64-bits unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern vuint64_t			wtou64_test(const WString& string, bool& success);
	/// <summary>Convert a string to 64-bits floating point number.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern double				atof_test(const AString& string, bool& success);
	/// <summary>Convert a string to 64-bits floating point number.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	/// <param name="success">Returns true if this operation succeeded.</param>
	extern double				wtof_test(const WString& string, bool& success);

	/// <summary>Convert a string to an signed integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vint					atoi(const AString& string);
	/// <summary>Convert a string to an signed integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vint					wtoi(const WString& string);
	/// <summary>Convert a string to an signed 64-bits integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vint64_t				atoi64(const AString& string);
	/// <summary>Convert a string to an signed 64-bits integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vint64_t				wtoi64(const WString& string);
	/// <summary>Convert a string to an unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vuint				atou(const AString& string);
	/// <summary>Convert a string to an unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vuint				wtou(const WString& string);
	/// <summary>Convert a string to a 64-bits unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vuint64_t			atou64(const AString& string);
	/// <summary>Convert a string to a 64-bits unsigned integer.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern vuint64_t			wtou64(const WString& string);
	/// <summary>Convert a string to a 64-bits floating point number.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern double				atof(const AString& string);
	/// <summary>Convert a string to a 64-bits floating point number.</summary>
	/// <returns>The converted number. If the convert failed, the result is undefined.</returns>
	/// <param name="string">The string to convert.</param>
	extern double				wtof(const WString& string);

	/// <summary>Convert a signed interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern AString				itoa(vint number);
	/// <summary>Convert a signed interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern WString				itow(vint number);
	/// <summary>Convert a 64-bits signed interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern AString				i64toa(vint64_t number);
	/// <summary>Convert a 64-bits signed interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern WString				i64tow(vint64_t number);
	/// <summary>Convert an unsigned interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern AString				utoa(vuint number);
	/// <summary>Convert an unsigned interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern WString				utow(vuint number);
	/// <summary>Convert a 64-bits unsigned interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern AString				u64toa(vuint64_t number);
	/// <summary>Convert a 64-bits unsigned interger to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern WString				u64tow(vuint64_t number);
	/// <summary>Convert a 64-bits floating pointer number to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern AString				ftoa(double number);
	/// <summary>Convert a 64-bits floating pointer number to a string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="number">The number to convert.</param>
	extern WString				ftow(double number);

	extern vint					_wtoa(const wchar_t* w, char* a, vint chars);
	/// <summary>Convert an Unicode string to an Ansi string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern AString				wtoa(const WString& string);
	extern vint					_atow(const char* a, wchar_t* w, vint chars);
	/// <summary>Convert an Ansi string to an Unicode string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern WString				atow(const AString& string);
	/// <summary>Convert all letters to lower case letters.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern AString				alower(const AString& string);
	/// <summary>Convert all letters to lower case letters.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern WString				wlower(const WString& string);
	/// <summary>Convert all letters to upper case letters.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern AString				aupper(const AString& string);
	/// <summary>Convert all letters to upper case letters.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern WString				wupper(const WString& string);

#if defined VCZH_GCC
	extern void					_itoa_s(vint32_t value, char* buffer, size_t size, vint radix);
	extern void					_itow_s(vint32_t value, wchar_t* buffer, size_t size, vint radix);
	extern void					_i64toa_s(vint64_t value, char* buffer, size_t size, vint radix);
	extern void					_i64tow_s(vint64_t value, wchar_t* buffer, size_t size, vint radix);
	extern void					_uitoa_s(vuint32_t value, char* buffer, size_t size, vint radix);
	extern void					_uitow_s(vuint32_t value, wchar_t* buffer, size_t size, vint radix);
	extern void					_ui64toa_s(vuint64_t value, char* buffer, size_t size, vint radix);
	extern void					_ui64tow_s(vuint64_t value, wchar_t* buffer, size_t size, vint radix);
	extern void					_gcvt_s(char* buffer, size_t size, double value, vint numberOfDigits);
	extern void					_strlwr_s(char* buffer, size_t size);
	extern void					_strupr_s(char* buffer, size_t size);
	extern void					_wcslwr_s(wchar_t* buffer, size_t size);
	extern void					_wcsupr_s(wchar_t* buffer, size_t size);
	extern void					wcscpy_s(wchar_t* buffer, size_t size, const wchar_t* text);
#endif

	enum class LoremIpsumCasing
	{
		AllWordsLowerCase,
		FirstWordUpperCase,
		AllWordsUpperCase,
	};

	extern WString				LoremIpsum(vint bestLength, LoremIpsumCasing casing);
	extern WString				LoremIpsumTitle(vint bestLength);
	extern WString				LoremIpsumSentence(vint bestLength);
	extern WString				LoremIpsumParagraph(vint bestLength);
}

#endif
