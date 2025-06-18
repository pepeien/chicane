#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

class Strcuture : public Chicane::Actor
{
public:
    Strcuture();

private:
    Chicane::CMesh* m_mesh;
};