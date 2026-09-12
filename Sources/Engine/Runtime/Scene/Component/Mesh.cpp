#include "Chicane/Runtime/Scene/Component/Mesh.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Animation/Clip.hpp"
#include "Chicane/Box/Animation/Keyframe.hpp"
#include "Chicane/Box/Animation/Loop.hpp"
#include "Chicane/Box/Animation/Track.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Skeleton.hpp"
#include "Chicane/Box/Texture.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Easing/Curve.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Mesh.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"
#include "Chicane/Renderer/Debug.hpp"

namespace Chicane
{
    std::vector<float> packTransform(const Transform& inTransform)
    {
        const Vec3&      translation = inTransform.getTranslation();
        const QuatFloat& rotation    = inTransform.getRotation().get();
        const Vec3&      scale       = inTransform.getScale();

        return {
            translation.x,
            translation.y,
            translation.z,
            rotation.w,
            rotation.x,
            rotation.y,
            rotation.z,
            scale.x,
            scale.y,
            scale.z
        };
    }

    void alignPackedRotation(std::vector<float>& ioValue, const QuatFloat& inPrevious)
    {
        if (ioValue.size() < 7)
        {
            return;
        }

        const QuatFloat current(ioValue.at(3), ioValue.at(4), ioValue.at(5), ioValue.at(6));
        const float align = glm::dot(static_cast<const glm::quat&>(inPrevious), static_cast<const glm::quat&>(current));
        if (align >= 0.0f)
        {
            return;
        }

        ioValue.at(3) = -ioValue.at(3);
        ioValue.at(4) = -ioValue.at(4);
        ioValue.at(5) = -ioValue.at(5);
        ioValue.at(6) = -ioValue.at(6);
    }

    Transform unpackTransform(const std::vector<float>& inValue, const Transform& inFallback)
    {
        if (inValue.size() < 9)
        {
            return inFallback;
        }

        Transform transform;
        transform.setTranslation(Vec3(inValue.at(0), inValue.at(1), inValue.at(2)));
        if (inValue.size() >= 10)
        {
            const QuatFloat rotation(inValue.at(3), inValue.at(4), inValue.at(5), inValue.at(6));
            transform.setRotation(rotation.normalize());
            transform.setScale(Vec3(inValue.at(7), inValue.at(8), inValue.at(9)));
        }
        else
        {
            transform.setRotation(Vec3(inValue.at(3), inValue.at(4), inValue.at(5)));
            transform.setScale(Vec3(inValue.at(6), inValue.at(7), inValue.at(8)));
        }

        return transform;
    }

    Drift::Loop toDriftLoop(Box::AnimationLoop inValue)
    {
        switch (inValue)
        {
        case Box::AnimationLoop::Repeat:
            return Drift::Loop::Repeat;

        case Box::AnimationLoop::PingPong:
            return Drift::Loop::PingPong;

        default:
            return Drift::Loop::Once;
        }
    }

    Drift::Clip toDriftClip(const Box::AnimationClip& inClip)
    {
        Drift::Clip clip(inClip.name);
        clip.duration   = inClip.duration;
        clip.loop       = toDriftLoop(inClip.loop);
        clip.iterations = inClip.iterations;

        for (const Box::AnimationTrack& track : inClip.tracks)
        {
            Drift::Track converted(track.name);
            QuatFloat    previous(1.0f, 0.0f, 0.0f, 0.0f);
            bool         hasPrevious = false;

            for (const Box::AnimationKeyframe& keyframe : track.keyframes)
            {
                std::vector<float> packed = packTransform(keyframe.transform);
                if (hasPrevious)
                {
                    alignPackedRotation(packed, previous);
                }

                if (packed.size() >= 7)
                {
                    previous    = QuatFloat(packed.at(3), packed.at(4), packed.at(5), packed.at(6));
                    hasPrevious = true;
                }

                converted.addKeyframe(
                    keyframe.time,
                    packed,
                    keyframe.easing.isEmpty() ? Drift::EasingCurve::linear()
                                              : Drift::EasingCurve::fromString(keyframe.easing)
                );
            }

            clip.addTrack(converted);
        }

        return clip;
    }

    String animationId(const Box::Animation* inAnimation)
    {
        if (!inAnimation)
        {
            return "";
        }

        if (!inAnimation->getClip().name.isEmpty())
        {
            return inAnimation->getClip().name;
        }

        return inAnimation->getId();
    }

    CMesh::CMesh()
        : Component(),
          m_bIsVisible(false),
          m_flags(Renderer::DrawPoly3DFlag::Lit | Renderer::DrawPoly3DFlag::Shadow),
          m_asset(nullptr),
          m_skeleton(nullptr),
          m_animations({}),
          m_animationById({}),
          m_queue(),
          m_bones({}),
          m_skins({})
    {}

    void CMesh::onAttachment(Object* inParent)
    {
        if (!inParent)
        {
            return;
        }

        inParent->addBounds(getBounds());
    }

    bool CMesh::isDrawable() const
    {
        return hasMesh() && isActive() && m_bIsVisible;
    }

    bool CMesh::hasMesh() const
    {
        return m_asset != nullptr;
    }

    void CMesh::setMesh(const FileSystem::Path& inFilePath)
    {
        if (inFilePath.isEmpty())
        {
            return;
        }

        clearAnimations();
        m_skeleton = nullptr;
        m_asset    = Box::load<Box::Mesh>(inFilePath);

        bindSkeleton();
        if (m_asset)
        {
            for (const Box::AssetReference& animation : m_asset->getAnimations())
            {
                addAnimation(animation.getSource());
            }
        }

        generateBounds();
        evaluatePose();
    }

    const Box::Mesh* CMesh::getMesh() const
    {
        return m_asset;
    }

    bool CMesh::hasSkeleton() const
    {
        return m_skeleton != nullptr;
    }

    const Box::Skeleton* CMesh::getSkeleton() const
    {
        return m_skeleton;
    }

    void CMesh::appendDebugWireframe(
        Vertex::List&                          outLines,
        Renderer::DrawPoly3DCommandMesh::List& outMeshes,
        const Renderer::Draw::Reference&       inSphereReference,
        const Vec4&                            inColor
    ) const
    {
        if (!m_skeleton || m_bones.empty())
        {
            return;
        }

        const Box::SkeletonBoneEntry::List& entries = m_skeleton->getEntries();
        if (entries.size() != m_bones.size())
        {
            return;
        }

        const Mat4        meshWorld = getMatrix();
        std::vector<Vec3> positions(m_bones.size());
        Vec3              min = Vec3(0.0f);
        Vec3              max = Vec3(0.0f);

        for (std::size_t i = 0; i < m_bones.size(); i++)
        {
            positions[i] = (meshWorld * m_bones[i]).getTranslation();

            if (i == 0)
            {
                min = positions[i];
                max = positions[i];

                continue;
            }

            min.x = std::min(min.x, positions[i].x);
            min.y = std::min(min.y, positions[i].y);
            min.z = std::min(min.z, positions[i].z);
            max.x = std::max(max.x, positions[i].x);
            max.y = std::max(max.y, positions[i].y);
            max.z = std::max(max.z, positions[i].z);
        }

        const Vec3  extent    = max - min;
        const float jointSize = std::max(0.06f, std::max(std::max(extent.x, extent.y), extent.z) * 0.028f);

        outLines.reserve(outLines.size() + entries.size() * 2);
        outMeshes.reserve(outMeshes.size() + entries.size());

        for (std::size_t i = 0; i < entries.size(); i++)
        {
            const std::int32_t parentIndex = entries[i].parentIndex;
            if (parentIndex < 0 || static_cast<std::size_t>(parentIndex) >= positions.size())
            {
                continue;
            }

            Renderer::Debug::appendSegment(
                outLines,
                positions[static_cast<std::size_t>(parentIndex)],
                positions[i],
                inColor
            );
        }

        if (inSphereReference.isEmpty())
        {
            return;
        }

        const float jointRadius = jointSize * 0.42f;
        for (std::size_t i = 0; i < entries.size(); i++)
        {
            Transform transform;
            transform.setTranslation(positions[i]);
            transform.setScale(jointRadius);

            Renderer::DrawPoly3DCommandMesh joint;
            joint.model          = inSphereReference;
            joint.textures[static_cast<std::uint8_t>(TextureMap::Base)] = Box::Texture::GREY_REFERENCE;
            joint.instance.model = transform.getMatrix();
            joint.instance.flags = Renderer::DrawPoly3DFlag::Foreground;
            outMeshes.push_back(joint);
        }
    }

    const std::vector<const Box::Animation*>& CMesh::getAnimations() const
    {
        return m_animations;
    }

    bool CMesh::hasAnimation(const String& inId) const
    {
        return !resolveAnimationId(inId).isEmpty();
    }

    const Box::Animation* CMesh::getAnimation(const String& inId) const
    {
        const auto found = m_animationById.find(resolveAnimationId(inId));
        if (found == m_animationById.end())
        {
            return nullptr;
        }

        return found->second;
    }

    void CMesh::addAnimation(const FileSystem::Path& inFilePath)
    {
        if (inFilePath.isEmpty())
        {
            return;
        }

        const Box::Animation* animation = Box::load<Box::Animation>(inFilePath);
        if (!animation)
        {
            return;
        }

        if (!m_skeleton)
        {
            if (animation->hasSkeleton())
            {
                m_skeleton = Box::load<Box::Skeleton>(animation->getSkeleton().getSource());
            }
            else if (m_asset && m_asset->hasSkeleton())
            {
                m_skeleton = Box::load<Box::Skeleton>(m_asset->getSkeleton().getSource());
            }
        }

        const String id = animationId(animation);
        if (id.isEmpty())
        {
            return;
        }

        const auto found = m_animationById.find(id);
        if (found == m_animationById.end())
        {
            m_animations.push_back(animation);
        }
        else
        {
            for (const Box::Animation*& slot : m_animations)
            {
                if (slot == found->second)
                {
                    slot = animation;

                    break;
                }
            }
        }

        m_animationById[id] = animation;

        Drift::Clip clip = toDriftClip(animation->getClip());
        clip.name        = id;
        m_queue.add(clip);
    }

    void CMesh::removeAnimation(const String& inId)
    {
        const String id = resolveAnimationId(inId);
        const auto   found = m_animationById.find(id);
        if (found == m_animationById.end())
        {
            return;
        }

        const Box::Animation* animation = found->second;
        m_animations.erase(std::remove(m_animations.begin(), m_animations.end(), animation), m_animations.end());
        m_animationById.erase(found);
        m_queue.remove(id);

        if (m_queue.isIdle())
        {
            setCanTick(false);
            evaluatePose();
        }
    }

    void CMesh::clearAnimations()
    {
        stopAnimation();
        m_animations.clear();
        m_animationById.clear();
        m_queue.clear();
    }

    void CMesh::playAnimation(const String& inId)
    {
        const String id = resolveAnimationId(inId);
        if (id.isEmpty())
        {
            return;
        }

        m_queue.play(id);
        setCanTick(true);
        evaluatePose();
    }

    void CMesh::queueAnimation(const String& inId)
    {
        const String id = resolveAnimationId(inId);
        if (id.isEmpty())
        {
            return;
        }

        m_queue.enqueue(id);
        setCanTick(true);
        evaluatePose();
    }

    void CMesh::stopAnimation()
    {
        m_queue.stop();
        setCanTick(false);
        evaluatePose();
    }

    bool CMesh::isPlayingAnimation() const
    {
        return m_queue.isPlaying();
    }

    const String& CMesh::getPlayingAnimation() const
    {
        return m_queue.getPlaying();
    }

    Mat4 CMesh::getGroupMatrix(const Box::MeshGroup& inGroup) const
    {
        const Mat4& local = inGroup.getModelMatrix();
        if (m_skins.empty() || !m_skeleton)
        {
            return local;
        }

        const std::int32_t index = findBoundBone(inGroup);
        if (index < 0 || static_cast<std::size_t>(index) >= m_skins.size())
        {
            return local;
        }

        return m_skins.at(static_cast<std::size_t>(index)) * local;
    }

    void CMesh::onTick(float inDeltaTime)
    {
        m_queue.tick(inDeltaTime);

        evaluatePose();
    }

    void CMesh::bindSkeleton()
    {
        m_skeleton = nullptr;
        if (!m_asset || !m_asset->hasSkeleton())
        {
            return;
        }

        m_skeleton = Box::load<Box::Skeleton>(m_asset->getSkeleton().getSource());
    }

    String CMesh::resolveAnimationId(const String& inId) const
    {
        if (inId.isEmpty())
        {
            return "";
        }

        if (m_queue.has(inId) || m_animationById.find(inId) != m_animationById.end())
        {
            return inId;
        }

        for (const auto& [id, animation] : m_animationById)
        {
            if (animation && animation->getClip().getTrack(inId))
            {
                return id;
            }
        }

        return "";
    }

    std::int32_t CMesh::findBoundBone(const Box::MeshGroup& inGroup) const
    {
        if (!m_skeleton)
        {
            return -1;
        }

        const auto findIndex = [this](const String& inId) -> std::int32_t
        {
            if (inId.isEmpty())
            {
                return -1;
            }

            return m_skeleton->findIndex(inId);
        };

        const std::int32_t bound = findIndex(inGroup.getBone());
        if (bound >= 0)
        {
            return bound;
        }

        const std::int32_t named = findIndex(inGroup.getId());
        if (named >= 0)
        {
            return named;
        }

        const String& groupId = inGroup.getId();
        if (groupId.isEmpty())
        {
            return -1;
        }

        const Box::SkeletonBoneEntry::List& entries = m_skeleton->getEntries();
        std::int32_t                        best    = -1;
        std::size_t                         bestLen = 0;

        for (std::size_t i = 0; i < entries.size(); i++)
        {
            const String& boneId = entries.at(i).id;
            if (boneId.isEmpty() || boneId.size() < bestLen || !groupId.startsWith(boneId))
            {
                continue;
            }

            if (groupId.size() != boneId.size() && groupId.at(boneId.size()) != '_')
            {
                continue;
            }

            bestLen = boneId.size();
            best    = static_cast<std::int32_t>(i);
        }

        return best;
    }

    void CMesh::evaluatePose()
    {
        m_skins.clear();
        m_bones.clear();
        if (!m_skeleton)
        {
            return;
        }

        const Box::SkeletonBoneEntry::List& entries = m_skeleton->getEntries();
        m_skins.resize(entries.size(), Mat4::One);
        m_bones.resize(entries.size(), Mat4::One);

        for (std::size_t i = 0; i < entries.size(); i++)
        {
            Transform local = entries.at(i).transform;
            if (m_queue.isPlaying())
            {
                local = unpackTransform(m_queue.sample(entries.at(i).id), local);
            }

            if (entries.at(i).parentIndex >= 0)
            {
                m_bones[i] = m_bones.at(static_cast<std::size_t>(entries.at(i).parentIndex)) * local.getMatrix();
            }
            else
            {
                m_bones[i] = local.getMatrix();
            }

            m_skins[i] = m_bones[i] * entries.at(i).inverseBind;
        }
    }

    Renderer::DrawPoly3DFlag CMesh::getFlags() const
    {
        return m_flags;
    }

    void CMesh::setFlags(Renderer::DrawPoly3DFlag inValue)
    {
        m_flags = inValue;
    }

    bool CMesh::canCastShadows() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Shadow);
    }

    void CMesh::setCanCastShadows(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Shadow, inValue);
    }

    bool CMesh::isLit() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Lit);
    }

    void CMesh::setIsLit(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Lit, inValue);
    }

    bool CMesh::isForeground() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Foreground);
    }

    void CMesh::setIsForeground(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Foreground, inValue);
    }

    bool CMesh::isOutlined() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Outlined);
    }

    void CMesh::setIsOutlined(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Outlined, inValue);
    }

    void CMesh::setFlag(Renderer::DrawPoly3DFlag inFlag, bool inValue)
    {
        if (inValue)
        {
            m_flags |= inFlag;

            return;
        }

        m_flags &= ~inFlag;
    }

    void CMesh::generateBounds()
    {
        if (!hasMesh())
        {
            return;
        }

        static std::unordered_map<const Box::Mesh*, Bounds3D> boundsByMesh;

        auto cached = boundsByMesh.find(m_asset);
        if (cached == boundsByMesh.end())
        {
            Bounds3D bounds;

            for (const Box::MeshGroup& group : m_asset->getGroups())
            {
                const Box::Model* model = Box::load<Box::Model>(group.getModel().getSource());

                if (!model)
                {
                    model = Box::Model::getDefault();
                }

                if (!model)
                {
                    continue;
                }

                const Vertex::List& vertices = model->getModel(group.getModel().getReference()).vertices;
                const Mat4&         local    = group.getModelMatrix();

                Vertex::List transformed;
                transformed.reserve(vertices.size());
                for (const Vertex& vertex : vertices)
                {
                    Vertex copy   = vertex;
                    copy.position = local * vertex.position;
                    transformed.push_back(copy);
                }

                bounds.add(transformed);
            }

            cached = boundsByMesh.emplace(m_asset, std::move(bounds)).first;
        }

        addBounds(cached->second);

        if (!isAttached())
        {
            return;
        }

        m_parent->addBounds(getBounds());
    }
}
