#include "Actor/Mesh.hpp"

namespace Chicane
{
    AEditorMesh::AEditorMesh()
        : Actor(),
        m_meshComponent(nullptr)
    {
        m_meshComponent = Application::getLevel()->createComponent<CMesh>();
        m_meshComponent->attachTo(this);
    }

    void AEditorMesh::setMesh(const std::string& inMesh)
    {
        m_meshComponent->setMesh(inMesh);
        m_meshComponent->activate();
    }
}