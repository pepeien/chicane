#include "Editor/Viewer/Scene.hpp"

#include <cmath>
#include <filesystem>
#include <stdexcept>

#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Material.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Sky.hpp>
#include <Chicane/Core/Math/Transform.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/View.hpp>

#include "Editor/Actor/Character.hpp"

namespace Editor
{
    static bool previewFocus(const std::vector<Chicane::Actor*>& inActors, Chicane::Vec3& outCenter, float& outRadius)
    {
        Chicane::Actor* actor = nullptr;
        for (Chicane::Actor* candidate : inActors)
        {
            if (!candidate || candidate->getBounds().getCorners().empty())
            {
                continue;
            }

            actor = candidate;

            break;
        }

        if (!actor)
        {
            return false;
        }

        outCenter                = actor->getCenter();
        const Chicane::Vec3 size = actor->getBounds().getSize();
        Chicane::Vec3       min  = outCenter - size * 0.5f;
        Chicane::Vec3       max  = outCenter + size * 0.5f;

        for (Chicane::Actor* candidate : inActors)
        {
            if (!candidate || candidate == actor || candidate->getBounds().getCorners().empty())
            {
                continue;
            }

            const Chicane::Vec3 center = candidate->getCenter();
            const Chicane::Vec3 half   = candidate->getBounds().getSize() * 0.5f;
            min                        = min.min(center - half);
            max                        = max.max(center + half);
        }

        outCenter                  = (min + max) * 0.5f;
        const Chicane::Vec3 extent = max - min;
        outRadius                  = 0.5f * std::sqrt(extent.dot(extent));

        return true;
    }

    static void previewView(const Chicane::Object* inObject, float& outFieldOfView, float& outAspectRatio)
    {
        outFieldOfView = 45.0f;
        outAspectRatio = 1.0f;
        if (!inObject)
        {
            return;
        }

        for (Chicane::Component* component : inObject->getAttachments())
        {
            Chicane::CView* view = dynamic_cast<Chicane::CView*>(component);
            if (!view)
            {
                continue;
            }

            outFieldOfView = view->getFieldOfView();
            if (view->getAspectRatio() > 0.0f)
            {
                outAspectRatio = view->getAspectRatio();
            }

            return;
        }
    }

    static Chicane::Vec3 parseLookTo(const Chicane::String& inValue, Chicane::Scene* inScene)
    {
        const Chicane::String value = inValue.trim();
        if (value.isEmpty())
        {
            return Chicane::Vec3::Zero();
        }

        Chicane::String raw = value;
        if (raw.startsWith("["))
        {
            raw = raw.substr(1);
        }

        if (raw.endsWith("]"))
        {
            raw = raw.substr(0, raw.size() - 1);
        }

        const std::vector<Chicane::String> parts = raw.split(',');
        if (parts.size() >= 3)
        {
            try
            {
                return Chicane::Vec3(
                    std::stof(parts.at(0).trim().toStandard()),
                    std::stof(parts.at(1).trim().toStandard()),
                    std::stof(parts.at(2).trim().toStandard())
                );
            }
            catch (const std::exception&)
            {}
        }

        if (!inScene)
        {
            return Chicane::Vec3::Zero();
        }

        Chicane::Object* target = inScene->getObject(value);
        if (!target)
        {
            return Chicane::Vec3::Zero();
        }

        return target->getAbsoluteTranslation();
    }

    ViewerScene::ViewerScene()
        : Scene(),
          m_asset(),
          m_previewType(Chicane::Box::AssetType::Undefined),
          m_previewShape(PREVIEW_SHAPE_SHADER_BALL),
          m_groups({}),
          m_tempMeshes({}),
          m_tempFiles({})
    {}

    ViewerScene::~ViewerScene()
    {
        removeTempFiles(m_tempMeshes);
        removeTempFiles(m_tempFiles);
    }

    void ViewerScene::onLoad()
    {
        spawnLights();
        spawnCharacter();
        spawnGizmo();
        loadPreviewStage(Chicane::Box::AssetType::Undefined);
    }

    void ViewerScene::setAsset(const Chicane::FileSystem::Path& inMesh)
    {
        if (m_asset == inMesh)
        {
            return;
        }

        clearPreview();

        m_asset = inMesh;
        if (m_asset.isEmpty() || !Chicane::FileSystem::exists(m_asset))
        {
            loadPreviewStage(Chicane::Box::AssetType::Undefined);

            return;
        }

        const Chicane::Box::AssetType type = Chicane::Box::getTypeFromExtension(m_asset);
        if (type == Chicane::Box::AssetType::Texture)
        {
            loadPreviewStage(Chicane::Box::AssetType::Undefined);

            return;
        }

        loadPreviewStage(type);

        if (type == Chicane::Box::AssetType::Sky)
        {
            spawnSky(m_asset);
            frameFromTrack();

            return;
        }

        if (type == Chicane::Box::AssetType::Model)
        {
            spawnModel(m_asset);
            frameFromTrack();

            return;
        }

        if (type == Chicane::Box::AssetType::Material)
        {
            spawnPreview();
            frameFromTrack();
            if (!m_groups.empty())
            {
                orientPreview(m_groups.back());
            }

            return;
        }

        if (type != Chicane::Box::AssetType::Mesh)
        {
            return;
        }

        Chicane::Box::Mesh mesh(m_asset);
        std::size_t        groupCount = 0;
        for (const Chicane::Box::MeshGroup& group : mesh.getGroups())
        {
            if (group.isValid())
            {
                groupCount++;
            }
        }

        if (mesh.hasSkeleton() || groupCount > 32)
        {
            Chicane::Actor* actor = createActor<Chicane::Actor>();
            actor->setId(mesh.getId());

            Chicane::CMesh* preview = createComponent<Chicane::CMesh>();
            preview->setIsTransient(true);
            preview->setMesh(m_asset);
            preview->attachTo(actor);
            preview->activate();

            const std::vector<const Chicane::Box::Animation*>& animations = preview->getAnimations();
            if (!animations.empty() && animations.front())
            {
                preview->playAnimation(animations.front()->getId());
            }

            m_groups.push_back(actor);
            appendOutline(actor, mesh.getGroups(), true);
            frameFromTrack();

            return;
        }

        for (const Chicane::Box::MeshGroup& group : mesh.getGroups())
        {
            if (!group.isValid())
            {
                continue;
            }

            const Chicane::FileSystem::Path slice = writeGroupMesh(mesh, group);
            if (slice.isEmpty())
            {
                continue;
            }

            Chicane::Actor* actor = createActor<Chicane::Actor>();
            actor->setId(group.getId());

            Chicane::CMesh* preview = createComponent<Chicane::CMesh>();
            preview->setIsTransient(true);
            preview->setMesh(slice);
            preview->attachTo(actor);
            preview->activate();
            const std::vector<const Chicane::Box::Animation*>& animations = preview->getAnimations();
            if (!animations.empty() && animations.front())
            {
                preview->playAnimation(animations.front()->getId());
            }

            actor->setAbsolute(group.getTransform());

            appendOutline(actor, {group}, false);

            m_groups.push_back(actor);
            m_tempMeshes.push_back(slice);
        }

        frameFromTrack();
    }

    void ViewerScene::setPreviewShape(const Chicane::String& inShape)
    {
        const Chicane::String next = inShape.isEmpty() ? PREVIEW_SHAPE_SHADER_BALL : inShape;
        if (m_previewShape.equals(next))
        {
            return;
        }

        m_previewShape = next;
        if (m_asset.isEmpty())
        {
            return;
        }

        const Chicane::Box::AssetType type = Chicane::Box::getTypeFromExtension(m_asset);
        if (type != Chicane::Box::AssetType::Material)
        {
            return;
        }

        const Chicane::FileSystem::Path current = m_asset;
        m_asset                                 = {};
        setAsset(current);
    }

    void ViewerScene::clearAsset()
    {
        clearPreview();
        loadPreviewStage(Chicane::Box::AssetType::Undefined);
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
                if (i < m_tempMeshes.size())
                {
                    std::error_code error;
                    std::filesystem::remove(m_tempMeshes[i].toStandard(), error);
                    m_tempMeshes.erase(m_tempMeshes.begin() + static_cast<std::ptrdiff_t>(i));
                }

                break;
            }
        }

        Scene::destroyObject(inObject);
    }

    void ViewerScene::commitGroups()
    {
        if (m_asset.isEmpty() || !Chicane::FileSystem::exists(m_asset))
        {
            return;
        }

        if (Chicane::Box::getTypeFromExtension(m_asset) != Chicane::Box::AssetType::Mesh)
        {
            return;
        }

        Chicane::Box::Mesh mesh(m_asset);
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
        return m_asset;
    }

    const Chicane::String& ViewerScene::getPreviewShape() const
    {
        return m_previewShape;
    }

    const std::vector<Chicane::Actor*>& ViewerScene::getGroups() const
    {
        return m_groups;
    }

    void ViewerScene::spawnModel(const Chicane::FileSystem::Path& inModel)
    {
        const Chicane::Box::Model model(inModel);
        if (model.getData().empty())
        {
            return;
        }

        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-viewer";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return;
        }

        const Chicane::FileSystem::Path path = Chicane::FileSystem::Path(directory) /
                                               (inModel.stem().toString() + "_model" + Chicane::Box::Mesh::EXTENSION);

        std::vector<Chicane::Box::MeshGroup> groups;
        for (const auto& [name, parsed] : model.getData())
        {
            if (parsed.vertices.empty() || name.isEmpty())
            {
                continue;
            }

            Chicane::Box::MeshGroup group;
            group.setId(name);
            group.setModel(inModel.toString(), name);
            group.setMaterial(Chicane::Box::Material::DEFAULT_SOURCE, Chicane::Box::Material::DEFAULT_REFERENCE);
            if (!group.isValid())
            {
                continue;
            }

            groups.push_back(group);
        }

        if (groups.empty())
        {
            return;
        }

        Chicane::Box::Mesh mesh(path);
        mesh.setId(model.getId().isEmpty() ? inModel.stem().toString() : model.getId());
        mesh.setGroups(groups);
        mesh.saveXML();

        spawnPreviewActor(path, mesh.getId(), groups, true);
        m_tempMeshes.push_back(path);
    }

    void ViewerScene::spawnPreview()
    {
        const Chicane::Box::AssetType type = Chicane::Box::getTypeFromExtension(m_asset);

        Chicane::String materialSource;
        Chicane::String materialReference;
        Chicane::String previewId = m_asset.stem().toString();

        if (type == Chicane::Box::AssetType::Material)
        {
            const Chicane::Box::Material material(m_asset);
            materialSource    = m_asset.toString();
            materialReference = material.getId().isEmpty() ? previewId : material.getId();
            previewId         = materialReference;
        }
        else
        {
            return;
        }

        Chicane::Box::MeshGroup group;
        group.setId("Body");
        group.setModel(previewModelSource(), Chicane::Box::Model::DEFAULT_REFERENCE);
        group.setMaterial(materialSource, materialReference);
        if (!group.isValid())
        {
            return;
        }

        const Chicane::FileSystem::Path path = writeTempMesh(previewId, {group});
        if (path.isEmpty())
        {
            return;
        }

        spawnPreviewActor(path, previewId, {group}, false);
        m_tempMeshes.push_back(path);
    }

    void ViewerScene::spawnPreviewActor(
        const Chicane::FileSystem::Path&            inMesh,
        const Chicane::String&                      inId,
        const std::vector<Chicane::Box::MeshGroup>& inGroups,
        bool                                        bGroups
    )
    {
        Chicane::Actor* actor = createActor<Chicane::Actor>();
        actor->setId(inId);

        Chicane::CMesh* preview = createComponent<Chicane::CMesh>();
        preview->setIsTransient(true);
        preview->setMesh(inMesh);
        preview->attachTo(actor);
        preview->activate();

        appendOutline(actor, inGroups, bGroups);

        m_groups.push_back(actor);
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
            const Chicane::String candidate = Chicane::String::sprint("%s %u", inBase.toChar(), index);
            if (!hasObject(candidate))
            {
                return candidate;
            }

            index++;
        }
    }

    Chicane::FileSystem::Path ViewerScene::previewTrackPath(Chicane::Box::AssetType inType) const
    {
        return Chicane::Box::AssetPreview::trackPath(inType);
    }

    void ViewerScene::loadPreviewStage(Chicane::Box::AssetType inType)
    {
        if (inType != m_previewType)
        {
            const Chicane::FileSystem::Path path = previewTrackPath(inType);
            if (Chicane::FileSystem::exists(path))
            {
                open(path);
                m_previewType = inType;
            }
        }

        frameFromTrack();
    }

    void ViewerScene::frameFromTrack()
    {
        Character* character = nullptr;
        for (Character* candidate : getActors<Character>())
        {
            character = candidate;

            break;
        }

        if (!character)
        {
            return;
        }

        Chicane::ACamera* stage = nullptr;
        for (Chicane::ACamera* camera : getActors<Chicane::ACamera>())
        {
            if (!camera || camera->isTransient())
            {
                continue;
            }

            stage = camera;

            break;
        }

        if (!stage)
        {
            return;
        }

        Chicane::Vec3 pivot  = parseLookTo(stage->lookTo, this);
        float         radius = 0.0f;
        Chicane::Vec3 center;
        if (previewFocus(m_groups, center, radius))
        {
            pivot = center;
        }

        float fieldOfView = 45.0f;
        float aspectRatio = 1.0f;
        previewView(character, fieldOfView, aspectRatio);

        character->frame(
            Chicane::Box::AssetPreview::cameraStart(
                pivot,
                stage->getAbsoluteTranslation(),
                Chicane::Box::AssetPreview::cameraDistance(radius, fieldOfView, aspectRatio)
            ),
            pivot
        );
    }

    void ViewerScene::spawnSky(const Chicane::FileSystem::Path& inSky)
    {
        const Chicane::Box::Sky   sky(inSky);
        Chicane::FileSystem::Path model = sky.getModel().getSource();
        if (model.isEmpty() || !Chicane::FileSystem::exists(model))
        {
            model = sky.getKind() == Chicane::Box::SkyKind::Panorama ? Chicane::Box::Sky::DOME_SOURCE
                                                                     : Chicane::Box::Sky::BOX_SOURCE;
        }

        spawnModel(model);
    }

    void ViewerScene::orientPreview(Chicane::Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        Character* character = nullptr;
        for (Character* candidate : getActors<Character>())
        {
            character = candidate;

            break;
        }

        if (!character)
        {
            return;
        }

        Chicane::Vec3 target = character->getAbsoluteTranslation();
        target.z             = inActor->getAbsoluteTranslation().z;
        inActor->lookAt(target);
    }

    void ViewerScene::clearPreview()
    {
        setSelection(nullptr);

        for (Chicane::Actor* actor : m_groups)
        {
            if (!actor)
            {
                continue;
            }

            Scene::destroyObject(actor);
        }

        m_groups.clear();
        removeTempFiles(m_tempMeshes);
        removeTempFiles(m_tempFiles);
        m_asset = {};
    }

    void ViewerScene::removeTempFiles(std::vector<Chicane::FileSystem::Path>& outFiles)
    {
        for (const Chicane::FileSystem::Path& path : outFiles)
        {
            std::error_code error;
            std::filesystem::remove(path.toStandard(), error);
        }

        outFiles.clear();
    }

    const char* ViewerScene::previewModelSource() const
    {
        if (m_previewShape.equals(PREVIEW_SHAPE_SPHERE))
        {
            return PREVIEW_MODEL_SPHERE;
        }

        if (m_previewShape.equals(PREVIEW_SHAPE_TORUS))
        {
            return PREVIEW_MODEL_TORUS;
        }

        if (m_previewShape.equals(PREVIEW_SHAPE_CUBE))
        {
            return PREVIEW_MODEL_CUBE;
        }

        if (m_previewShape.equals(PREVIEW_SHAPE_KNOB))
        {
            return PREVIEW_MODEL_KNOB;
        }

        return PREVIEW_MODEL_SHADER_BALL;
    }

    Chicane::FileSystem::Path ViewerScene::writeTempMesh(
        const Chicane::String& inId, const std::vector<Chicane::Box::MeshGroup>& inGroups
    ) const
    {
        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-viewer";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return {};
        }

        const Chicane::FileSystem::Path path =
            Chicane::FileSystem::Path(directory) / (inId + Chicane::Box::Mesh::EXTENSION);

        Chicane::Box::Mesh mesh(path);
        mesh.setId(inId);
        mesh.setGroups(inGroups);
        mesh.saveXML();

        return path;
    }

    Chicane::FileSystem::Path ViewerScene::writeGroupMesh(
        const Chicane::Box::Mesh& inSource, const Chicane::Box::MeshGroup& inGroup
    ) const
    {
        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-viewer";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return {};
        }

        const Chicane::FileSystem::Path path =
            Chicane::FileSystem::Path(directory) /
            (m_asset.stem().toString() + "_" + inGroup.getId() + Chicane::Box::Mesh::EXTENSION);

        Chicane::Box::MeshGroup slice = inGroup;
        slice.setTransform(Chicane::Transform());

        Chicane::Box::Mesh mesh(path);
        mesh.setId(inGroup.getId());
        mesh.setGroups({slice});
        if (inSource.hasSkeleton())
        {
            mesh.setSkeleton(inSource.getSkeleton());
        }
        for (const Chicane::Box::AssetReference& animation : inSource.getAnimations())
        {
            mesh.appendAnimation(animation);
        }
        mesh.saveXML();

        return path;
    }
}
