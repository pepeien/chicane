#include "Chicane/Drift/Interpolation.hpp"

#include <algorithm>
#include <cmath>

namespace Chicane
{
    namespace Drift
    {
        std::vector<float> mix(const std::vector<float>& inFrom, const std::vector<float>& inTo, float inTime)
        {
            const std::size_t count = std::min(inFrom.size(), inTo.size());

            std::vector<float> result;
            result.reserve(count);

            for (std::size_t i = 0; i < count; i++)
            {
                result.push_back(mix(inFrom.at(i), inTo.at(i), inTime));
            }

            return result;
        }
    }
}
