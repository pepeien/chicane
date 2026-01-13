#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene.hpp"

#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        LScene::LScene()
            : Super("Engine_Scene")
        {
            pushLayer<LSceneSky>();

            init();
        }
    }
}