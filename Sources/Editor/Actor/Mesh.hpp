#pragma once

#include "Base.hpp"

namespace Chicane
{
    class AEditorMesh : public Actor
    {
    public:
        AEditorMesh();

    public:
        void setMesh(const std::string& inMesh);

    private:
        CMesh* m_meshComponent;
    };
}