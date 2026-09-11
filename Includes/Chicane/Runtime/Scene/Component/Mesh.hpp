#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Box/Mesh.hpp"

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Drift/Queue.hpp"

#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Mesh.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    namespace Box
    {
        class Animation;
        class Skeleton;
    }

    CH_TYPE(Manual)
    class CHICANE_RUNTIME CMesh : public Component
    {
    public:
        CMesh();

    protected:
        void onAttachment(Object* inRoot) override;
        void onTick(float inDeltaTime) override;

    public:
        bool isDrawable() const;

        bool hasMesh() const;
        void setMesh(const FileSystem::Path& inFilePath);
        const Box::Mesh* getMesh() const;

        bool hasSkeleton() const;
        const Box::Skeleton* getSkeleton() const;
        void appendDebugWireframe(
            Vertex::List&                          outLines,
            Renderer::DrawPoly3DCommandMesh::List& outMeshes,
            const Renderer::Draw::Reference&       inSphereReference,
            const Vec4&                            inColor
        ) const;

        const std::vector<const Box::Animation*>& getAnimations() const;
        bool hasAnimation(const String& inId) const;
        const Box::Animation* getAnimation(const String& inId) const;
        void addAnimation(const FileSystem::Path& inFilePath);
        void removeAnimation(const String& inId);
        void clearAnimations();

        void playAnimation(const String& inId);
        void queueAnimation(const String& inId);
        void stopAnimation();
        bool isPlayingAnimation() const;
        const String& getPlayingAnimation() const;

        Mat4 getGroupMatrix(const Box::MeshGroup& inGroup) const;

        Renderer::DrawPoly3DFlag getFlags() const;
        void setFlags(Renderer::DrawPoly3DFlag inValue);

        bool canCastShadows() const;
        void setCanCastShadows(bool inValue);

        bool isLit() const;
        void setIsLit(bool inValue);

        bool isForeground() const;
        void setIsForeground(bool inValue);

        bool isOutlined() const;
        void setIsOutlined(bool inValue);

    protected:
        void generateBounds();
        void bindSkeleton();
        void evaluatePose();
        std::int32_t findBoundBone(const Box::MeshGroup& inGroup) const;

        void setFlag(Renderer::DrawPoly3DFlag inFlag, bool inValue);

    protected:
        bool                                              m_bIsVisible;
        Renderer::DrawPoly3DFlag                          m_flags;

        const Box::Mesh*                                  m_asset;
        const Box::Skeleton*                              m_skeleton;

        std::vector<const Box::Animation*>                m_animations;
        std::unordered_map<String, const Box::Animation*> m_animationById;
        Drift::Queue                                      m_queue;
        std::vector<Mat4>                                 m_bones;
        std::vector<Mat4>                                 m_skins;
    };
}
