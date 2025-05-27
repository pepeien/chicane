#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Mesh/Group.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);
                virtual ~Instance() = default;

            public:
                const std::vector<Group>& getGroups() const;
                void setGroups(const std::vector<Group>& inGroups);
                void appendGroup(const Group& inGroup);
                void updateGroup(const Group& inGroup);

            private:
                void fetchGroups();

            private:
                std::vector<Group> m_groups;
            };
        }
    }
}