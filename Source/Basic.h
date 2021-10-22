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

#if defined _WIN64 || __x86_64 || __LP64__
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

#if defined VCZH_MSVC
#include <intrin.h>
#elif defined VCZH_GCC
#include <x86intrin.h>
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
#elif defined VCZH_GCC
#define INCRC(x)	(__sync_add_and_fetch(x, 1))
#define DECRC(x)	(__sync_sub_and_fetch(x, 1))
#endif
#endif

	/***********************************************************************
	Basic Types
	***********************************************************************/

	/// <summary>Base type for all classes to stop generating default copy constructors.</summary>
	class NotCopyable
	{
	private:
		NotCopyable(const NotCopyable&);
		NotCopyable& operator=(const NotCopyable&);
	public:
		NotCopyable();
	};

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

	/***********************************************************************
	Type Traits
	***********************************************************************/

	template<typename T>
	struct RemoveReference
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveReference<T&>
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveReference<T&&>
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveConst
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveConst<const T>
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveVolatile
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveVolatile<volatile T>
	{
		typedef T			Type;
	};

	template<typename T>
	struct RemoveCVR
	{
		typedef T								Type;
	};

	template<typename T>
	struct RemoveCVR<T&>
	{
		typedef typename RemoveCVR<T>::Type		Type;
	};

	template<typename T>
	struct RemoveCVR<T&&>
	{
		typedef typename RemoveCVR<T>::Type		Type;
	};

	template<typename T>
	struct RemoveCVR<const T>
	{
		typedef typename RemoveCVR<T>::Type		Type;
	};

	template<typename T>
	struct RemoveCVR<volatile T>
	{
		typedef typename RemoveCVR<T>::Type		Type;
	};

	template<typename T>
	typename RemoveReference<T>::Type&& MoveValue(T&& value)
	{
		return (typename RemoveReference<T>::Type&&)value;
	}

	template<typename T>
	T&& ForwardValue(typename RemoveReference<T>::Type&& value)
	{
		return (T&&)value;
	}

	template<typename T>
	T&& ForwardValue(typename RemoveReference<T>::Type& value)
	{
		return (T&&)value;
	}

	template<typename ...TArgs>
	struct TypeTuple
	{
	};

	/***********************************************************************
	Basic Types
	***********************************************************************/

	/// <summary>
	/// Base type of all classes.
	/// This type has a virtual destructor, making all derived classes destructors virtual.
	/// In this way an object is allowed to be deleted using a pointer of a qualified base type pointing to this object.
	/// </summary>
	class Object
	{
	public:
		virtual ~Object();
	};

	/// <summary>Store data of a value type in a reference type object. It is useful when the data is required to be stored in a pointer to [T:vl.Object].</summary>
	/// <example><![CDATA[
	/// int main()
	/// {
	///     Ptr<Object> boxed = MakePtr<ObjectBox<vint>>(100);
	///     vint unboxed = boxed.Cast<ObjectBox<vint>>()->Unbox();
	///     Console::WriteLine(itow(unboxed));
	/// }
	/// ]]></example>
	/// <typeparam name="T">Value type to use.</typeparam>
	template<typename T>
	class ObjectBox : public Object
	{
	private:
		T					object;
	public:
		/// <summary>Create a boxed reference object from data of a value type.</summary>
		/// <param name="_object">The data to box.</param>
		ObjectBox(const T& _object)
			:object(_object)
		{
		}

		/// <summary>Create a boxed reference object by moving data of a value type.</summary>
		/// <param name="_object">The data to move and box.</param>
		ObjectBox(T&& _object)
			:object(MoveValue(_object))
		{
		}

		/// <summary>Copy a boxed reference object.</summary>
		/// <param name="value">The reference object to copy.</param>
		ObjectBox(const ObjectBox<T>& value) = default;

		/// <summary>Move a boxed reference object.</summary>
		/// <param name="value">The reference object to move.</param>
		ObjectBox(ObjectBox<T>&& value) = default;

		/// <summary>Replace the boxed data of a value type.</summary>
		/// <returns>The reference object itself.</returns>
		/// <param name="_object">The data to replace the original data.</param>
		ObjectBox<T>& operator=(const T& _object)
		{
			object = _object;
			return *this;
		}

		/// <summary>Replace the boxed data from another reference object.</summary>
		/// <returns>The reference object itself.</returns>
		/// <param name="value">The reference object to copy.</param>
		ObjectBox<T>& operator=(const ObjectBox<T>& value) = default;

		/// <summary>Replace the boxed data by moving from another reference object.</summary>
		/// <returns>The reference object itself.</returns>
		/// <param name="value">The reference object to move.</param>
		ObjectBox<T>& operator=(ObjectBox<T>&& value) = default;

		/// <summary>Get the boxed data of a value type.</summary>
		/// <returns>The unboxed data of a value type.</returns>
		const T& Unbox()
		{
			return object;
		}
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
			:object(new T(MoveValue(value)))
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
		/// Returns a positive value when the first value is greater than the second value.
		/// Returns a negative value when the first value is lesser than the second value.
		/// Returns zero when the two values equal.
		/// When one is null and another one is not, the non-null one is greater.
		/// </returns>
		/// <param name="a">The first nullable value to compare.</param>
		/// <param name="b">The second nullable value to compare.</param>
		static vint Compare(const Nullable<T>& a, const Nullable<T>& b)
		{
			if (a.object && b.object)
			{
				if (*a.object > *b.object) return 1;
				if (*a.object < *b.object) return -1;
				return 0;
			}
			if (a.object) return 1;
			if (b.object) return -1;
			return 0;
		}

		bool operator==(const Nullable<T>& nullable)const
		{
			return Equals(*this, nullable);
		}

		bool operator!=(const Nullable<T>& nullable)const
		{
			return !Equals(*this, nullable);
		}

		bool operator<(const Nullable<T>& nullable)const
		{
			return Compare(*this, nullable) < 0;
		}

		bool operator<=(const Nullable<T>& nullable)const
		{
			return Compare(*this, nullable) <= 0;
		}

		bool operator>(const Nullable<T>& nullable)const
		{
			return Compare(*this, nullable) > 0;
		}

		bool operator>=(const Nullable<T>& nullable)const
		{
			return Compare(*this, nullable) >= 0;
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

	/// <summary>Test is a type a Plain-Old-Data type for containers.</summary>
	/// <typeparam name="T">The type to test.</typeparam>
	template<typename T>
	struct POD
	{
		/// <summary>Returns true if the type is a Plain-Old-Data type.</summary>
		static const bool Result = false;
	};

	template<>struct POD<bool> { static const bool Result = true; };
	template<>struct POD<vint8_t> { static const bool Result = true; };
	template<>struct POD<vuint8_t> { static const bool Result = true; };
	template<>struct POD<vint16_t> { static const bool Result = true; };
	template<>struct POD<vuint16_t> { static const bool Result = true; };
	template<>struct POD<vint32_t> { static const bool Result = true; };
	template<>struct POD<vuint32_t> { static const bool Result = true; };
	template<>struct POD<vint64_t> { static const bool Result = true; };
	template<>struct POD<vuint64_t> { static const bool Result = true; };
	template<>struct POD<char> { static const bool Result = true; };
	template<>struct POD<wchar_t> { static const bool Result = true; };
	template<typename T>struct POD<T*> { static const bool Result = true; };
	template<typename T>struct POD<T&> { static const bool Result = true; };
	template<typename T>struct POD<T&&> { static const bool Result = true; };
	template<typename T, typename C>struct POD<T C::*> { static const bool Result = true; };
	template<typename T, vint _Size>struct POD<T[_Size]> { static const bool Result = POD<T>::Result; };
	template<typename T>struct POD<const T> { static const bool Result = POD<T>::Result; };
	template<typename T>struct POD<volatile T> { static const bool Result = POD<T>::Result; };
	template<typename T>struct POD<const volatile T> { static const bool Result = POD<T>::Result; };

	/***********************************************************************
	Date and Time
	***********************************************************************/

	/// <summary>A type representing the combination of date and time.</summary>
	struct DateTime
	{
		/// <summary>The year.</summary>
		vint				year;
		/// <summary>The month, from 1 to 12.</summary>
		vint				month;
		/// <summary>The day, from 1 to 31.</summary>
		vint				day;
		/// <summary>The hour, from 0 to 23.</summary>
		vint				hour;
		/// <summary>The minute, from 0 to 59.</summary>
		vint				minute;
		/// <summary>The second, from 0 to 60.</summary>
		vint				second;
		/// <summary>The milliseconds, from 0 to 999.</summary>
		vint				milliseconds;

		/// <summary>
		/// The calculated total milliseconds. It is OS dependent because the start time is different.
		/// It is from 0 to 6, representing Sunday to Saturday.
		/// </summary>
		vint				dayOfWeek;

		/// <summary>
		/// The calculated total milliseconds. It is OS dependent because the start time is different.
		/// You should not rely on the fact about how this value is created.
		/// The only invariant thing is that, when an date time is earlier than another, the totalMilliseconds is lesser.
		/// </summary>
		vuint64_t			totalMilliseconds;

		/// <summary>
		/// The calculated file time for the date and time. It is OS dependent.
		/// You should not rely on the fact about how this value is created.
		/// The only invariant thing is that, when an date time is earlier than another, the filetime is lesser.
		/// </summary>
		vuint64_t			filetime;

		/// <summary>Get the current local time.</summary>
		/// <returns>The current local time.</returns>
		static DateTime		LocalTime();

		/// <summary>Get the current UTC time.</summary>
		/// <returns>The current UTC time.</returns>
		static DateTime		UtcTime();

		/// <summary>Create a date time value.</summary>
		/// <returns>The created date time value.</returns>
		/// <param name="_year">The year.</param>
		/// <param name="_month">The month.</param>
		/// <param name="_day">The day.</param>
		/// <param name="_hour">The hour.</param>
		/// <param name="_minute">The minute.</param>
		/// <param name="_second">The second.</param>
		/// <param name="_milliseconds">The millisecond.</param>
		static DateTime		FromDateTime(vint _year, vint _month, vint _day, vint _hour = 0, vint _minute = 0, vint _second = 0, vint _milliseconds = 0);

		/// <summary>Create a date time value from a file time.</summary>
		/// <returns>The created date time value.</returns>
		/// <param name="filetime">The file time.</param>
		static DateTime		FromFileTime(vuint64_t filetime);

		/// <summary>Create an empty date time value that is not meaningful.</summary>
		DateTime();

		/// <summary>Convert the UTC time to the local time.</summary>
		/// <returns>The UTC time.</returns>
		DateTime			ToLocalTime();
		/// <summary>Convert the local time to the UTC time.</summary>
		/// <returns>The local time.</returns>
		DateTime			ToUtcTime();
		/// <summary>Move forward.</summary>
		/// <returns>The moved time.</returns>
		/// <param name="milliseconds">The delta in milliseconds.</param>
		DateTime			Forward(vuint64_t milliseconds);
		/// <summary>Move Backward.</summary>
		/// <returns>The moved time.</returns>
		/// <param name="milliseconds">The delta in milliseconds.</param>
		DateTime			Backward(vuint64_t milliseconds);

		bool operator==(const DateTime& value)const { return filetime == value.filetime; }
		bool operator!=(const DateTime& value)const { return filetime != value.filetime; }
		bool operator<(const DateTime& value)const { return filetime < value.filetime; }
		bool operator<=(const DateTime& value)const { return filetime <= value.filetime; }
		bool operator>(const DateTime& value)const { return filetime > value.filetime; }
		bool operator>=(const DateTime& value)const { return filetime >= value.filetime; }
	};

	/***********************************************************************
	Interface
	***********************************************************************/

	/// <summary>Base type of all interfaces. All interface types are encouraged to be virtual inherited.</summary>
	class Interface : private NotCopyable
	{
	public:
		virtual ~Interface();
	};

	/***********************************************************************
	Type Traits
	***********************************************************************/

	struct YesType {};
	struct NoType {};

	template<typename T, typename YesOrNo>
	struct AcceptType
	{
	};

	template<typename T>
	struct AcceptType<T, YesType>
	{
		typedef T Type;
	};

	template<typename T1, typename T2>
	struct YesNoAnd
	{
		typedef NoType Type;
	};

	template<>
	struct YesNoAnd<YesType, YesType>
	{
		typedef YesType Type;
	};

	template<typename T1, typename T2>
	struct YesNoOr
	{
		typedef YesType Type;
	};

	template<>
	struct YesNoOr<NoType, NoType>
	{
		typedef NoType Type;
	};

	template<typename YesOrNo>
	struct AcceptValue
	{
		static const bool Result = false;
	};

	template<>
	struct AcceptValue<YesType>
	{
		static const bool Result = true;
	};

	template<typename T>
	T ValueOf();

	template<typename TFrom, typename TTo>
	struct PointerConvertable
	{
		static YesType Test(TTo* value);
		static NoType Test(void* value);

		typedef decltype(Test(ValueOf<TFrom*>())) YesNoType;
	};

	template<typename TFrom, typename TTo>
	struct ReturnConvertable
	{
		static YesType Test(TTo&& value);
		static NoType Test(...);

		typedef decltype(Test(ValueOf<TFrom&&>())) YesNoType;
	};

	template<typename TFrom>
	struct ReturnConvertable<TFrom, void>
	{
		typedef YesType YesNoType;
	};

	template<typename TTo>
	struct ReturnConvertable<void, TTo>
	{
		typedef NoType YesNoType;
	};

	template<>
	struct ReturnConvertable<void, void>
	{
		typedef YesType YesNoType;
	};

	template<typename T, typename U>
	struct AcceptAlways
	{
		typedef T Type;
	};
}

#endif
