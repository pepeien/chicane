#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Font/Types.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"
#include "Chicane/Box/Essential.hpp"

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