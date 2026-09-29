#include "Chicane/Smoke/Parse.hpp"

#include <algorithm>
#include <cmath>
#include <random>

#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Smoke
    {
        static std::mt19937& rng()
        {
            static thread_local std::mt19937 result(std::random_device{}());

            return result;
        }

        float randomFloat(float inFrom, float inTo)
        {
            if (inFrom == inTo)
            {
                return inFrom;
            }

            std::uniform_real_distribution<float> distribution(std::min(inFrom, inTo), std::max(inFrom, inTo));

            return distribution(rng());
        }

        float randomFloat()
        {
            return randomFloat(0.0f, 1.0f);
        }

        Vec3 randomDirection()
        {
            const float z = randomFloat(-1.0f, 1.0f);
            const float a = randomFloat(0.0f, Math::TWO_PI);
            const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));

            return Vec3(r * std::cos(a), r * std::sin(a), z);
        }

        Range parseRange(const String& inValue, const Range& inFallback)
        {
            const std::vector<String> values = inValue.split(',');
            if (values.empty())
            {
                return inFallback;
            }

            try
            {
                const float from = std::stof(values.at(0).trim().toStandard());
                if (values.size() < 2)
                {
                    return Range(from);
                }

                return Range(from, std::stof(values.at(1).trim().toStandard()));
            }
            catch (const std::exception&)
            {
                return inFallback;
            }
        }

        Range parseRange(const XmlNode& inNode, const char* inName, const Range& inFallback)
        {
            if (!inNode.hasAttribute(inName))
            {
                return inFallback;
            }

            return parseRange(inNode.getAttribute(inName), inFallback);
        }
    }
}
