#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene.hpp"

#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene/Collision.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene/Mesh.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene/Shadow.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LScene::LScene()
            : Super("Engine_Scene")
        {
            pushLayer<LSceneSky>();
            pushLayer<LSceneShadow>();
            pushLayer<LSceneMesh>();
            pushLayer<LSceneCollision>();

            init();
        }
    }
}