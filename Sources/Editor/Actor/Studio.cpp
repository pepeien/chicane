#include "Editor/Actor/Studio.reflected.hpp"

#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

namespace Editor
{
    namespace
    {
        struct StudioLight
        {
            Chicane::Vec3 color;
            Chicane::Vec3 view;
        };

        const StudioLight LIGHTS[] = {
            {{0.60f, 0.60f, 0.60f},   {-0.353f, 0.171f, -0.920f}},
            {{1.12f, 1.137f, 1.137f}, {-0.408f, 0.347f, 0.844f} },
            {{0.38f, 0.42f, 0.52f},   {0.522f, 0.826f, 0.213f}  },
            {{0.40f, 0.34f, 0.28f},   {0.625f, -0.562f, -0.542f}}
        };
    }

    Studio::Studio()
        : Chicane::Actor(),
          m_lights({})
    {
        setCanTick(true);
    }

    void Studio::onLoad()
    {
        for (std::size_t index = 0; index < LightCount; ++index)
        {
            Chicane::CLight* light = getScene()->createComponent<Chicane::CLight>();
            light->setType(Chicane::LightType::Directional);
            light->setCanCastShadows(false);
            light->setColor(LIGHTS[index].color);
            light->setIntensity(1.0f);
            light->activate();

            m_lights[index] = light;
        }

        syncLights();
    }

    void Studio::onTick(float inDeltaTime)
    {
        (void)inDeltaTime;

        syncLights();
    }

    void Studio::syncLights()
    {
        Chicane::Scene* scene = getScene();
        if (!scene)
        {
            return;
        }

        std::vector<Chicane::CCamera*> cameras = scene->getActiveComponents<Chicane::CCamera>();
        if (cameras.empty())
        {
            return;
        }

        Chicane::CCamera*   camera = cameras.back();
        const Chicane::Vec3 origin = camera->getTranslation();
        const Chicane::Vec3 right  = camera->getRight().normalize();
        const Chicane::Vec3 up     = camera->getUp().normalize();
        const Chicane::Vec3 look   = camera->getForward().normalize();

        for (std::size_t index = 0; index < LightCount; ++index)
        {
            Chicane::CLight* light = m_lights[index];
            if (!light)
            {
                continue;
            }

            const Chicane::Vec3& view    = LIGHTS[index].view;
            const Chicane::Vec3  toLight = (right * view.x + up * view.y - look * view.z).normalize();

            light->setAbsoluteTranslation(origin);
            light->lookAt(origin - toLight);
        }
    }
}
