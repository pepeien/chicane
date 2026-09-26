#pragma once

#include <memory>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Preview/GeometryBatch.hpp"
#include "Chicane/Box/Asset/Type.hpp"
#include "Chicane/Box/Font/Family.hpp"

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
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
            static constexpr inline const char* TAG       = "Preview";
            static constexpr inline const char* OPEN_TAG  = "<Preview";
            static constexpr inline const char* CLOSE_TAG = "</Preview>";

            // Attributes
            static constexpr inline const char* TYPE_ATTRIBUTE_NAME   = "type";
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME = "height";
            static constexpr inline const char* TEXTURE_PREFIX        = "preview:";

            // Values
            static constexpr inline int         SIZE        = 256;
            static constexpr inline int         CHANNELS    = 4;
            static constexpr inline std::size_t CACHE_LIMIT = 256;

            static constexpr inline Color::Rgba BACKGROUND_COLOR = Color::Rgba(36, 36, 40, 255);
            static constexpr inline Color::Rgba FOREGROUND_COLOR = Color::Rgba(236, 236, 240, 255);
            static constexpr inline Color::Rgba CLAY_COLOR       = Color::Rgba(228, 232, 236, 255);
            static constexpr inline Color::Rgba AXIS_COLOR       = Color::Rgba(58, 58, 64, 255);

            static constexpr inline Vec3        VIEW_DIRECTION  = Vec3(0.46f, -0.85f, -0.26f);
            static constexpr inline Vec3        LIGHT_DIRECTION = Vec3(-0.50f, 0.50f, 0.70f);
            static constexpr inline float       START_DISTANCE  = 2.0f;

            static constexpr inline const char* TRACK_PATH        = "Assets/Engine/Levels/Preview.track";
            static constexpr inline const char* EVENT_PREFIX      = "Preview::";
            static constexpr inline const char* EVENT_BAKE_SUFFIX = "::Bake";

            static constexpr inline float       GEOMETRY_FIT        = 0.82f;
            static constexpr inline float       CAMERA_FIT          = 0.88f;
            static constexpr inline float       GLYPH_FIT           = 0.78f;
            static constexpr inline float       WAVEFORM_HEIGHT     = 0.38f;
            static constexpr inline float       SHADE_MIN           = 0.22f;
            static constexpr inline float       SHADE_RANGE         = 0.78f;
            static constexpr inline float       SPACE_ADVANCE       = 0.35f;
            static constexpr inline float       PIXEL_CENTER        = 0.5f;
            static constexpr inline float       HALF                = 0.5f;
            static constexpr inline float       CHANNEL_MAX         = 255.0f;
            static constexpr inline float       AREA_EPSILON        = 1e-5f;
            static constexpr inline float       NORMAL_EPSILON      = 1e-8f;
            static constexpr inline float       EXTENT_EPSILON      = 1e-10f;
            static constexpr inline int         WAVEFORM_MARGIN     = 12;
            static constexpr inline std::size_t FONT_FALLBACK_COUNT = 4;

            static constexpr std::size_t        PREVIEW_EXTRACT_LIMIT = 512 * 1024;
            static constexpr std::size_t        PREVIEW_EXTRACT_CHUNK = 4096;

            static constexpr std::size_t        PNG_HEADER_SIZE = 8;
            static constexpr unsigned char      PNG_SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47};

            static constexpr std::size_t        WAV_HEADER_SIZE         = 12;
            static constexpr std::size_t        WAV_CHUNK_HEADER_SIZE   = 8;
            static constexpr std::size_t        WAV_CHUNK_SIZE_OFFSET   = 4;
            static constexpr std::size_t        WAV_FMT_MIN_SIZE        = 16;
            static constexpr std::size_t        WAV_FMT_CHANNELS_OFFSET = 2;
            static constexpr std::size_t        WAV_FMT_BITS_OFFSET     = 14;
            static constexpr std::uint16_t      WAV_FORMAT_PCM          = 1;
            static constexpr std::uint16_t      WAV_FORMAT_FLOAT        = 3;
            static constexpr std::uint16_t      WAV_BITS_PER_BYTE       = 8;
            static constexpr std::uint16_t      WAV_BITS_8              = 8;
            static constexpr std::uint16_t      WAV_BITS_16             = 16;
            static constexpr std::uint16_t      WAV_BITS_24             = 24;
            static constexpr std::uint16_t      WAV_BITS_32             = 32;
            static constexpr float              WAV_PCM8_BIAS           = 128.0f;
            static constexpr float              WAV_PCM16_SCALE         = 32768.0f;
            static constexpr float              WAV_PCM24_SCALE         = 8388608.0f;
            static constexpr float              WAV_PCM32_SCALE         = 2147483648.0f;
            static constexpr std::int32_t       WAV_PCM24_SIGN          = 0x800000;
            static constexpr std::int32_t       WAV_PCM24_EXTEND        = static_cast<std::int32_t>(0xFF000000);

        public:
            static String sTextureIdOf(const FileSystem::Path& inAsset);
            static FileSystem::Path sTrackPath();
            static String sEventName(AssetType inType, bool bBake = false);
            static float sCameraDistance(float inRadius, float inFieldOfView = 45.0f, float inAspectRatio = 1.0f);
            static Vec3 sCameraStart(const Vec3& inTarget, const Vec3& inFrom);
            static Vec3 sCameraStart(const Vec3& inTarget, const Vec3& inFrom, float inDistance);

            static std::unique_ptr<AssetPreview> sCreate(
                const FileSystem::Path& inAsset, AssetType inType, const Image& inImage
            );
            static std::unique_ptr<AssetPreview> sCreateFromGeometry(
                const FileSystem::Path&                  inAsset,
                const std::vector<PreviewGeometryBatch>& inBatches,
                const std::vector<Image::Instance>&      inFaces = {}
            );
            static std::unique_ptr<AssetPreview> sCreateFromGeometry(
                const FileSystem::Path&             inAsset,
                const Vertex::List&                 inVertices,
                const Vertex::Indices&              inIndices,
                const Image::Instance&              inTexture = {},
                const std::vector<Image::Instance>& inFaces   = {}
            );
            static std::unique_ptr<AssetPreview> sCreateFromFont(
                const FileSystem::Path& inAsset, const FontFamily& inFamily, const String& inLabel
            );
            static std::unique_ptr<AssetPreview> sCreateFromSound(
                const FileSystem::Path& inAsset, const std::vector<unsigned char>& inData
            );
            static std::unique_ptr<AssetPreview> sCreateFromSky(
                const FileSystem::Path&             inAsset,
                const Vertex::List&                 inVertices,
                const Vertex::Indices&              inIndices,
                const std::vector<Image::Instance>& inFaces
            );
            static bool sWrite(XmlNode inRoot, AssetType inType, const Image& inImage);
            static bool sBake(const FileSystem::Path& inAsset, AssetType inType, const Image& inImage);
            static std::unique_ptr<AssetPreview> sRead(const FileSystem::Path& inAsset);
            static std::unique_ptr<AssetPreview> sRead(const XmlNode& inRoot);

        public:
            AssetPreview() = default;

        public:
            String textureId() const;

        public:
            FileSystem::Path path;
            AssetType        type = AssetType::Undefined;
            Image::Instance  image;
        };

        std::unique_ptr<AssetPreview> decodePreview(const FileSystem::Path& inFilePath, bool inShouldUseStored = true);
    }
}
