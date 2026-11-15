#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/Physics.hpp>

CH_TYPE(Type = (Manual), Alias = (Structure))
class Strcuture : public Chicane::Actor
{
public:
    CH_CONSTRUCTOR()
    Strcuture();

protected:
    void onLoad() override;

private:
    Chicane::CMesh*    m_mesh;
    Chicane::CPhysics* m_physics;
};