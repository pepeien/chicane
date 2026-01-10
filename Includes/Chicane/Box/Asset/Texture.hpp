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
            static inline constexpr const char* EXTENSION = ".btex";
            static inline constexpr const char* TAG       = "Texture";

            static inline constexpr const char* VENDOR_ATTRIBUTE_NAME =
                "vendor";

            static inline constexpr const std::uint32_t MAX_COUNT = 1000;

        public:
            Texture(const FileSystem::Path& inFilepath);
            virtual ~Texture() = default;

        public:
            ImageVendor getVendor() const;
            void setVendor(ImageVendor inVendor);

            const Image::Raw& getData() const;
            void setData(const String& inFilepath);
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