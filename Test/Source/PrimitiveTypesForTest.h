#include "../../Source/Basic.h"

using namespace vl;

constexpr const vint UNORDERABLE_INT_CONSTANT = -999;

template<typename T>
struct ordering_vint
{
	vint value = 0;

	ordering_vint() = default;
	ordering_vint(const ordering_vint<T>&) = default;
	ordering_vint(ordering_vint<T>&&) = default;
	~ordering_vint() = default;
	ordering_vint<T>& operator=(const ordering_vint<T>&) = default;
	ordering_vint<T>& operator=(ordering_vint<T>&&) = default;

	ordering_vint(vint _value) :value(_value) {}

	friend T operator<=>(const ordering_vint<T>& a, const ordering_vint<T>& b)
	{
		if constexpr (std::is_same_v<T, std::partial_ordering>)
		{
			if (a.value == UNORDERABLE_INT_CONSTANT || b.value == UNORDERABLE_INT_CONSTANT)
			{
				return std::partial_ordering::unordered;
			}
		}
		return a.value <=> b.value;
	}

	friend bool operator==(const ordering_vint<T>& a, const ordering_vint<T>& b)
	{
		return a.value == b.value;
	}
};

using so_vint = ordering_vint<std::strong_ordering>;
using wo_vint = ordering_vint<std::weak_ordering>;
using po_vint = ordering_vint<std::partial_ordering>;

template<typename T>
struct Copyable
{
	Ptr<T> value;

	Copyable() = default;
	Copyable(T _value) :value(Ptr(new T(_value))) {}
	Copyable(const Copyable<T>&) = delete;
	Copyable(Copyable<T>&&) = default;
	~Copyable() = default;
	Copyable<T>& operator=(const Copyable<T>&) = delete;
	Copyable<T>& operator=(Copyable<T>&&) = default;

	auto operator<=>(const Copyable<T>& c) const
	{
		if (value && c.value)
		{
			return *value.Obj() <=> *c.value.Obj();
		}
		else
		{
			return value.Obj() <=> c.value.Obj();
		}
	}

	bool operator==(const Copyable<T>& value) const
	{
		return operator<=>(value) == 0;
	}
};

template<typename T>
struct Moveonly
{
	T value;

	Moveonly() = default;
	Moveonly(T _value) :value(_value) {}
	Moveonly(const Moveonly<T>&) = delete;
	Moveonly(Moveonly<T>&&) = default;
	~Moveonly() = default;
	Moveonly<T>& operator=(const Moveonly<T>&) = delete;
	Moveonly<T>& operator=(Moveonly<T>&&) = default;

	auto operator<=>(const Moveonly<T>&) const = default;
};