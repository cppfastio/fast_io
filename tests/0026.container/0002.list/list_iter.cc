#include <fast_io_dsal/list.h>

struct X
{
	int a;
};

void test_iter()
{
	::fast_io::list<X> l{X{1}, X{2}, X{3}};

	auto iter = l.begin();
	{
		auto &&value = *iter;
		auto &&value2 = iter->a;
		auto value3 = *iter++;

		if (value.a != 1 || value2 != 1 || value3.a != 1)
		{
			::fast_io::fast_terminate();
		}
	}

	++iter;

	{
		auto &&value = *iter;
		auto &&value2 = iter->a;
		auto value3 = *iter++;

		if (value.a != 3 || value2 != 3 || value3.a != 3)
		{
			::fast_io::fast_terminate();
		}
	}

	if (iter != l.end())
	{
		::fast_io::fast_terminate();
	}
}

void test_iter2()
{
	::fast_io::list<X> l{X{1}, X{2}, X{3}};

	for (auto &&v : l)
	{
		if (v.a != 1 && v.a != 2 && v.a != 3)
		{
			::fast_io::fast_terminate();
		}
	}
}

void test_const_iter()
{
    ::fast_io::list<X> const l{X{1}, X{2}, X{3}};

	auto iter = l.begin();
	{
		auto &&value = *iter;
		auto &&value2 = iter->a;
		auto value3 = *iter++;

		if (value.a != 1 || value2 != 1 || value3.a != 1)
		{
			::fast_io::fast_terminate();
		}
	}

	++iter;

	{
		auto &&value = *iter;
		auto &&value2 = iter->a;
		auto value3 = *iter++;

		if (value.a != 3 || value2 != 3 || value3.a != 3)
		{
			::fast_io::fast_terminate();
		}
	}

	if (iter != l.end())
	{
		::fast_io::fast_terminate();
	}
}

void test_const_iter2()
{
	::fast_io::list<X> const l{X{1}, X{2}, X{3}};

	for (auto &&v : l)
	{
		if (v.a != 1 && v.a != 2 && v.a != 3)
		{
			::fast_io::fast_terminate();
		}
	}
}

void test_reverse_iter()
{
    ::fast_io::list<X> l{X{1}, X{2}, X{3}};
    auto iter = l.rbegin();
    {
        auto &&value = *iter;
        auto &&value2 = iter->a;
        auto value3 = *iter++;

        if (value.a != 3 || value2 != 3 || value3.a != 3)
        {
            ::fast_io::fast_terminate();
        }
    }
    {
        auto &&value = *iter;
        auto &&value2 = iter->a;
        auto value3 = *iter++;

        if (value.a != 2 || value2 != 2 || value3.a != 2)
        {
            ::fast_io::fast_terminate();
        }
    }
    ++iter;
    if (iter != l.rend())
    {
        ::fast_io::fast_terminate();
    }
}

void test_reverse_const_iter()
{
    ::fast_io::list<X> const l{X{1}, X{2}, X{3}};
    auto iter = l.crbegin();
    {
        auto &&value = *iter;
        auto &&value2 = iter->a;
        auto value3 = *iter++;

        if (value.a != 3 || value2 != 3 || value3.a != 3)
        {
            ::fast_io::fast_terminate();
        }
    }
    {
        auto &&value = *iter;
        auto &&value2 = iter->a;
        auto value3 = *iter++;

        if (value.a != 2 || value2 != 2 || value3.a != 2)
        {
            ::fast_io::fast_terminate();
        }
    }
    ++iter;
    if (iter != l.crend())
    {
        ::fast_io::fast_terminate();
    }
}

int main()
{
	::test_iter();
	::test_iter2();
	::test_const_iter();
    ::test_const_iter2();
	::test_reverse_iter();
	::test_reverse_const_iter();
}
