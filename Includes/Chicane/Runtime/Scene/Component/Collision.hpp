#pragma once

#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Model/Extracted.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CCollision : public Component
    {
    public:
        CCollision();
        virtual ~CCollision() = default;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;
        void setCanCollide(bool inValue);

        const Box::ModelExtracted& getShape() const;
        void setShape(const Box::Model* inAsset);
        void setShape(const String& inAsset);

    protected:
        bool isColliding(CCollision* inCollider) const;

    protected:
        bool                m_bCanCollide;

        Box::ModelExtracted m_shape;
    };
}