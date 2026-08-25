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

            ImageVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(ImageVendor inValue);

            Image::Reference getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Image::Raw& inData);

        private:
            void fetchVendorFromXML();
            void fetchDataFromXML();

        private:
            ImageVendor     m_vendor;
            Image::Instance m_data;
        };
    }
}