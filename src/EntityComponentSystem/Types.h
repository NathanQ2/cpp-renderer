#pragma once

#include <bitset>

namespace PalmTree {
    using ComponentType = uint8_t;
    constexpr size_t MAX_COMPONENTS = 8;

    static constexpr size_t MAX_GAME_OBJECTS = 16;
    using Signature = std::bitset<MAX_COMPONENTS>;
    using Id = unsigned int;
}
