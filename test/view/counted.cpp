// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <range/v3/core.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;
    std::cout << "\nTesting counted\n";

    {
        int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto rng = view::counted(forward_iterator<int*>{rgi}, 10);
        rng.size();
        CONCEPT_ASSERT(SizedView<decltype(rng)>());
        auto i = rng.begin();
        auto b = i.base();
        auto c = i.count();
        decltype(i) j{b, c};
        ::check_equal(rng, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        static_assert(std::is_same<decltype(i), counted_iterator<forward_iterator<int*>>>::value, "");
    }

    {
        std::list<int> l;
        counted_iterator<std::list<int>::iterator> a(l.begin(), 0);
        counted_iterator<std::list<int>::const_iterator> b(l.begin(), 0);

        detail::ignore_unused(
            a-a,
            b-b,
            a-b,
            b-a);

        counted_iterator<char*> c(0,0);
        counted_iterator<char const*> d(0,0);
        detail::ignore_unused(
            c-c,
            d-d,
            c-d,
            d-c);
    }

    return ::test_result();
}
