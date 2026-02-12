#pragma once

#include <map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Reference.hpp"
#include "Chicane/Box/Sky/Raw.hpp"
#include "Chicane/Box/Sky/Side.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Sky : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bsky";
            static constexpr inline const char* TAG       = "Sky";

            static constexpr inline const char* TEXTURES_TAG = "Textures";

        public:
            Sky(const FileSystem::Path& inFilepath);
            virtual ~Sky() = default;

        public:
            const AssetReference::List& getTextures() const;
            void addTexture(const String& inSource, const String& inReference);

            const AssetReference& getModel() const;
            void setModel(const String& inSource, const String& inReference);

        private:
            void fetchTextures();
            void fetchModel();

        private:
            AssetReference::List m_textures;
            AssetReference       m_model;
        };
    }
}