#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        class Mesh : public Asset
        {
        public:
            struct Group
            {
            public:
                const std::string& getModel() const;
                void setModel(const std::string& inFilepath);

                const std::string& getTexture() const;
                void setTexture(const std::string& inFilepath);

            private:
                std::string m_model;
                std::string m_texture;
            };

        public:
            static inline std::string EXTENSION = ".bmsh";
            static inline std::string TAG       = "Mesh";

        public:
            Mesh(const std::string& inFilepath);

        public:
            const std::vector<Group>& getGroups() const;

        private:
            void fetchGroups();

        private:
            std::vector<Group> m_groups;
        };
    }
}