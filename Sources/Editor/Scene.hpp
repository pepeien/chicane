#pragma once

#include <Chicane/Runtime/Scene.hpp>

namespace Editor
{
    class Scene : public Chicane::Scene
    {
    public:
        Scene();

    public:
        void onLoad() override;

    private:
        void spawnSky();
        void spawnLights();
        void spawnCharacter();
        void spawnDefaultItem();
    };
}