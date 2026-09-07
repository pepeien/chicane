#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Body/MotionType.h>

#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    namespace Kerb
    {
        namespace Convert
        {
            JPH::Vec3 toPhysicsPosition(const Vec3& inValue);
            JPH::Vec3 toPhysicsSize(const Vec3& inValue);
            JPH::Quat toPhysicsRotation(const QuatFloat& inValue);
            JPH::EMotionType toPhysicsMotion(MotionType inValue);

            Vec3 toEnginePosition(JPH::Vec3 inValue);
            QuatFloat toEngineRotation(JPH::Quat inValue);
        }
    }
}
