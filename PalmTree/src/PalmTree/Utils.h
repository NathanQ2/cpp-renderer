#pragma once

#include <glm/glm.hpp>

namespace PalmTree {
    // from: https://stackoverflow.com/a/57595105
    template<typename T, typename... Rest>
    void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (HashCombine(seed, rest), ...);
    }

    inline glm::vec3 GetDiagonal(glm::mat3 mat) {
        return glm::vec3{mat[0][0], mat[1][1], mat[2][2]};
    }

    inline glm::mat3 DiagonalMat(glm::vec3 diagonal) {
        return glm::mat3{
            {
                diagonal[0],
                0,
                0
            },
            {
                0.0f,
                diagonal[1],
                0.0f,
            },
            {
                0.0f,
                0.0f,
                diagonal[2]
            }
        };
    }
} 
