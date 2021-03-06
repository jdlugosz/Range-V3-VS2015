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
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/delimit.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

template<typename BidiRange>
struct my_reverse_view
  : ranges::view_adaptor<my_reverse_view<BidiRange>, BidiRange>
{
private:
#ifdef RANGES_WORKAROUND_MSVC_SFINAE_CONSTEXPR
    CONCEPT_ASSERT(ranges::BidirectionalRange<BidiRange>::value);
    CONCEPT_ASSERT(ranges::BoundedRange<BidiRange>::value);
#else
    CONCEPT_ASSERT(ranges::BidirectionalRange<BidiRange>());
    CONCEPT_ASSERT(ranges::BoundedRange<BidiRange>());
#endif
    friend ranges::range_access;
    using base_iterator_t = ranges::range_iterator_t<BidiRange>;

    struct adaptor : ranges::adaptor_base
    {
        // Cross-wire begin and end.
        base_iterator_t begin(my_reverse_view const &rng) const
        {
            return ranges::end(rng.base());
        }
        base_iterator_t end(my_reverse_view const &rng) const
        {
            return ranges::begin(rng.base());
        }
        void next(base_iterator_t &it)
        {
            --it;
        }
        void prev(base_iterator_t &it)
        {
            ++it;
        }
        ranges::range_reference_t<BidiRange> get(base_iterator_t it) const
        {
            return *ranges::prev(it);
        }
#ifdef RANGES_WORKAROUND_MSVC_SFINAE_CONSTEXPR
        CONCEPT_REQUIRES(ranges::RandomAccessRange<BidiRange>::value)
#else
        CONCEPT_REQUIRES(ranges::RandomAccessRange<BidiRange>())
#endif
        void advance(base_iterator_t &it, ranges::range_difference_t<BidiRange> n)
        {
            it -= n;
        }
#ifdef RANGES_WORKAROUND_MSVC_SFINAE_CONSTEXPR
        CONCEPT_REQUIRES(ranges::SizedIteratorRange<base_iterator_t, base_iterator_t>::value)
#else
        CONCEPT_REQUIRES(ranges::SizedIteratorRange<base_iterator_t, base_iterator_t>())
#endif
        ranges::range_difference_t<BidiRange>
        distance_to(base_iterator_t const &here, base_iterator_t const &there)
        {
            return here - there;
        }
    };
    adaptor begin_adaptor() const
    {
        return {};
    }
    adaptor end_adaptor() const
    {
        return {};
    }
public:
    using my_reverse_view::view_adaptor::view_adaptor;
};

struct my_delimited_range
  : ranges::view_adaptor<
        my_delimited_range,
        ranges::delimit_view<ranges::istream_range<int>, int>>
{
    using my_delimited_range::view_adaptor::view_adaptor;
};

int main()
{
    using namespace ranges;
    std::vector<int> v{1, 2, 3, 4};
    my_reverse_view<std::vector<int>& > retro{v};
    ::models<concepts::BoundedView>(retro);
    ::models<concepts::RandomAccessIterator>(retro.begin());
    ::check_equal(retro, {4, 3, 2, 1});

    std::list<int> l{1, 2, 3, 4};
    my_reverse_view<std::list<int>& > retro2{l};
    ::models<concepts::BoundedView>(retro2);
    ::models<concepts::BidirectionalIterator>(retro2.begin());
    ::models_not<concepts::RandomAccessIterator>(retro2.begin());
    ::check_equal(retro2, {4, 3, 2, 1});

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    my_delimited_range r{view::delimit(istream<int>(sinx), 42)};
    ::models<concepts::View>(r);
    ::models_not<concepts::BoundedView>(r);
    ::models<concepts::InputIterator>(r.begin());
    ::models_not<concepts::ForwardIterator>(r.begin());
    ::check_equal(r, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});

    return ::test_result();
}
