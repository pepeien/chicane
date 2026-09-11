#pragma once

#include <array>

#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Light.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Studio : public Chicane::Actor
    {
    public:
        static constexpr inline std::size_t LIGHT_COUNT = 4;

    public:
        Studio();

    protected:
        void onLoad() override;
        void onTick(float inDeltaTime) override;

        void syncLights();

    private:
        std::array<Chicane::CLight*, LIGHT_COUNT> m_lights;
    };
}
