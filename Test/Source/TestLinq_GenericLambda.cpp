#include "AssertCollection.h"

TEST_FILE
{
	auto Square = [](auto x) { return x * x; };
	auto Double = [](auto x) { return x * 2; };
	auto Odd = [](auto x) { return x % 2 == 1; };
	auto Add = [](auto x, auto y) { return x + y; };
	auto And = [](auto x, auto y) { return x && y; };
	auto Or = [](auto x, auto y) { return x || y; };
	auto dividable = [](auto x, auto y) { return y % x == 0; };
	auto Compare = [](auto x, auto y) { return x - y; };

	TEST_CASE(L"Test Select()")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Select(Square).Select(Double));
		CHECK_LIST_ITEMS(dst, { 2 _ 8 _ 18 _ 32 _ 50 _ 72 _ 98 _ 128 _ 162 _ 200 });
		CompareEnumerable(dst, From(src).Select(Square).Select(Double));
	});

	TEST_CASE(L"Test Where()")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Where(Odd).Select(Square));
		CHECK_LIST_ITEMS(dst, { 1 _ 9 _ 25 _ 49 _ 81 });
		CopyFrom(dst, From(src).Where(Odd));
		CHECK_LIST_ITEMS(dst, { 1 _ 3 _ 5 _ 7 _ 9 });
		CompareEnumerable(dst, From(src).Where(Odd));
	});

	TEST_CASE(L"Test Cast()")
	{
		List<Ptr<Object>> src;
		src.Add(new WString(L"0"));
		src.Add(new WString(L"1"));
		src.Add(new WString(L"2"));
		src.Add(new AString("3"));
		src.Add(new AString("4"));
		src.Add(new AString("5"));

		List<vint> dst;
		CopyFrom(dst, From(src)
			.Cast<WString>()
			.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
		);
		CHECK_LIST_ITEMS(dst, { 0 _ 1 _ 2 _ - 1 _ - 1 _ - 1 });

		CopyFrom(dst, From(src)
			.FindType<WString>()
			.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
		);
		CHECK_LIST_ITEMS(dst, { 0 _ 1 _ 2 });
	});
}