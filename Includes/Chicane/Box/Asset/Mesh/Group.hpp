#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX MeshGroup
        {
        public:
            bool isValid() const;

            const String& getId() const;
            void setId(const String& inId);

            const String& getModel() const;
            void setModel(const String& inFilepath);

            const String& getTexture() const;
            void setTexture(const String& inFilepath);

        private:
            String m_id      = "";
            String m_model   = "";
            String m_texture = "";
        };
    }
}