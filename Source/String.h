/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::String

Classes:
	AString										£ºMbcs×Ö·û´®
	WString										£ºUtf-16×Ö·û´®
***********************************************************************/

#ifndef VCZH_STRING
#define VCZH_STRING

#include <memory.h>
#include "Basic.h"

namespace vl
{
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

		ObjectString()
		{
			buffer=(T*)&zero;
			counter=0;
			start=0;
			length=0;
			realLength=0;
		}

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

		ObjectString(const ObjectString<T>& string)
		{
			buffer=string.buffer;
			counter=string.counter;
			start=string.start;
			length=string.length;
			realLength=string.realLength;
			Inc();
		}

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

		vint Length()const
		{
			return length;
		}

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

		ObjectString<T> Left(vint count)const
		{
			CHECK_ERROR(count>=0 && count<=length, L"ObjectString<T>::Left(vint)#Argument count not in range.");
			return ObjectString<T>(*this, 0, count);
		}

		ObjectString<T> Right(vint count)const
		{
			CHECK_ERROR(count>=0 && count<=length, L"ObjectString<T>::Right(vint)#Argument count not in range.");
			return ObjectString<T>(*this, length-count, count);
		}

		ObjectString<T> Sub(vint index, vint count)const
		{
			CHECK_ERROR(index>=0 && index<=length, L"ObjectString<T>::Sub(vint, vint)#Argument index not in range.");
			CHECK_ERROR(index+count>=0 && index+count<=length, L"ObjectString<T>::Sub(vint, vint)#Argument count not in range.");
			return ObjectString<T>(*this, index, count);
		}

		ObjectString<T> Remove(vint index, vint count)const
		{
			CHECK_ERROR(index>=0 && index<length, L"ObjectString<T>::Remove(vint, vint)#Argument index not in range.");
			CHECK_ERROR(index+count>=0 && index+count<=length, L"ObjectString<T>::Remove(vint, vint)#Argument count not in range.");
			return ObjectString<T>(*this, ObjectString<T>(), index, count);
		}

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

	typedef ObjectString<char>		AString;
	typedef ObjectString<wchar_t>	WString;

	extern vint					atoi_test(const AString& string, bool& success);
	extern vint					wtoi_test(const WString& string, bool& success);
	extern vint64_t				atoi64_test(const AString& string, bool& success);
	extern vint64_t				wtoi64_test(const WString& string, bool& success);
	extern vuint				atou_test(const AString& string, bool& success);
	extern vuint				wtou_test(const WString& string, bool& success);
	extern vuint64_t			atou64_test(const AString& string, bool& success);
	extern vuint64_t			wtou64_test(const WString& string, bool& success);
	extern double				atof_test(const AString& string, bool& success);
	extern double				wtof_test(const WString& string, bool& success);

	extern vint					atoi(const AString& string);
	extern vint					wtoi(const WString& string);
	extern vint64_t				atoi64(const AString& string);
	extern vint64_t				wtoi64(const WString& string);
	extern vuint				atou(const AString& string);
	extern vuint				wtou(const WString& string);
	extern vuint64_t			atou64(const AString& string);
	extern vuint64_t			wtou64(const WString& string);
	extern double				atof(const AString& string);
	extern double				wtof(const WString& string);

	extern AString				itoa(vint number);
	extern WString				itow(vint number);
	extern AString				i64toa(vint64_t number);
	extern WString				i64tow(vint64_t number);
	extern AString				utoa(vuint number);
	extern WString				utow(vuint number);
	extern AString				u64toa(vuint64_t number);
	extern WString				u64tow(vuint64_t number);
	extern AString				ftoa(double number);
	extern WString				ftow(double number);

	extern vint					_wtoa(const wchar_t* w, char* a, vint chars);
	extern AString				wtoa(const WString& string);
	extern vint					_atow(const char* a, wchar_t* w, vint chars);
	extern WString				atow(const AString& string);
	extern AString				alower(const AString& string);
	extern WString				wlower(const WString& string);
	extern AString				aupper(const AString& string);
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
#endif
}

#endif
