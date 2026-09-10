#include "Chicane/Runtime/Scene/Component/Mesh.reflected.hpp"

#include <algorithm>
#include <unordered_map>

#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Animation/Clip.hpp"
#include "Chicane/Box/Animation/Keyframe.hpp"
#include "Chicane/Box/Animation/Loop.hpp"
#include "Chicane/Box/Animation/Track.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Skeleton.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Easing/Curve.hpp"
#include "Chicane/Drift/Loop.hpp"

namespace Chicane
{
    namespace
    {
        constexpr float TO_MILLISECONDS = 1000.0f;

        std::vector<float> packTransform(const Transform& inTransform)
        {
            const Vec3& translation = inTransform.getTranslation();
            const Vec3& rotation    = inTransform.getRotation().getAngles();
            const Vec3& scale       = inTransform.getScale();

            return {
                translation.x,
                translation.y,
                translation.z,
                rotation.x,
                rotation.y,
                rotation.z,
                scale.x,
                scale.y,
                scale.z
            };
        }

        Transform unpackTransform(const std::vector<float>& inValue, const Transform& inFallback)
        {
            if (inValue.size() < 9)
            {
                return inFallback;
            }

            Transform transform;
            transform.setTranslation(Vec3(inValue.at(0), inValue.at(1), inValue.at(2)));
            transform.setRotation(Vec3(inValue.at(3), inValue.at(4), inValue.at(5)));
            transform.setScale(Vec3(inValue.at(6), inValue.at(7), inValue.at(8)));

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
            clip.duration   = inClip.duration * TO_MILLISECONDS;
            clip.loop       = toDriftLoop(inClip.loop);
            clip.iterations = inClip.iterations;

            for (const Box::AnimationTrack& track : inClip.tracks)
            {
                Drift::Track converted(track.name);
                for (const Box::AnimationKeyframe& keyframe : track.keyframes)
                {
                    converted.addKeyframe(
                        keyframe.time * TO_MILLISECONDS,
                        packTransform(keyframe.transform),
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

    const std::vector<const Box::Animation*>& CMesh::getAnimations() const
    {
        return m_animations;
    }

    bool CMesh::hasAnimation(const String& inId) const
    {
        return m_animationById.find(inId) != m_animationById.end();
    }

    const Box::Animation* CMesh::getAnimation(const String& inId) const
    {
        const auto found = m_animationById.find(inId);
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
        const auto found = m_animationById.find(inId);
        if (found == m_animationById.end())
        {
            return;
        }

        const Box::Animation* animation = found->second;
        m_animations.erase(std::remove(m_animations.begin(), m_animations.end(), animation), m_animations.end());
        m_animationById.erase(found);
        m_queue.remove(inId);

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
        if (!m_queue.has(inId))
        {
            return;
        }

        m_queue.play(inId);
        setCanTick(true);
        evaluatePose();
    }

    void CMesh::queueAnimation(const String& inId)
    {
        if (!m_queue.has(inId))
        {
            return;
        }

        m_queue.enqueue(inId);
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

        const std::int32_t index = m_skeleton->findIndex(inGroup.getId());
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

    void CMesh::evaluatePose()
    {
        m_skins.clear();
        if (!m_skeleton)
        {
            return;
        }

        const Box::SkeletonBoneEntry::List& entries = m_skeleton->getEntries();
        m_skins.resize(entries.size(), Mat4::One);

        std::vector<Mat4> worlds(entries.size(), Mat4::One);

        for (std::size_t i = 0; i < entries.size(); i++)
        {
            Transform local = entries.at(i).transform;
            if (m_queue.isPlaying())
            {
                local = unpackTransform(m_queue.sample(entries.at(i).id), local);
            }

            if (entries.at(i).parentIndex >= 0)
            {
                worlds[i] = worlds.at(static_cast<std::size_t>(entries.at(i).parentIndex)) * local.getMatrix();
            }
            else
            {
                worlds[i] = local.getMatrix();
            }

            m_skins[i] = worlds[i] * entries.at(i).inverseBind;
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
