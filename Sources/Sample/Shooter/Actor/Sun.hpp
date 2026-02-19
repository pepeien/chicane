#pragma once

#include <Chicane/Runtime/Scene/Actor/Light.hpp>

class Sun : public Chicane::ALight
{
public:
    Sun();

protected:
    void onLoad() override;

private:
    float m_distance;
};