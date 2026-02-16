#pragma once

#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

class Strcuture : public Chicane::Actor
{
public:
    Strcuture();

protected:
    void onLoad() override;

private:
    Chicane::CMesh* m_mesh;
};