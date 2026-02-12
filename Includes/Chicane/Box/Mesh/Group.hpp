#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Reference.hpp"

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

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

            const AssetReference& getModel() const;
            void setModel(const pugi::xml_node& inNode);
            void setModel(const String& inSource, const String& inReference);

            const AssetReference& getTexture() const;
            void setTexture(const pugi::xml_node& inNode);
            void setTexture(const String& inSource, const String& inReference);

        private:
            String         m_id      = "";
            AssetReference m_model   = {};
            AssetReference m_texture = {};
        };
    }
}