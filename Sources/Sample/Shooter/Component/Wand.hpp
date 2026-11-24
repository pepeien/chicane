#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

CH_TYPE(Type = (Manual), Alias = (Wand))
class Wand : public Chicane::CMesh
{
public:
    static constexpr inline const char* ID = "Wand";

public:
    CH_CONSTRUCTOR()
    Wand();

public:
    void playFire(const Chicane::Vec3& inAim, const Chicane::Vec3& inImpact) const;

private:
    Chicane::Vec3 muzzle(const Chicane::Vec3& inAim) const;
};
