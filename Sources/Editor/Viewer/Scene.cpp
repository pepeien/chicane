#include "Editor/Viewer/Scene.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Math.hpp>
#include <Chicane/Core/Math/Transform.hpp>
#include <Chicane/Runtime/Preview/Service.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Character.hpp"

namespace Editor
{
    namespace
    {
        float projectedSpan(const Chicane::Vec3& inSize)
        {
            const Chicane::Vec3 look  = Chicane::Box::AssetPreview::VIEW_DIRECTION.normalize();
            Chicane::Vec3       right = look.cross(Chicane::Vec3::sUp());
            if (right.dot(right) <= Chicane::Box::AssetPreview::EXTENT_EPSILON)
            {
                right = Chicane::Vec3::sRight();
            }
            else
            {
                right = right.normalize();
            }

            const Chicane::Vec3 up   = right.cross(look).normalize();
            const Chicane::Vec3 half = inSize * 0.5f;
            const float spanX = std::abs(half.x * right.x) + std::abs(half.y * right.y) + std::abs(half.z * right.z);
            const float spanY = std::abs(half.x * up.x) + std::abs(half.y * up.y) + std::abs(half.z * up.z);

            return std::max(spanX, spanY);
        }
    }

    ViewerScene::ViewerScene()
        : Scene(),
          m_previewShape(PREVIEW_SHAPE_SHADER_BALL),
          m_groups({})
    {}

    ViewerScene::~ViewerScene()
    {
        Chicane::PreviewService::sInstance().clear(*this);
    }

    void ViewerScene::onLoad()
    {
        spawnLights();
        spawnCharacter();
        for (Character* character : getActors<Character>())
        {
            if (!character)
            {
                continue;
            }

            character->setId(Chicane::PreviewService::CAMERA_ID);
        }
        spawnGizmo();

        Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
        preview.openStage(*this);
        preview.notify(*this, Chicane::Box::AssetType::Undefined);
        frameCamera();
    }

    void ViewerScene::setAsset(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
        if (preview.getAsset() == inMesh)
        {
            return;
        }

        setSelection(nullptr);
        m_groups.clear();

        if (inMesh.isEmpty() || !Chicane::FileSystem::exists(inMesh))
        {
            preview.show(*this, {}, false);
            syncGroups();
            frameCamera();

            return;
        }

        const Chicane::Box::AssetType type = Chicane::Box::getTypeFromExtension(inMesh);
        if (type == Chicane::Box::AssetType::Mesh)
        {
            preview.clear(*this);
            preview.openStage(*this);
            preview.bindAsset(inMesh);

            showMesh(inMesh);

            preview.notify(*this, type);

            syncGroups();
            frameCamera();

            return;
        }

        preview.setShape(m_previewShape);
        preview.show(*this, inMesh, false);

        syncGroups();
        frameCamera();
    }

    void ViewerScene::setPreviewShape(const Chicane::String& inShape)
    {
        const Chicane::String next = inShape.isEmpty() ? PREVIEW_SHAPE_SHADER_BALL : inShape;
        if (m_previewShape.equals(next))
        {
            return;
        }

        m_previewShape = next;

        Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
        if (preview.getAsset().isEmpty())
        {
            return;
        }

        if (Chicane::Box::getTypeFromExtension(preview.getAsset()) != Chicane::Box::AssetType::Material)
        {
            return;
        }

        const Chicane::FileSystem::Path current = preview.getAsset();
        setAsset({});
        setAsset(current);
    }

    void ViewerScene::clearAsset()
    {
        setSelection(nullptr);
        m_groups.clear();
        Chicane::PreviewService::sInstance().show(*this, {}, false);
        syncGroups();
        frameCamera();
    }

    void ViewerScene::destroyObject(Chicane::Object* inObject)
    {
        if (Chicane::Actor* actor = dynamic_cast<Chicane::Actor*>(inObject))
        {
            for (std::size_t i = 0; i < m_groups.size(); ++i)
            {
                if (m_groups[i] != actor)
                {
                    continue;
                }

                m_groups.erase(m_groups.begin() + static_cast<std::ptrdiff_t>(i));
                Chicane::PreviewService::sInstance().detach(actor);

                break;
            }
        }

        Scene::destroyObject(inObject);
    }

    void ViewerScene::commitGroups()
    {
        const Chicane::FileSystem::Path& asset = Chicane::PreviewService::sInstance().getAsset();
        if (asset.isEmpty() || !Chicane::FileSystem::exists(asset))
        {
            return;
        }

        if (Chicane::Box::getTypeFromExtension(asset) != Chicane::Box::AssetType::Mesh)
        {
            return;
        }

        Chicane::Box::Mesh mesh(asset);
        for (Chicane::Actor* actor : m_groups)
        {
            if (!actor)
            {
                continue;
            }

            for (Chicane::Box::MeshGroup group : mesh.getGroups())
            {
                if (!group.getId().equals(actor->getId()))
                {
                    continue;
                }

                Chicane::Transform transform;
                transform.setTranslation(actor->getAbsoluteTranslation());
                transform.setRotation(actor->getAbsoluteRotation());
                transform.setScale(actor->getAbsoluteScale());
                group.setTransform(transform);
                mesh.updateGroup(group);

                break;
            }
        }

        mesh.saveXML();
    }

    const Chicane::FileSystem::Path& ViewerScene::getAsset() const
    {
        return Chicane::PreviewService::sInstance().getAsset();
    }

    const Chicane::String& ViewerScene::getPreviewShape() const
    {
        return m_previewShape;
    }

    const std::vector<Chicane::Actor*>& ViewerScene::getGroups() const
    {
        return m_groups;
    }

    void ViewerScene::showMesh(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
        Chicane::Box::Mesh       mesh(inMesh);

        std::size_t groupCount = 0;
        for (const Chicane::Box::MeshGroup& group : mesh.getGroups())
        {
            if (group.isValid())
            {
                groupCount++;
            }
        }

        if (mesh.hasSkeleton() || groupCount > 32)
        {
            Chicane::Actor* actor = preview.spawnMeshActor(*this, inMesh, mesh.getId());
            appendOutline(actor, mesh.getGroups(), true);

            return;
        }

        for (const Chicane::Box::MeshGroup& group : mesh.getGroups())
        {
            if (!group.isValid())
            {
                continue;
            }

            const Chicane::FileSystem::Path slice = preview.writeGroupMesh(mesh, group, inMesh);
            if (slice.isEmpty())
            {
                continue;
            }

            Chicane::Actor* actor = preview.spawnMeshActor(*this, slice, group.getId());
            actor->setAbsolute(group.getTransform());
            appendOutline(actor, {group}, false);
        }
    }

    void ViewerScene::syncGroups()
    {
        m_groups = Chicane::PreviewService::sInstance().getActors();
    }

    void ViewerScene::frameCamera()
    {
        Character* character = nullptr;
        for (Character* candidate : getActors<Character>())
        {
            if (!candidate || !candidate->getId().equals(Chicane::PreviewService::CAMERA_ID))
            {
                continue;
            }

            character = candidate;

            break;
        }

        if (!character)
        {
            return;
        }

        Chicane::Vec3 minPosition = Chicane::Vec3::sZero();
        Chicane::Vec3 maxPosition = Chicane::Vec3::sZero();
        bool          bHasBounds  = false;

        for (Chicane::Actor* actor : Chicane::PreviewService::sInstance().getActors())
        {
            if (!actor)
            {
                continue;
            }

            const Chicane::Vec3 size = actor->getBounds().getSize();
            if (size.dot(size) <= Chicane::Box::AssetPreview::EXTENT_EPSILON)
            {
                continue;
            }

            const Chicane::Vec3 half = size * 0.5f;
            const Chicane::Vec3 low  = actor->getCenter() - half;
            const Chicane::Vec3 high = actor->getCenter() + half;
            if (!bHasBounds)
            {
                minPosition = low;
                maxPosition = high;
                bHasBounds  = true;

                continue;
            }

            minPosition = minPosition.min(low);
            maxPosition = maxPosition.max(high);
        }

        const Chicane::Vec3 pivot = bHasBounds ? (minPosition + maxPosition) * 0.5f : Chicane::Vec3::sZero();
        const Chicane::Vec3 size  = bHasBounds ? maxPosition - minPosition : Chicane::Vec3::sZero();

        float fov    = 45.0f;
        float aspect = 1.0f;
        for (Chicane::Component* attachment : character->getAttachments())
        {
            Chicane::CCamera* camera = dynamic_cast<Chicane::CCamera*>(attachment);
            if (!camera)
            {
                continue;
            }

            fov    = camera->getFieldOfView();
            aspect = camera->getAspectRatio();
            if (aspect <= 0.0f)
            {
                aspect = 1.0f;
            }

            break;
        }

        const float         distance = Chicane::Box::AssetPreview::sCameraDistance(projectedSpan(size), fov, aspect);
        const Chicane::Vec3 start    = Chicane::Box::AssetPreview::sCameraStart(
            pivot,
            Chicane::Box::AssetPreview::VIEW_DIRECTION * -1.0f,
            distance
        );

        character->frame(start, pivot);
    }

    void ViewerScene::appendOutline(
        Chicane::Object* inParent, const std::vector<Chicane::Box::MeshGroup>& inGroups, bool bGroups
    )
    {
        if (!inParent)
        {
            return;
        }

        for (const Chicane::Box::MeshGroup& group : inGroups)
        {
            if (!group.isValid())
            {
                continue;
            }

            Chicane::Object* materialParent = inParent;
            if (bGroups)
            {
                Chicane::Component* node = createComponent<Chicane::Component>();
                node->setId(uniqueLabel(group.getId()));
                node->attachTo(inParent);
                materialParent = node;
            }

            Chicane::String label = "Material";
            if (!group.getMaterial().getReference().isEmpty())
            {
                label = group.getMaterial().getReference();
            }
            if (hasObject(label))
            {
                label = group.getId() + " Material";
            }

            Chicane::Component* slot = createComponent<Chicane::Component>();
            slot->setId(uniqueLabel(label));
            slot->attachTo(materialParent);
        }
    }

    Chicane::String ViewerScene::uniqueLabel(const Chicane::String& inBase) const
    {
        if (!hasObject(inBase))
        {
            return inBase;
        }

        std::uint32_t index = 2;
        while (true)
        {
            const Chicane::String candidate = Chicane::String::sSprint("%s %u", inBase.toChar(), index);
            if (!hasObject(candidate))
            {
                return candidate;
            }

            index++;
        }
    }
}
