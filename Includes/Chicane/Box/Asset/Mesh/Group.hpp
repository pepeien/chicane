#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            struct CHICANE Group
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
        }
    }
}