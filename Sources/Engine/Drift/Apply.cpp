#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Transformable.hpp"

#include <vector>

namespace Chicane
{
    namespace Drift
    {
        void applyPose(Transformable& inTarget, const std::vector<float>& inValue)
        {
            if (inValue.size() < 9)
            {
                return;
            }

            inTarget.setRelativeTranslation(Vec3(inValue.at(0), inValue.at(1), inValue.at(2)));
            inTarget.setRelativeRotation(Vec3(inValue.at(3), inValue.at(4), inValue.at(5)));
            inTarget.setRelativeScale(Vec3(inValue.at(6), inValue.at(7), inValue.at(8)));
        }
    }
}
