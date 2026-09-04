#pragma once

#include <memory>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Font/Family.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX AssetPreview
        {
        public:
            // Tag
            static constexpr inline const char* TAG = "Preview";

            // Attributes
            static constexpr inline const char* TYPE_ATTRIBUTE_NAME   = "type";
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME = "height";
            static constexpr inline const char* TEXTURE_PREFIX        = "preview:";

            // Values
            static constexpr inline int         SIZE        = 256;
            static constexpr inline int         CHANNELS    = 4;
            static constexpr inline std::size_t CACHE_LIMIT = 256;

        public:
            static String textureIdOf(const FileSystem::Path& inAsset);

            static std::unique_ptr<AssetPreview> create(
                const FileSystem::Path& inAsset, AssetType inType, const Image& inImage
            );
            static std::unique_ptr<AssetPreview> createFromGeometry(
                const FileSystem::Path& inAsset,
                const Vertex::List&     inVertices,
                const Vertex::Indices&  inIndices
            );
            static std::unique_ptr<AssetPreview> createFromFont(
                const FileSystem::Path& inAsset, const FontFamily& inFamily, const String& inLabel
            );
            static std::unique_ptr<AssetPreview> createFromSound(
                const FileSystem::Path&             inAsset,
                const std::vector<unsigned char>&   inData
            );
            static std::unique_ptr<AssetPreview> createFromSky(
                const FileSystem::Path&                 inAsset,
                const Vertex::List&                     inVertices,
                const Vertex::Indices&                  inIndices,
                const std::vector<Image::Instance>&     inFaces
            );
            static bool write(pugi::xml_node inRoot, AssetType inType, const Image& inImage);
            static bool bake(const FileSystem::Path& inAsset, AssetType inType, const Image& inImage);
            static std::unique_ptr<AssetPreview> read(const FileSystem::Path& inAsset);
            static std::unique_ptr<AssetPreview> read(const pugi::xml_node& inRoot);

        public:
            AssetPreview() = default;

        public:
            String textureId() const;

        public:
            FileSystem::Path path;
            AssetType        type = AssetType::Undefined;
            Image::Instance  image;
        };
    }
}
