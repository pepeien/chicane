#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Mesh/Group.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Mesh : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bmsh";
            static constexpr inline const char* TAG       = "Mesh";

            static constexpr inline const char* GROUP_TAG               = "Group";
            static constexpr inline const char* GROUP_ID_ATTRIBUTE_NAME = "id";

        public:
            Mesh(const FileSystem::Path& inFilepath);
            virtual ~Mesh() = default;

        public:
            const std::vector<MeshGroup>& getGroups() const;
            void setGroups(const std::vector<MeshGroup>& inGroups);
            void appendGroup(const MeshGroup& inGroup);
            void updateGroup(const MeshGroup& inGroup);

        private:
            void fetchGroups();

        private:
            std::vector<MeshGroup> m_groups;
        };
    }
}