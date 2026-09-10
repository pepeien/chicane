#include "Editor/Actor/Item.reflected.hpp"

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
        m_asset->setMesh("Kenney.bmsh");
        m_asset->addAnimation("idle.bani");
        m_asset->addAnimation("walk.bani");
        m_asset->attachTo(this);
        m_asset->activate();

        m_asset->playAnimation("walk");
    }
}