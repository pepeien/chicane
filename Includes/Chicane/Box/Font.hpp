#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Font/Raw.hpp"
#include "Chicane/Box/Font/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Font : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bfon";
            static constexpr inline const char* TAG       = "Font";

            static constexpr inline const char* VENDOR_ATTRIBUTE_NAME = "vendor";

            static constexpr inline const float BASE_SIZE = 16.0f;

        public:
            static FontVendor parseVendor(const String& inValue);
            static const String& getVendorExtension(FontVendor inValue);

        public:
            Font(const FileSystem::Path& inFilepath);
            virtual ~Font() = default;

        public:
            FontVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(FontVendor inValue);

            const FontRaw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const FontRaw& inValue);

        private:
            void fetchVendor();
            void fetchData();

        private:
            FontVendor m_vendor;
            FontRaw    m_data;
        };
    }
}