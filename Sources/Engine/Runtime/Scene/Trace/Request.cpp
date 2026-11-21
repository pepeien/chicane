#include "Chicane/Runtime/Scene/Trace/Request.reflected.hpp"

#include "Chicane/Kerb/Collision/Preset/Info.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

#include "Chicane/Runtime/Scene/Component/Physics.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

#include <cfloat>
#include <cmath>

namespace Chicane
{
    static const CPhysics* findPhysics(const Object* inObject)
    {
        for (const Object* current = inObject; current; current = current->getParent())
        {
            if (const CPhysics* physics = dynamic_cast<const CPhysics*>(current))
            {
                return physics;
            }

            for (const Object* attachment : current->getAttachments())
            {
                if (const CPhysics* physics = dynamic_cast<const CPhysics*>(attachment))
                {
                    return physics;
                }
            }
        }

        return nullptr;
    }

    static Kerb::ObjectLayer physicsLayer(const CPhysics& inPhysics)
    {
        if (inPhysics.body.bUsePreset)
        {
            const Kerb::ObjectLayer layer =
                Kerb::resolveCollisionPreset(inPhysics.body.preset, inPhysics.body.motion).layer;
            if (layer != Kerb::ObjectLayer::Auto)
            {
                return layer;
            }
        }

        return Kerb::resolveObjectLayer(inPhysics.body.layer, inPhysics.body.motion);
    }

    bool SceneTraceRequest::isValid() const
    {
        return shape && shape->isValid() && cellSize > FLT_EPSILON && getLength() > FLT_EPSILON;
    }

    float SceneTraceRequest::getLength() const
    {
        const Vec3 delta = destination - origin;

        return std::sqrt(delta.dot(delta));
    }

    Vec3 SceneTraceRequest::getDirection() const
    {
        const Vec3  delta  = destination - origin;
        const float length = std::sqrt(delta.dot(delta));
        if (length <= FLT_EPSILON)
        {
            return Vec3::sZero();
        }

        return delta / length;
    }

    bool SceneTraceRequest::accepts(const Object* inObject) const
    {
        if (!inObject || preset == Kerb::CollisionPreset::Default)
        {
            return inObject != nullptr;
        }

        const CPhysics* physics = findPhysics(inObject);
        if (!physics || !physics->isActive())
        {
            return false;
        }

        const Kerb::ObjectLayer trace = Kerb::resolveCollisionPreset(preset, Kerb::MotionType::Dynamic).layer;

        return Kerb::canCollide(trace, physicsLayer(*physics));
    }

    bool SceneTraceRequest::intersects(const Bounds3D& inBounds, float& outEnter) const
    {
        if (!isValid())
        {
            return false;
        }

        return shape->intersects(origin, destination, inBounds, outEnter);
    }
}
