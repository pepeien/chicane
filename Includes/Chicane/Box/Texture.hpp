#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Texture : public Asset
        {
        public:
            // File
            static constexpr inline const char*         EXTENSION = ".btex";

            // Tag
            static constexpr inline const char*         TAG = "Texture";

            // Attributes
            static constexpr inline const char*         VENDOR_ATTRIBUTE_NAME = "vendor";
            static constexpr inline const char*         WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr inline const char*         HEIGHT_ATTRIBUTE_NAME = "height";
            static constexpr inline const char*         MIPS_ATTRIBUTE_NAME   = "mips";
            static constexpr inline const char*         NORMAL_ATTRIBUTE_NAME = "normal";
            static constexpr inline const char*         MIPS_TAG              = "Mips";
            static constexpr inline const char*         MIP_TAG               = "Mip";
            static constexpr inline const char*         LEVEL_ATTRIBUTE_NAME  = "level";

            // Values
            static constexpr inline const std::uint32_t FORMAT_VERSION    = 2;
            static constexpr inline const char*         DEFAULT_SOURCE    = "Assets/Engine/Textures/Checkered.btex";
            static constexpr inline const char*         DEFAULT_REFERENCE = "Checkered";
            static constexpr inline const char*         GREY_SOURCE       = "Assets/Engine/Textures/Color/Grey.btex";
            static constexpr inline const char*         GREY_REFERENCE    = "Color_Grey";

        public:
            static const Texture* getDefault();

        public:
            Texture(const FileSystem::Path& inFilepath);
            virtual ~Texture() = default;

        public:
            bool isEmpty() const;
            bool isAnimated() const;

            ImageVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(ImageVendor inValue);

            Image::Reference getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Image::Raw& inData);

            bool isNormal() const;
            void setNormal(bool inValue);

            std::shared_ptr<Image::MipChain> getMipChain(std::size_t inIndex = 0) const;
            bool bakeMips();

            std::size_t getFrameCount() const;
            int getDelay(std::size_t inIndex) const;
            Image::Reference getFrame(std::size_t inIndex) const;
            String getFrameId(std::size_t inIndex) const;

        private:
            void fetchVendorFromXML();
            void fetchDataFromXML();
            void rebuildFrames();
            void rebuildMipChains();
            void bakePreview();
            void writeMipsToXML(const Image::MipChain& inChain);
            bool fetchMipsFromXML();
            bool looksLikeNormal() const;

        private:
            ImageVendor                                   m_vendor;
            bool                                          m_bNormal;
            Image::Instance                               m_data;
            std::vector<Image::Instance>                  m_frames;
            std::vector<int>                              m_delays;
            std::vector<std::shared_ptr<Image::MipChain>> m_chains;
        };
    }
}