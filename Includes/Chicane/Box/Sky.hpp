#pragma once

#include <map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Reference.hpp"
#include "Chicane/Box/Sky/Kind.hpp"
#include "Chicane/Box/Sky/Raw.hpp"
#include "Chicane/Box/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Sky : public Asset
        {
        public:
            // File
            static constexpr inline const char* EXTENSION = ".bsky";

            // Tag
            static constexpr inline const char* TAG          = "Sky";
            static constexpr inline const char* TEXTURES_TAG = "Textures";

            // Attributes
            static constexpr inline const char* KIND_ATTRIBUTE_NAME = "kind";

            // Values
            static constexpr inline const char* PREVIEW_SOURCE = "Assets/Engine/Skies/Preview.bsky";
            static constexpr inline const char* BOX_SOURCE     = "Assets/Engine/Models/Sky/Box.bmdl";
            static constexpr inline const char* DOME_SOURCE    = "Assets/Engine/Models/Sky/Dome.bmdl";

        public:
            Sky(const FileSystem::Path& inFilepath);
            virtual ~Sky() = default;

        public:
            SkyKind getKind() const;
            void setKind(SkyKind inKind);

            const AssetReference::List& getTextures() const;
            void addTexture(const std::vector<FileSystem::Path>& inFilepaths);
            void addTexture(const FileSystem::Path& inFilepath);
            void addTexture(const String& inSource, const String& inReference);

            const AssetReference& getModel() const;
            void setModel(const FileSystem::Path& inFilepath);
            void setModel(const String& inSource, const String& inReference);

        private:
            void fetchKind();
            void fetchTextures();
            void fetchModel();

        private:
            SkyKind              m_kind;
            AssetReference::List m_textures;
            AssetReference       m_model;
        };
    }
}