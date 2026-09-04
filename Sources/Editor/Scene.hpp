#pragma once

#include <Chicane/Runtime/Scene.hpp>

#include "Editor/Component/Gizmo.hpp"

namespace Editor
{
    class Scene : public Chicane::Scene
    {
    public:
        Scene();

    public:
        void onLoad() override;

        void setSelection(Chicane::Object* inItem);

    private:
        void spawnSky();
        void spawnLights();
        void spawnCharacter();
        void spawnDefaultItem();
        void spawnGizmo();

    private:
        Gizmo* m_gizmo;
    };
}
