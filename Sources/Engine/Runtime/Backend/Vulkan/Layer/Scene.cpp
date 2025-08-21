#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene.hpp"

#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene/Collision.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene/Mesh.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene/Shadow.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LScene::LScene()
            : Super("Engine_Scene")
        {
            m_children.push_back(new LSceneSky());
            m_children.push_back(new LSceneShadow());
            m_children.push_back(new LSceneMesh());
            m_children.push_back(new LSceneCollision());
        }
    }
}