#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Texture : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".btex";
            static constexpr inline const char* TAG       = "Texture";

            static constexpr inline const char* VENDOR_ATTRIBUTE_NAME = "vendor";
            static constexpr inline const char* DEFAULT_SOURCE        = "Assets/Engine/Textures/Checkered.btex";
            static constexpr inline const char* DEFAULT_REFERENCE     = "Checkered";

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

            std::size_t getFrameCount() const;
            int getDelay(std::size_t inIndex) const;
            Image::Reference getFrame(std::size_t inIndex) const;
            String getFrameId(std::size_t inIndex) const;

        private:
            void fetchVendorFromXML();
            void fetchDataFromXML();
            void rebuildFrames();
            void bakePreview();

        private:
            ImageVendor                  m_vendor;
            Image::Instance              m_data;
            std::vector<Image::Instance> m_frames;
            std::vector<int>             m_delays;
        };
    }
}