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

        public:
            Texture(const FileSystem::Path& inFilepath);
            virtual ~Texture() = default;

        public:
            ImageVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(ImageVendor inValue);

            const Image& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Image::Raw& inData);

        private:
            void fetchVendorFromXML();
            void fetchDataFromXML();

        private:
            ImageVendor m_vendor;
            Image       m_data;
        };
    }
}