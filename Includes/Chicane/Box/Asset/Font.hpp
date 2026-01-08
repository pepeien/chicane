#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Font/Raw.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Font : public Asset
        {
        public:
            static inline constexpr const char* EXTENSION = ".bfon";
            static inline constexpr const char* TAG       = "Font";

            static inline constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

            static inline constexpr const float BASE_SIZE = 16.0f;

        public:
            Font(const FileSystem::Path& inFilepath);
            virtual ~Font() = default;

        public:
            FontVendor getVendor() const;
            void setVendor(FontVendor inVendor);

            const FontRaw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const FontRaw& inData);

        private:
            void fetchVendor();
            void fetchData();

        private:
            FontVendor m_vendor;
            FontRaw    m_data;
        };
    }
}