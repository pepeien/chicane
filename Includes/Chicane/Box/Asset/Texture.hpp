#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Texture : public Asset
        {
        public:
            static inline constexpr const char* EXTENSION = ".btex";
            static inline constexpr const char* TAG       = "Texture";

            static inline constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

            static inline constexpr const std::uint32_t MAX_COUNT = 1000;

        public:
            Texture(const FileSystem::Path& inFilepath);
            virtual ~Texture() = default;

        public:
            Image::Vendor getVendor() const;
            void setVendor(Image::Vendor inVendor);

            const Image::Raw& getData() const;
            void setData(const String& inFilepath);
            void setData(const Image::Raw& inData);

        private:
            void fetchVendor();
            void fetchData();

        private:
            Image::Vendor m_vendor;
            Image::Raw    m_data;
        };
    }
}