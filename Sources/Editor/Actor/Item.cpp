#include "Actor/Item.hpp"

#include <Chicane/Runtime/Application.hpp>

namespace Editor
{
    Item::Item(const Chicane::FileSystem::Path& inMesh)
        : Chicane::Actor(),
          m_mesh(nullptr)
    {
        m_mesh = Chicane::Application::getInstance().getScene()->createComponent<Chicane::CMesh>();
        m_mesh->attachTo(this);
        m_mesh->setMesh(inMesh);
        m_mesh->activate();
    }
}