/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_BASIC
#define VCZH_BASIC

#ifdef VCZH_CHECK_MEMORY_LEAKS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define VCZH_CHECK_MEMORY_LEAKS_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#if defined _WIN64 || defined __x86_64 || defined __LP64__ || defined __aarch64__ || defined _M_ARM64
#define VCZH_64
#endif

#if defined _MSC_VER
#define VCZH_MSVC
#else
#define VCZH_GCC
#if defined(__APPLE__)
#define VCZH_APPLE
#endif
#endif

#if defined __arm__ || defined __aarch64__ || defined _M_ARM || defined _M_ARM64
#define VCZH_ARM
#endif

#if defined VCZH_MSVC
#define VCZH_WCHAR_UTF16
#elif defined VCZH_GCC
#define VCZH_WCHAR_UTF32
#endif

#if defined VCZH_WCHAR_UTF16
static_assert(sizeof(wchar_t) == sizeof(char16_t), "wchar_t is not UTF-16.");
#elif defined VCZH_WCHAR_UTF32
static_assert(sizeof(wchar_t) == sizeof(char32_t), "wchar_t is not UTF-32.");
#else
static_assert(false, "wchar_t configuration is not right.");
#endif

#if defined VCZH_ARM
#include <stdlib.h>
#elif defined VCZH_MSVC
#include <intrin.h>
#elif defined VCZH_GCC
#include <x86intrin.h>
#endif

#if defined VCZH_GCC
#include <stdint.h>
#include <stddef.h>
#include <wchar.h>
#define abstract
#define __thiscall
#define __forceinline inline

#define _I8_MIN     ((vint8_t)0x80)
#define _I8_MAX     ((vint8_t)0x7F)
#define _UI8_MAX    ((vuint8_t)0xFF)

#define _I16_MIN    ((vint16_t)0x8000)
#define _I16_MAX    ((vint16_t)0x7FFF)
#define _UI16_MAX   ((vuint16_t)0xFFFF)

#define _I32_MIN    ((vint32_t)0x80000000)
#define _I32_MAX    ((vint32_t)0x7FFFFFFF)
#define _UI32_MAX   ((vuint32_t)0xFFFFFFFF)

#define _I64_MIN    ((vint64_t)0x8000000000000000L)
#define _I64_MAX    ((vint64_t)0x7FFFFFFFFFFFFFFFL)
#define _UI64_MAX   ((vuint64_t)0xFFFFFFFFFFFFFFFFL)
#endif

#include <type_traits>
#include <utility>
#include <compare>

#define L_(x) L__(x)
#define L__(x) L ## x

namespace vl
{

/***********************************************************************
x86 and x64 Compatbility
***********************************************************************/

#if defined VCZH_MSVC
	/// <summary>1-byte signed integer.</summary>
	typedef signed __int8			vint8_t;
	/// <summary>1-byte unsigned integer.</summary>
	typedef unsigned __int8			vuint8_t;
	/// <summary>2-bytes signed integer.</summary>
	typedef signed __int16			vint16_t;
	/// <summary>2-bytes unsigned integer.</summary>
	typedef unsigned __int16		vuint16_t;
	/// <summary>4-bytes signed integer.</summary>
	typedef signed __int32			vint32_t;
	/// <summary>4-bytes unsigned integer.</summary>
	typedef unsigned __int32		vuint32_t;
	/// <summary>8-bytes signed integer.</summary>
	typedef signed __int64			vint64_t;
	/// <summary>8-bytes unsigned integer.</summary>
	typedef unsigned __int64		vuint64_t;
#elif defined VCZH_GCC
	typedef int8_t					vint8_t;
	typedef uint8_t					vuint8_t;
	typedef int16_t					vint16_t;
	typedef uint16_t				vuint16_t;
	typedef int32_t					vint32_t;
	typedef uint32_t				vuint32_t;
	typedef int64_t					vint64_t;
	typedef uint64_t				vuint64_t;
#endif

#ifdef VCZH_64
	/// <summary>Signed interface whose size equals to sizeof(void*).</summary>
	typedef vint64_t				vint;
	/// <summary>Signed interface whose size equals to sizeof(void*).</summary>
	typedef vint64_t				vsint;
	/// <summary>Unsigned interface whose size equals to sizeof(void*).</summary>
	typedef vuint64_t				vuint;
#else
	/// <summary>Signed interface whose size equals to sizeof(void*).</summary>
	typedef vint32_t				vint;
	/// <summary>Signed interface whose size equals to sizeof(void*).</summary>
	typedef vint32_t				vsint;
	/// <summary>Unsigned interface whose size equals to sizeof(void*).</summary>
	typedef vuint32_t				vuint;
#endif
	/// <summary>Signed interger representing position.</summary>
	typedef vint64_t				pos_t;

#ifdef VCZH_64
#define ITOA_S		_i64toa_s
#define ITOW_S		_i64tow_s
#define I64TOA_S	_i64toa_s
#define I64TOW_S	_i64tow_s
#define UITOA_S		_ui64toa_s
#define UITOW_S		_ui64tow_s
#define UI64TOA_S	_ui64toa_s
#define UI64TOW_S	_ui64tow_s
#if defined VCZH_MSVC
#define INCRC(x)	(_InterlockedIncrement64(x))
#define DECRC(x)	(_InterlockedDecrement64(x))
#elif defined VCZH_ARM
#define INCRC(x)	(__atomic_add_fetch(x, 1, __ATOMIC_SEQ_CST))
#define DECRC(x)	(__atomic_sub_fetch(x, 1, __ATOMIC_SEQ_CST))
#elif defined VCZH_GCC
#define INCRC(x)	(__sync_add_and_fetch(x, 1))
#define DECRC(x)	(__sync_sub_and_fetch(x, 1))
#endif
#else
#define ITOA_S		_itoa_s
#define ITOW_S		_itow_s
#define I64TOA_S	_i64toa_s
#define I64TOW_S	_i64tow_s
#define UITOA_S		_ui64toa_s
#define UITOW_S		_ui64tow_s
#define UI64TOA_S	_ui64toa_s
#define UI64TOW_S	_ui64tow_s
#if defined VCZH_MSVC
#define INCRC(x)	(_InterlockedIncrement((volatile long*)(x)))
#define DECRC(x)	(_InterlockedDecrement((volatile long*)(x)))
#elif defined VCZH_ARM
#define INCRC(x)	(__atomic_add_fetch(x, 1, __ATOMIC_SEQ_CST))
#define DECRC(x)	(__atomic_sub_fetch(x, 1, __ATOMIC_SEQ_CST))
#elif defined VCZH_GCC
#define INCRC(x)	(__sync_add_and_fetch(x, 1))
#define DECRC(x)	(__sync_sub_and_fetch(x, 1))
#endif
#endif

/***********************************************************************
Basic Types
***********************************************************************/

	/// <summary>Base type of all errors. An error is an exception that is not recommended to catch. Raising it means there is a mistake in the code.</summary>
	class Error
	{
	private:
		const wchar_t*		description;
	public:
		Error(const wchar_t* _description);

		const wchar_t*		Description()const;
	};

#if defined VCZH_MSVC || defined VCZH_GCC || defined _DEBUG
#define CHECK_ERROR(CONDITION,DESCRIPTION) do{if(!(CONDITION))throw Error(DESCRIPTION);}while(0)
#elif defined NDEBUG
#define CHECK_ERROR(CONDITION,DESCRIPTION)
#endif

#define CHECK_FAIL(DESCRIPTION) do{throw Error(DESCRIPTION);}while(0)

#define SCOPE_VARIABLE(TYPE, VARIABLE, VALUE)\
	if(bool __scope_variable_flag__=true)\
		for(TYPE VARIABLE = VALUE;__scope_variable_flag__;__scope_variable_flag__=false)

	template<typename ...TArgs>
	struct TypeTuple
	{
	};

	/// <summary>
	/// Base type of all classes.
	/// This type has a virtual destructor, making all derived classes destructors virtual.
	/// In this way an object is allowed to be deleted using a pointer of a qualified base type pointing to this object.
	/// </summary>
	class Object
	{
	public:
		virtual ~Object() = default;
	};

	/// <summary>Type for representing nullable data.</summary>
	/// <typeparam name="T">Type of the data, typically it is a value type, or [T:vl.Ptr`1] could be used here.</typeparam>
	template<typename T>
	class Nullable
	{
	private:
		T*					object = nullptr;
	public:
		/// <summary>Create a null value.</summary>
		Nullable() = default;

		/// <summary>Create a non-null value by copying data.</summary>
		/// <param name="value">The data to copy.</param>
		Nullable(const T& value)
			:object(new T(value))
		{
		}

		/// <summary>Create a non-null value by moving data.</summary>
		/// <param name="value">The data to move.</param>
		Nullable(T&& value)
			:object(new T(std::move(value)))
		{
		}

		/// <summary>Create a nullable value by copying from another nullable value.</summary>
		/// <param name="nullable">The nullable value to copy.</param>
		Nullable(const Nullable<T>& nullable)
			:object(nullable.object ? new T(*nullable.object) : nullptr)
		{
		}

		/// <summary>Create a nullable value by moving from another nullable value.</summary>
		/// <param name="nullable">The nullable value to move.</param>
		Nullable(Nullable<T>&& nullable)
			:object(nullable.object)
		{
			nullable.object = nullptr;
		}

		~Nullable()
		{
			if (object) delete object;
		}

		/// <summary>Replace the data inside this nullable value by copying from data.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="value">The data to copy.</param>
		Nullable<T>& operator=(const T& value)
		{
			if (object) delete object;
			object = new T(value);
			return *this;
		}

		/// <summary>Replace the data inside this nullable value by copying from another nullable value.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="nullable">The nullable value to copy.</param>
		Nullable<T>& operator=(const Nullable<T>& nullable)
		{
			if (this != &nullable)
			{
				if (object) delete object;
				if (nullable.object)
				{
					object = new T(*nullable.object);
				}
				else
				{
					object = nullptr;
				}
			}
			return *this;
		}

		/// <summary>Replace the data inside this nullable value by moving from another nullable value.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="nullable">The nullable value to move.</param>
		Nullable<T>& operator=(Nullable<T>&& nullable)
		{
			if (this != &nullable)
			{
				if (object) delete object;
				object = nullable.object;
				nullable.object = nullptr;
			}
			return *this;
		}

		/// <summary>Comparing two nullable values.</summary>
		/// <returns>Returns true when these nullable values are all null, or the data inside them equals.</returns>
		/// <param name="a">The first nullable value to compare.</param>
		/// <param name="b">The second nullable value to compare.</param>
		static bool Equals(const Nullable<T>& a, const Nullable<T>& b)
		{
			if (!a.object && !b.object) return true;
			if (a.object && b.object) return *a.object == *b.object;
			return false;
		}

		/// <summary>Comparing two nullable values.</summary>
		/// <returns>
		/// Returns std::strong_ordering indicating the order of the two values.
		/// When one is null and another one is not, the non-null one is greater.
		/// </returns>
		/// <param name="a">The first nullable value to compare.</param>
		/// <param name="b">The second nullable value to compare.</param>
		std::strong_ordering operator<=>(const Nullable<T>& b)const
		{
			if (object && b.object)
			{
				if (*object > *b.object) return std::strong_ordering::greater;
				if (*object < *b.object) return std::strong_ordering::less;
				return std::strong_ordering::equal;
			}
			if (object) return std::strong_ordering::greater;
			if (b.object) return std::strong_ordering::less;
			return std::strong_ordering::equal;
		}

		bool operator==(const Nullable<T>& b)const
		{
			return operator<=>(b) == 0;
		}

		/// <summary>Test if this nullable value is non-null.</summary>
		/// <returns>Returns true if it is non-null.</returns>
		operator bool()const
		{
			return object != nullptr;
		}

		/// <summary>Return the data inside this nullable value</summary>
		/// <returns>The data inside this nullable value. It crashes when it is null.</returns>
		const T& Value()const
		{
			if (!object) throw Error(L"Nullable<T>::Value()#Cannot unbox from null.");
			return *object;
		}
	};

	template<typename T, size_t minSize>
	union BinaryRetriver
	{
		T t;
		char binary[sizeof(T) > minSize ? sizeof(T) : minSize];
	};

/***********************************************************************
Type Traits
***********************************************************************/

	/// <summary>Type for specify and create a representative value for comparing another value of a specific type for containers.</summary>
	/// <typeparam name="T">The element type for containers.</typeparam>
	template<typename T>
	struct KeyType
	{
	public:
		/// <summary>The type of the representative value.</summary>
		typedef T Type;

		/// <summary>Convert a value in a container to its representative value.</summary>
		/// <returns>The representative value.</returns>
		/// <param name="value">The value in a container.</param>
		static const T& GetKeyValue(const T& value)
		{
			return value;
		}
	};

/***********************************************************************
Interface
***********************************************************************/

#define NOT_COPYABLE(TYPE)\
	TYPE(const TYPE&) = delete;\
	TYPE(TYPE&&) = delete;\
	TYPE& operator=(const TYPE&) = delete;\
	TYPE& operator=(TYPE&&) = delete

	/// <summary>Base type of all interfaces. All interface types are encouraged to be virtual inherited.</summary>
	class Interface
	{
	public:
		NOT_COPYABLE(Interface);

		Interface() = default;
		virtual ~Interface() = default;
	};
}

#endif
