#pragma once

#include <map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Reference.hpp"

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Texture/Map.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX MeshGroup
        {
        public:
            using TextureMaps = std::map<TextureMap, AssetReference>;

        public:
            static constexpr inline const char* TRANSLATION_ATTRIBUTE_NAME = "translation";
            static constexpr inline const char* ROTATION_ATTRIBUTE_NAME    = "rotation";
            static constexpr inline const char* SCALE_ATTRIBUTE_NAME       = "scale";

        public:
            bool isValid() const;

            const String& getId() const;
            void setId(const String& inId);

            const AssetReference& getModel() const;
            void setModel(const pugi::xml_node& inNode);
            void setModel(const String& inSource, const String& inReference);

            const TextureMaps& getTextures() const;
            bool hasTexture(TextureMap inMap) const;
            const AssetReference& getTexture(TextureMap inMap = TextureMap::Base) const;
            void setTexture(TextureMap inMap, const pugi::xml_node& inNode);
            void setTexture(TextureMap inMap, const String& inSource, const String& inReference);
            void setTexture(const pugi::xml_node& inNode);
            void setTexture(const String& inSource, const String& inReference);
            void saveTextures(pugi::xml_node& outNode) const;

            const Transform& getTransform() const;
            const Mat4& getModelMatrix() const;
            void setTransform(const Transform& inTransform);
            void setTransform(const pugi::xml_node& inNode);
            void saveTransform(pugi::xml_node& outNode) const;

        private:
            String         m_id        = "";
            AssetReference m_model     = {};
            TextureMaps    m_textures  = {};
            Transform      m_transform = {};
        };
    }
}
