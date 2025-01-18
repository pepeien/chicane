#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Sun : public Chicane::Light
{
public:
    Sun();

protected:
    void onTick(float inDeltaTime) override;

private:
    Chicane::Vec<3, float> m_switch;
    float                  m_radius;
    float                  m_rate;

    Chicane::MeshComponent* m_mesh;
};