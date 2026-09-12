#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Reference.hpp"
#include "Chicane/Box/Mesh/Group.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Mesh : public Asset
        {
        public:
            // File
            static constexpr inline const char* EXTENSION = ".bmsh";

            // Tag
            static constexpr inline const char* TAG = "Mesh";

            // Values
            static constexpr inline const char* DEFAULT_SOURCE = "Assets/Engine/Meshes/Shape/Cube.bmsh";
            static constexpr inline const char* SPHERE_SOURCE  = "Assets/Engine/Meshes/Shape/Sphere.bmsh";

        public:
            Mesh(const FileSystem::Path& inFilepath);
            virtual ~Mesh() = default;

        public:
            const std::vector<MeshGroup>& getGroups() const;
            void setGroups(const std::vector<MeshGroup>& inGroups);
            void appendGroup(const MeshGroup& inGroup);
            void updateGroup(const MeshGroup& inGroup);

            bool hasSkeleton() const;
            const AssetReference& getSkeleton() const;
            void setSkeleton(const FileSystem::Path& inSource);
            void setSkeleton(const AssetReference& inValue);

            const std::vector<AssetReference>& getAnimations() const;
            bool hasAnimation(const FileSystem::Path& inSource) const;
            void setAnimations(const std::vector<AssetReference>& inAnimations);
            void appendAnimation(const FileSystem::Path& inSource);
            void appendAnimation(const AssetReference& inValue);

        private:
            void fetchGroups();
            void fetchSkeleton();
            void fetchAnimations();
            void writeSkeleton();
            void writeAnimations();

        private:
            AssetReference              m_skeleton;
            std::vector<AssetReference> m_animations;
            std::vector<MeshGroup>      m_groups;
        };
    }
}