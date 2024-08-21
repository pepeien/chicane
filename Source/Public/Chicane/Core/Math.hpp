#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<typename T = float, int O = 1, glm::qualifier Q = glm::packed_highp>
    struct Math
    {
        typedef glm::vec<O, T, Q> Vec;
        typedef glm::mat<O, O, T, Q> Mat;
        typedef glm::qua<T, Q> Quat;
    };

    template<typename Type>
    struct Vec
    {
        typedef glm::vec<2, Type, glm::packed_highp> Two;
        typedef glm::vec<3, Type, glm::packed_highp> Three;
        typedef glm::vec<4, Type, glm::packed_highp> Four;
    };

    template<typename Type>
    struct Mat
    {
        typedef glm::mat<4, 4, Type, glm::packed_highp> Four;
    };

    template<typename Type>
    struct Quat
    {
        typedef glm::qua<Type, glm::defaultp> Default;
        typedef glm::qua<Type, glm::packed_highp> High;
    };

}