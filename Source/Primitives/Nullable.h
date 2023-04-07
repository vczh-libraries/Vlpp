/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_NULLABLE
#define VCZH_NULLABLE

#include "../Basic.h"

namespace vl
{
	/// <summary>Type for representing nullable data.</summary>
	/// <typeparam name="T">Type of the data, typically it is a value type, or [T:vl.Ptr`1] could be used here.</typeparam>
	template<typename T>
	class Nullable
	{
	private:
		T*									object = nullptr;
	public:
		static const Nullable<T>			Empty;

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

		/// <summary>
		/// Remove the contained data.
		/// </summary>
		void Reset()
		{
			if (object) delete object;
			object = nullptr;
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
		/// <returns>
		/// Returns std::strong_ordering indicating the order of the two values.
		/// When one is null and another one is not, the non-null one is greater.
		/// </returns>
		/// <param name="a">The first nullable value to compare.</param>
		/// <param name="b">The second nullable value to compare.</param>
		std::strong_ordering operator<=>(const Nullable<T>& b)const
		{
			if (object && b.object) return *object <=> *b.object;
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

	template<typename T>
	const Nullable<T> Nullable<T>::Empty;
}

#endif