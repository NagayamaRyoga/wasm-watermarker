#ifndef INCLUDE_kyut_side_effect_hpp
#define INCLUDE_kyut_side_effect_hpp

#include <cstdint>

namespace kyut {
    enum class SideEffect : std::int32_t {
        none = 0,
        read = 1,
        write = 2,
    };
} // namespace kyut

#endif // INCLUDE_kyut_side_effect_hpp
