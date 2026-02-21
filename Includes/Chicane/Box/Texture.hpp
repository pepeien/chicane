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
            static constexpr inline const char*         EXTENSION = ".btex";
            static constexpr inline const char*         TAG       = "Texture";

            static constexpr inline const char*         VENDOR_ATTRIBUTE_NAME = "vendor";

            static constexpr inline const std::uint32_t MAX_COUNT = 1000;

        public:
            Texture(const FileSystem::Path& inFilepath);
            virtual ~Texture() = default;

        public:
            ImageVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(ImageVendor inValue);

            const Image::Raw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Image::Raw& inData);

        private:
            void fetchVendor();
            void fetchData();

        private:
            ImageVendor m_vendor;
            Image::Raw  m_data;
        };
    }
}