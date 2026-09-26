#pragma once

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

namespace Editor
{
    struct SceneHelper
    {
        Chicane::CMesh*              mesh = nullptr;
        Chicane::EventSubscription<> subscription;
    };
}
