#pragma once

#include <Chicane/Core/FileSystem.hpp>

#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

namespace Editor
{
    class Item : public Chicane::Actor
    {
    public:
        Item(const Chicane::FileSystem::Path& inMesh);

    private:
        Chicane::CMesh* m_mesh;
    };
}