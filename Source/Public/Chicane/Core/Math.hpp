#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<typename T = float, std::uint32_t O = 1, glm::qualifier Q = glm::packed_highp>
    struct Math
    {
        typedef glm::vec<O, T, Q> Vec;
        typedef glm::mat<O, O, T, Q> Mat;
        typedef glm::qua<T, Q> Quat;

        static bool isIdentical(const Vec& inA, const Vec& inB)
        {
            for (std::uint32_t i = 0; i < O; i++)
            {
                if (inA[i] != inB[i])
                {
                    return false;
                }
            }

            return true;
        }

        static bool isIdentical(const Mat& inA, const Mat& inB)
        {
            for (std::uint32_t i = 0; i < O; i++)
            {
                for (std::uint32_t j = 0; i < O; j++)
                {
                    if (inA[i][j] != inB[i][j])
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        static bool isIdentical(const Quat& inA, const Quat& inB)
        {
            for (std::uint32_t i = 0; i < 4; i++)
            {
                if (inA[i] != inB[i])
                {
                    return false;
                }
            }

            return true;
        }
    };
}