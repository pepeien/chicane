#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Game/Actor/Component.hpp"

namespace Chicane
{
    class MeshComponent : public ActorComponent
    {
    public:
        MeshComponent();

    public:
        void onActivation() override;

    public:
        bool hasMesh() const;
        const Box::Instance& getMesh() const;
        void setMesh(const std::string& inMesh);

    protected:
        Box::Instance m_mesh;
    };
}