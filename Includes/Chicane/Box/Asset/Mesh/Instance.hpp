#pragma once

#include "Core.hpp"
#include "Box/Asset/Mesh/Group.hpp"
#include "Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            class CHICANE Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

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