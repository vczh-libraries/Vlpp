/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_NULLABLE
#define VCZH_NULLABLE

#include "../Basic.h"

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif

namespace vl
{
	/// <summary>Type for representing nullable data.</summary>
	/// <typeparam name="T">Type of the data, typically it is a value type, or [T:vl.Ptr`1] could be used here.</typeparam>
	template<typename T>
	class Nullable
	{
	private:
		union
		{
			char buffer[sizeof(T)];
			T object;
		};
		bool								initialized = false;

	public:
		static const Nullable<T>			Empty;

		/// <summary>Create a null value.</summary>
		Nullable()
		{
		}

		/// <summary>Create a non-null value by copying data.</summary>
		/// <param name="value">The data to copy.</param>
		Nullable(const T& value)
			: initialized(true)
		{
			new (&object)T(value);
		}

		/// <summary>Create a non-null value by moving data.</summary>
		/// <param name="value">The data to move.</param>
		Nullable(T&& value)
			: initialized(true)
		{
			new (&object)T(std::move(value));
		}

		/// <summary>Create a nullable value by copying from another nullable value.</summary>
		/// <param name="nullable">The nullable value to copy.</param>
		Nullable(const Nullable<T>& nullable)
			: initialized(nullable.initialized)
		{
			if (nullable.initialized)
			{
				new (&object)T(nullable.object);
			}
		}

		/// <summary>Create a nullable value by moving from another nullable value.</summary>
		/// <param name="nullable">The nullable value to move.</param>
		Nullable(Nullable<T>&& nullable)
			: initialized(nullable.initialized)
		{
			if (nullable.initialized)
			{
				new (&object)T(std::move(nullable.object));
				nullable.Reset();
			}
		}

		~Nullable()
		{
			Reset();
		}

		/// <summary>
		/// Remove the contained data.
		/// </summary>
		void Reset()
		{
			if (initialized)
			{
				object.~T();
				initialized = false;
			}
		}

		/// <summary>Replace the data inside this nullable value by copying from data.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="value">The data to copy.</param>
		Nullable<T>& operator=(const T& value)
		{
			if constexpr (std::is_copy_assignable_v<T>)
			{
				if (initialized)
				{
					object = value;
					return *this;
				}
			}

			Reset();
			new (&object)T(value);
			initialized = true;
			return *this;
		}

		/// <summary>Replace the data inside this nullable value by moving from data.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="value">The data to copy.</param>
		Nullable<T>& operator=(T&& value)
		{
			if constexpr (std::is_move_assignable_v<T>)
			{
				if (initialized)
				{
					object = std::move(value);
					return *this;
				}
			}

			Reset();
			new (&object)T(std::move(value));
			initialized = true;
			return *this;
		}

		/// <summary>Replace the data inside this nullable value by copying from another nullable value.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="nullable">The nullable value to copy.</param>
		Nullable<T>& operator=(const Nullable<T>& nullable)
		{
			if (!nullable.initialized)
			{
				Reset();
			}
			else
			{
				if constexpr (std::is_copy_assignable_v<T>)
				{
					if (initialized)
					{
						object = nullable.object;
						return *this;
					}
				}

				Reset();
				new (&object)T(nullable.object);
				initialized = true;
			}
			return *this;
		}

		/// <summary>Replace the data inside this nullable value by moving from another nullable value.</summary>
		/// <returns>The nullable value itself.</returns>
		/// <param name="nullable">The nullable value to move.</param>
		Nullable<T>& operator=(Nullable<T>&& nullable)
		{
			if (!nullable.initialized)
			{
				Reset();
			}
			else
			{
				if constexpr (std::is_move_assignable_v<T>)
				{
					if (initialized)
					{
						object = std::move(nullable.object);
						nullable.Reset();
						return *this;
					}
				}

				Reset();
				new (&object)T(std::move(nullable.object));
				nullable.Reset();
				initialized = true;
			}
			return *this;
		}

		/// <summary>Comparing two nullable values.</summary>
		/// <returns>
		/// Returns a value indicating the order of the two values, the type is decided by T.
		/// When one is null and another one is not, the non-null one is greater.
		/// </returns>
		/// <param name="a">The first nullable value to compare.</param>
		/// <param name="b">The second nullable value to compare.</param>
		auto operator<=>(const Nullable<T>& b) const
			requires(std::three_way_comparable<T>)
		{
			using TOrdering = decltype(object <=> b.object);
			if (initialized && b.initialized) return object <=> b.object;
			if (initialized) return (TOrdering)std::strong_ordering::greater;
			if (b.initialized) return (TOrdering)std::strong_ordering::less;
			return (TOrdering)std::strong_ordering::equal;
		}

		bool operator==(const Nullable<T>& b)const
			requires(std::equality_comparable<T>)
		{
			if (initialized && b.initialized) return object == b.object;
			return initialized == b.initialized;
		}

		/// <summary>Test if this nullable value is non-null.</summary>
		/// <returns>Returns true if it is non-null.</returns>
		operator bool()const
		{
			return initialized;
		}

		/// <summary>Return the data inside this nullable value</summary>
		/// <returns>The data inside this nullable value. It crashes when it is null.</returns>
		const T& Value()const
		{
			CHECK_ERROR(initialized, L"vl::Nullable<T>::Value()#Cannot unbox from an empty nullable value.");
			return object;
		}
	};

	template<typename T>
	const Nullable<T> Nullable<T>::Empty;
}

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#endif