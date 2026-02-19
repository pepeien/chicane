#pragma once

#include <Chicane/Runtime/Scene/Actor/Light.hpp>

namespace Editor
{
    class Sun : public Chicane::ALight
    {
    public:
        Sun();

    protected:
        void onLoad() override;

    private:
        float m_distance;
    };
}