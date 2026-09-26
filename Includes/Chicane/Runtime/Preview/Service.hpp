#pragma once

#include <vector>

#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Actor/Camera.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME PreviewService
    {
    public:
        static constexpr inline const char* CAMERA_ID = "Camera";

        static constexpr inline const char* SHAPE_SHADER_BALL = "Shader Ball";
        static constexpr inline const char* SHAPE_SPHERE      = "Sphere";
        static constexpr inline const char* SHAPE_TORUS       = "Torus";
        static constexpr inline const char* SHAPE_CUBE        = "Cube";

        static constexpr inline const char* MODEL_SHADER_BALL = "Assets/Engine/Models/ShaderBall.bmdl";
        static constexpr inline const char* MODEL_SPHERE      = "Assets/Engine/Models/Shape/Sphere.bmdl";
        static constexpr inline const char* MODEL_TORUS       = "Assets/Engine/Models/Shape/Torus.bmdl";
        static constexpr inline const char* MODEL_CUBE        = "Assets/Engine/Models/Shape/Cube.bmdl";

    public:
        static inline PreviewService& sInstance()
        {
            static PreviewService service;

            return service;
        }

    public:
        PreviewService();

    public:
        void openStage(Scene& inScene);
        ACamera* spawnCamera(Scene& inScene);
        void activateCamera(Scene& inScene);

        void setShape(const String& inShape);
        void setShapeModel(const String& inModel);
        const String& getShape() const;
        const char* shapeModel() const;

        void show(Scene& inScene, const FileSystem::Path& inAsset, bool bBake = false);
        void notify(Scene& inScene, Box::AssetType inType, bool bBake = false);
        void clear(Scene& inScene);
        void detach(Actor* inActor);
        void bindAsset(const FileSystem::Path& inAsset);

        const FileSystem::Path& getAsset() const;
        const std::vector<Actor*>& getActors() const;

        FileSystem::Path writeTempMesh(const String& inId, const std::vector<Box::MeshGroup>& inGroups);
        FileSystem::Path writeGroupMesh(
            const Box::Mesh& inSource, const Box::MeshGroup& inGroup, const FileSystem::Path& inAsset
        );

        Actor* spawnMeshActor(
            Scene& inScene, const FileSystem::Path& inMesh, const String& inId, bool bTransient = true
        );

    private:
        void spawnMesh(Scene& inScene, const FileSystem::Path& inMesh);
        void spawnModel(Scene& inScene, const FileSystem::Path& inModel);
        void spawnMaterial(Scene& inScene, const FileSystem::Path& inMaterial);
        void spawnSky(Scene& inScene, const FileSystem::Path& inSky);
        void destroyActors(Scene& inScene);
        void removeTempFiles();

    private:
        Scene*                        m_stageScene;
        FileSystem::Path              m_asset;
        String                        m_shape;
        String                        m_shapeModel;
        std::vector<Actor*>           m_actors;
        std::vector<FileSystem::Path> m_tempFiles;
    };
}
