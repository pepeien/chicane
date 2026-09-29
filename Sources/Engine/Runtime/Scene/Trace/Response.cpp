#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"
#include "Chicane/Runtime/Scene/Trace/Response.reflected.hpp"

#include "Chicane/Core/Script/Handle.hpp"

namespace Chicane
{
    Object* SceneTraceResponse::getObject() const
    {
        if (!object || !Script::Handle::contains(object))
        {
            return nullptr;
        }

        return object;
    }

    Actor* SceneTraceResponse::getActor() const
    {
        return dynamic_cast<Actor*>(getObject());
    }
}
