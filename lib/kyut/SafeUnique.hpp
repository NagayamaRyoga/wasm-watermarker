#ifndef INCLUDE_kyut_SafeUnique_cpp
#define INCLUDE_kyut_SafeUnique_cpp

#include <cassert>
#include <vector>

namespace kyut {
    template <typename Iterator, typename Pred>
    Iterator safe_unique(Iterator begin, Iterator end, Pred pred) {
        if (begin == end) {
            return begin;
        }

        // This implementation is inefficient
        std::vector<typename std::iterator_traits<Iterator>::value_type> a, b;

        auto it = begin;
        a.emplace_back(std::move(*it++));

        while (it != end) {
            if (pred(a.back(), *it)) {
                b.emplace_back(std::move(*it));
            } else {
                a.emplace_back(std::move(*it));
            }
            ++it;
        }

        it = begin;
        for (std::size_t i = 0; i < a.size(); i++) {
            *it++ = std::move(a[i]);
        }

        const auto result = it;

        for (std::size_t i = 0; i < b.size(); i++) {
            *it++ = std::move(b[i]);
        }

        assert(it == end);
        return result;
    }
} // namespace kyut

#endif // INCLUDE_kyut_SafeUnique_cpp
