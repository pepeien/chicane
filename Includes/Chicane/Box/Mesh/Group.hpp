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
            static constexpr inline const char* BONE_ATTRIBUTE_NAME        = "bone";
            static constexpr inline const char* EMISSIVE_STRENGTH_ATTRIBUTE_NAME = "emissiveStrength";

        public:
            bool isValid() const;

            const String& getId() const;
            void setId(const String& inId);

            const String& getBone() const;
            void setBone(const String& inBone);

            const AssetReference& getModel() const;
            void setModel(const pugi::xml_node& inNode, const FileSystem::Path& inBase = {});
            void setModel(const String& inSource, const String& inReference);

            const TextureMaps& getTextures() const;
            bool hasTexture(TextureMap inMap) const;
            const AssetReference& getTexture(TextureMap inMap = TextureMap::Base) const;
            void setTexture(TextureMap inMap, const pugi::xml_node& inNode, const FileSystem::Path& inBase = {});
            void setTexture(TextureMap inMap, const String& inSource, const String& inReference);
            void setTexture(const pugi::xml_node& inNode, const FileSystem::Path& inBase = {});
            void setTexture(const String& inSource, const String& inReference);
            void saveTextures(pugi::xml_node& outNode) const;

            const Transform& getTransform() const;
            const Mat4& getModelMatrix() const;
            void setTransform(const Transform& inTransform);
            void setTransform(const pugi::xml_node& inNode);
            void saveTransform(pugi::xml_node& outNode) const;

            float getEmissiveStrength() const;
            void setEmissiveStrength(float inValue);

        private:
            String         m_id               = "";
            String         m_bone             = "";
            AssetReference m_model            = {};
            TextureMaps    m_textures         = {};
            Transform      m_transform        = {};
            float          m_emissiveStrength = 1.0f;
        };
    }
}
