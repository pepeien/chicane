#include "Chicane/Runtime/Preview/Service.hpp"

#include <filesystem>

#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Material.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    PreviewService::PreviewService()
        : m_stageScene(nullptr),
          m_asset(),
          m_shape(SHAPE_SHADER_BALL),
          m_shapeModel(MODEL_SHADER_BALL),
          m_actors({}),
          m_tempFiles({})
    {}

    void PreviewService::openStage(Scene& inScene)
    {
        if (m_stageScene == &inScene)
        {
            return;
        }

        const FileSystem::Path path = Box::AssetPreview::sTrackPath();
        if (FileSystem::exists(path))
        {
            inScene.open(path);
        }

        m_stageScene = &inScene;
    }

    ACamera* PreviewService::spawnCamera(Scene& inScene)
    {
        if (Actor* existing = inScene.getActor(CAMERA_ID))
        {
            return dynamic_cast<ACamera*>(existing);
        }

        ACamera* camera = inScene.createActor<ACamera>();
        camera->setId(CAMERA_ID);

        return camera;
    }

    void PreviewService::activateCamera(Scene& inScene)
    {
        for (ACamera* camera : inScene.getActors<ACamera>())
        {
            if (!camera)
            {
                continue;
            }

            camera->activate();

            break;
        }
    }

    void PreviewService::setShape(const String& inShape)
    {
        m_shape = inShape.isEmpty() ? SHAPE_SHADER_BALL : inShape;

        if (m_shape.equals(SHAPE_SPHERE))
        {
            m_shapeModel = MODEL_SPHERE;

            return;
        }

        if (m_shape.equals(SHAPE_TORUS))
        {
            m_shapeModel = MODEL_TORUS;

            return;
        }

        if (m_shape.equals(SHAPE_CUBE))
        {
            m_shapeModel = MODEL_CUBE;

            return;
        }

        m_shapeModel = MODEL_SHADER_BALL;
    }

    void PreviewService::setShapeModel(const String& inModel)
    {
        if (inModel.isEmpty())
        {
            return;
        }

        m_shapeModel = inModel;
    }

    const String& PreviewService::getShape() const
    {
        return m_shape;
    }

    const char* PreviewService::shapeModel() const
    {
        return m_shapeModel.toChar();
    }

    void PreviewService::show(Scene& inScene, const FileSystem::Path& inAsset, bool bBake)
    {
        clear(inScene);
        openStage(inScene);

        m_asset = inAsset;
        if (m_asset.isEmpty() || !FileSystem::exists(m_asset))
        {
            notify(inScene, Box::AssetType::Undefined, bBake);

            return;
        }

        const Box::AssetType type = Box::getTypeFromExtension(m_asset);
        switch (type)
        {
        case Box::AssetType::Material:
            spawnMaterial(inScene, m_asset);

            break;

        case Box::AssetType::Model:
            spawnModel(inScene, m_asset);

            break;

        case Box::AssetType::Mesh:
            spawnMesh(inScene, m_asset);

            break;

        case Box::AssetType::Sky:
            spawnSky(inScene, m_asset);

            break;

        default:
            break;
        }

        notify(inScene, type, bBake);
    }

    void PreviewService::notify(Scene& inScene, Box::AssetType inType, bool bBake)
    {
        openStage(inScene);
        inScene.receive(Box::AssetPreview::sEventName(inType, bBake), m_asset.toString());
    }

    void PreviewService::clear(Scene& inScene)
    {
        destroyActors(inScene);
        removeTempFiles();
        m_asset = {};
    }

    void PreviewService::detach(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        for (std::size_t i = 0; i < m_actors.size(); ++i)
        {
            if (m_actors[i] != inActor)
            {
                continue;
            }

            m_actors.erase(m_actors.begin() + static_cast<std::ptrdiff_t>(i));

            break;
        }
    }

    void PreviewService::bindAsset(const FileSystem::Path& inAsset)
    {
        m_asset = inAsset;
    }

    const FileSystem::Path& PreviewService::getAsset() const
    {
        return m_asset;
    }

    const std::vector<Actor*>& PreviewService::getActors() const
    {
        return m_actors;
    }

    FileSystem::Path PreviewService::writeTempMesh(const String& inId, const std::vector<Box::MeshGroup>& inGroups)
    {
        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-preview";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return {};
        }

        const FileSystem::Path path = FileSystem::Path(directory) / (inId + Box::Mesh::EXTENSION);

        Box::Mesh mesh(path);
        mesh.setId(inId);
        mesh.setGroups(inGroups);
        mesh.saveXML();

        m_tempFiles.push_back(path);

        return path;
    }

    FileSystem::Path PreviewService::writeGroupMesh(
        const Box::Mesh& inSource, const Box::MeshGroup& inGroup, const FileSystem::Path& inAsset
    )
    {
        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-preview";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return {};
        }

        const FileSystem::Path path =
            FileSystem::Path(directory) / (inAsset.stem().toString() + "_" + inGroup.getId() + Box::Mesh::EXTENSION);

        Box::MeshGroup slice = inGroup;
        slice.setTransform(Transform());

        Box::Mesh mesh(path);
        mesh.setId(inGroup.getId());
        mesh.setGroups({slice});
        if (inSource.hasSkeleton())
        {
            mesh.setSkeleton(inSource.getSkeleton());
        }
        for (const Box::AssetReference& animation : inSource.getAnimations())
        {
            mesh.appendAnimation(animation);
        }
        mesh.saveXML();

        m_tempFiles.push_back(path);

        return path;
    }

    Actor* PreviewService::spawnMeshActor(
        Scene& inScene, const FileSystem::Path& inMesh, const String& inId, bool bTransient
    )
    {
        Actor* actor = inScene.createActor<Actor>();

        try
        {
            String actorId = inId.isEmpty() ? inMesh.stem().toString() : inId;
            if (actorId.isEmpty())
            {
                actorId = "Preview";
            }

            if (inScene.hasObject(actorId))
            {
                std::size_t index = 2;
                String      candidate;
                do
                {
                    candidate = String::sSprint("%s%d", actorId.toChar(), index);
                    index++;
                } while (inScene.hasObject(candidate));
                actorId = candidate;
            }
            actor->setId(actorId);

            CMesh* mesh = inScene.createComponent<CMesh>();
            if (bTransient)
            {
                mesh->setIsTransient(true);
            }
            mesh->setMesh(inMesh);
            mesh->attachTo(actor);
            mesh->activate();

            const std::vector<const Box::Animation*>& animations = mesh->getAnimations();
            if (!animations.empty() && animations.front())
            {
                mesh->playAnimation(animations.front()->getId());
            }
        }
        catch (...)
        {
            inScene.removeActor(actor);

            throw;
        }

        m_actors.push_back(actor);

        return actor;
    }

    void PreviewService::spawnMesh(Scene& inScene, const FileSystem::Path& inMesh)
    {
        const Box::Mesh mesh(inMesh);
        spawnMeshActor(inScene, inMesh, mesh.getId().isEmpty() ? inMesh.stem().toString() : mesh.getId());
    }

    void PreviewService::spawnModel(Scene& inScene, const FileSystem::Path& inModel)
    {
        const Box::Model model(inModel);
        if (model.getData().empty())
        {
            return;
        }

        std::vector<Box::MeshGroup> groups;
        for (const auto& [name, parsed] : model.getData())
        {
            if (parsed.vertices.empty() || name.isEmpty())
            {
                continue;
            }

            Box::MeshGroup group;
            group.setId(name);
            group.setModel(FileSystem::rootRelative(inModel).toString(), name);
            group.setMaterial(Box::Material::GRAY_SOURCE, Box::Material::GRAY_REFERENCE);
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

        const String           id   = model.getId().isEmpty() ? inModel.stem().toString() : model.getId();
        const FileSystem::Path path = writeTempMesh(id + "_model", groups);
        if (path.isEmpty())
        {
            return;
        }

        spawnMeshActor(inScene, path, id);
    }

    void PreviewService::spawnMaterial(Scene& inScene, const FileSystem::Path& inMaterial)
    {
        const Box::Material material(inMaterial);
        String              previewId = inMaterial.stem().toString();
        const String        reference = material.getId().isEmpty() ? previewId : material.getId();
        previewId                     = reference;

        Box::MeshGroup group;
        group.setId("Body");
        group.setModel(shapeModel(), Box::Model::DEFAULT_REFERENCE);
        group.setMaterial(FileSystem::rootRelative(inMaterial).toString(), reference);
        if (!group.isValid())
        {
            return;
        }

        String meshId = previewId;
        if (!m_shape.isEmpty())
        {
            meshId.append('_');
            meshId.append(m_shape);
        }

        const FileSystem::Path path = writeTempMesh(meshId, {group});
        if (path.isEmpty())
        {
            return;
        }

        spawnMeshActor(inScene, path, previewId);
    }

    void PreviewService::spawnSky(Scene& inScene, const FileSystem::Path& inSky)
    {
        const Box::Sky   sky(inSky);
        FileSystem::Path model = sky.getModel().getSource();
        if (model.isEmpty() || !FileSystem::exists(model))
        {
            model = sky.getKind() == Box::SkyKind::Panorama ? Box::Sky::DOME_SOURCE : Box::Sky::BOX_SOURCE;
        }

        spawnModel(inScene, model);
    }

    void PreviewService::destroyActors(Scene& inScene)
    {
        for (Actor* actor : m_actors)
        {
            if (!actor)
            {
                continue;
            }

            const std::vector<Component*> attachments = actor->getAttachments();
            for (Component* component : attachments)
            {
                if (!component)
                {
                    continue;
                }

                component->detach();
                inScene.removeComponent(component);
                delete component;
            }

            inScene.removeActor(actor);
            delete actor;
        }

        m_actors.clear();
    }

    void PreviewService::removeTempFiles()
    {
        for (const FileSystem::Path& path : m_tempFiles)
        {
            std::error_code error;
            std::filesystem::remove(path.toStandard(), error);
        }

        m_tempFiles.clear();
    }
}
