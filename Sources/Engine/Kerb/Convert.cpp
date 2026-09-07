#include "Convert.hpp"

namespace Chicane
{
    namespace Kerb
    {
        namespace Convert
        {
            // Engine is Z-up. Jolt is Y-up. (x, y, z) -> (x, z, -y) is a -90 deg rotation around X.
            const JPH::Quat BASIS = JPH::Quat::sRotation(JPH::Vec3::sAxisX(), -JPH::JPH_PI * 0.5f);
            const JPH::Quat BASIS_CONJUGATE = BASIS.Conjugated();

            JPH::Vec3 toPhysicsPosition(const Vec3& inValue)
            {
                return JPH::Vec3(inValue.x, inValue.z, -inValue.y);
            }

            JPH::Vec3 toPhysicsSize(const Vec3& inValue)
            {
                return JPH::Vec3(inValue.x, inValue.z, inValue.y);
            }

            JPH::Quat toPhysicsRotation(const QuatFloat& inValue)
            {
                const JPH::Quat orientation(inValue.x, inValue.y, inValue.z, inValue.w);

                return BASIS * orientation * BASIS_CONJUGATE;
            }

            JPH::EMotionType toPhysicsMotion(MotionType inValue)
            {
                switch (inValue)
                {
                case MotionType::Dynamic:
                    return JPH::EMotionType::Dynamic;

                case MotionType::Kinematic:
                    return JPH::EMotionType::Kinematic;

                default:
                    return JPH::EMotionType::Static;
                }
            }

            Vec3 toEnginePosition(JPH::Vec3 inValue)
            {
                return Vec3(inValue.GetX(), -inValue.GetZ(), inValue.GetY());
            }

            QuatFloat toEngineRotation(JPH::Quat inValue)
            {
                const JPH::Quat orientation = BASIS_CONJUGATE * inValue * BASIS;

                return QuatFloat(orientation.GetW(), orientation.GetX(), orientation.GetY(), orientation.GetZ());
            }
        }
    }
}
