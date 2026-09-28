#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Trace/Response.reflected.hpp"

#include "Chicane/Core/Script/Handle.hpp"

namespace Chicane
{
    Actor* SceneTraceResponse::getActor() const
    {
        if (!actor || !Script::Handle::contains(actor))
        {
            return nullptr;
        }

        return actor;
    }
}
