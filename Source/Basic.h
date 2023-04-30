/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_BASIC
#define VCZH_BASIC

#include <stdlib.h>

#ifdef VCZH_CHECK_MEMORY_LEAKS
#define _CRTDBG_MAP_ALLOC
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
#include <new>
#include <atomic>

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
	/// <summary>Signed atomic integer.</summary>
	typedef std::atomic<vint>		atomic_vint;

#define INCRC(ATOMIC) ((ATOMIC)->fetch_add(1) + 1)
#define DECRC(ATOMIC) ((ATOMIC)->fetch_sub(1) - 1)

#ifdef VCZH_64
#define ITOA_S		_i64toa_s
#define ITOW_S		_i64tow_s
#define I64TOA_S	_i64toa_s
#define I64TOW_S	_i64tow_s
#define UITOA_S		_ui64toa_s
#define UITOW_S		_ui64tow_s
#define UI64TOA_S	_ui64toa_s
#define UI64TOW_S	_ui64tow_s
#else
#define ITOA_S		_itoa_s
#define ITOW_S		_itow_s
#define I64TOA_S	_i64toa_s
#define I64TOW_S	_i64tow_s
#define UITOA_S		_ui64toa_s
#define UITOW_S		_ui64tow_s
#define UI64TOA_S	_ui64toa_s
#define UI64TOW_S	_ui64tow_s
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

	template<vint Index, typename TTuple>
	struct TypeTupleItemRetriver;

	template<vint Index, typename T, typename ...TArgs>
	struct TypeTupleItemRetriver<Index, TypeTuple<T, TArgs...>>
	{
		using Type = typename TypeTupleItemRetriver<Index - 1, TypeTuple<TArgs...>>::Type;
	};

	template<typename T, typename ...TArgs>
	struct TypeTupleItemRetriver<0, TypeTuple<T, TArgs...>>
	{
		using Type = T;
	};

	template<vint Index, typename TTuple>
	using TypeTupleElement = typename TypeTupleItemRetriver<Index, TTuple>::Type;

	template<typename T>
	struct RemoveCVRefArrayCtad { using Type = std::remove_cvref_t<T>; };

	template<typename T, vint I>
	struct RemoveCVRefArrayCtad<T(&)[I]> { using Type = T*; };

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

	namespace ordering_decision
	{
		template<bool PO, bool WO, bool SO>
		struct OrderingSelection
		{
			using Type = std::partial_ordering;
		};

		template<bool SO>
		struct OrderingSelection<false, true, SO>
		{
			using Type = std::weak_ordering;
		};

		template<bool WO, bool SO>
		struct OrderingSelection<true, WO, SO>
		{
			using Type = std::partial_ordering;
		};

		template<typename... TOrderings>
		struct OrderingDecision
		{
			static constexpr const vint POs = (0 + ... + (std::is_same_v<std::remove_cvref_t<TOrderings>, std::partial_ordering>));
			static constexpr const vint WOs = (0 + ... + (std::is_same_v<std::remove_cvref_t<TOrderings>, std::weak_ordering>));
			static constexpr const vint SOs = (0 + ... + (std::is_same_v<std::remove_cvref_t<TOrderings>, std::strong_ordering>));
			static_assert(POs + WOs + SOs == sizeof...(TOrderings), "vl::OrderingDecision can only be used on std::(strong|weak|partial)_ordering.");

			using Type = typename OrderingSelection<(POs > 0), (WOs > 0), (SOs > 0)>::Type;
		};
	}

	template<typename... TOrderings>
	using OrderingOf = typename ordering_decision::OrderingDecision<TOrderings...>::Type;

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
