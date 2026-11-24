#include "Sample/Shooter/Component/Wand.reflected.hpp"

#include <cmath>

#include <Chicane/Smoke/Engine.hpp>

constexpr inline const char* FIRE_EFFECT   = "Assets/Sample/Shooter/Effects/Wand/Fire.bvfx";
constexpr inline float       TIP_CLEARANCE = 0.03f;

Wand::Wand()
    : Chicane::CMesh()
{}

void Wand::playFire(const Chicane::Vec3& inAim, const Chicane::Vec3& inImpact) const
{
    const Chicane::Vec3 origin = muzzle(inAim);

    Chicane::Smoke::PlayInfo play;
    play.origin      = origin;
    play.destination = inImpact;
    play.bHasBeam    = true;
    play.transform.setTranslation(origin);

    Chicane::Smoke::Engine::sInstance().play(FIRE_EFFECT, play);
}

Chicane::Vec3 Wand::muzzle(const Chicane::Vec3& inAim) const
{
    const float halfLength = std::fabs(getScale().y) * 0.5f;

    return getTranslation() + getForward() * halfLength + inAim * TIP_CLEARANCE;
}
