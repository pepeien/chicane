#pragma once

#include <map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Reference.hpp"

#include "Chicane/Core/Texture/Material.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Material : public Asset
        {
        public:
            using TextureMaps = std::map<TextureMaterial, AssetReference>;

        public:
            // File
            static constexpr inline const char* EXTENSION = ".bmat";

            // Tag
            static constexpr inline const char* TAG = "Material";

            // Values
            static constexpr inline const char* DEFAULT_SOURCE    = "Assets/Engine/Materials/Checkered.bmat";
            static constexpr inline const char* DEFAULT_REFERENCE = "Checkered";
            static constexpr inline const char* GRAY_SOURCE       = "Assets/Engine/Materials/Color/Gray.bmat";
            static constexpr inline const char* GRAY_REFERENCE    = "Color_Gray";

        public:
            static const Material* getDefault();

        public:
            Material(const FileSystem::Path& inFilepath);
            virtual ~Material() = default;

        public:
            bool isValid() const;

            const TextureMaps& getTextures() const;
            bool hasTexture(TextureMaterial inType) const;
            const AssetReference& getTexture(TextureMaterial inType = TextureMaterial::Albedo) const;
            void setTexture(TextureMaterial inType, const XmlNode& inNode, const FileSystem::Path& inBase = {});
            void setTexture(TextureMaterial inType, const String& inSource, const String& inReference);
            void setTexture(const String& inSource, const String& inReference);

        private:
            void fetchTextures();
            void saveTextures();

        private:
            TextureMaps m_textures = {};
        };
    }
}
