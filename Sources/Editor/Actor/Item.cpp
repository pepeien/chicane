#include "Actor/Item.hpp"

#include <Chicane/Runtime/Scene.hpp>

namespace Editor
{
    Item::Item(const Chicane::FileSystem::Path& inMesh)
        : Chicane::Actor(),
          m_mesh(inMesh),
          m_asset(nullptr)
    {}

    void Item::onLoad()
    {
        m_asset = getScene()->createComponent<Chicane::CMesh>();
        m_asset->attachTo(this);
        m_asset->setMesh(m_mesh);
        m_asset->activate();
    }
}