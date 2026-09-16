#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Runtime/Scene/Actor/Light.hpp>

CH_TYPE(Type = (Manual))
class Sun : public Chicane::ALight
{
public:
    CH_CONSTRUCTOR()
    Sun();

protected:
    void onLoad() override;

private:
    float m_distance;
    float m_angle;
};