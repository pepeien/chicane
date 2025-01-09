#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        class Mesh : public Asset::Instance
        {
        public:
            struct Group
            {
            public:
                bool isValid() const;

                const std::string& getId() const;
                void setId(const std::string& inId);

                const std::string& getModel() const;
                void setModel(const std::string& inFilepath);

                const std::string& getTexture() const;
                void setTexture(const std::string& inFilepath);

            private:
                std::string m_id      = "";
                std::string m_model   = "";
                std::string m_texture = "";
            };

        public:
            static inline std::string EXTENSION = ".bmsh";
            static inline std::string TAG       = "Mesh";

        public:
            Mesh(const std::string& inFilepath);

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