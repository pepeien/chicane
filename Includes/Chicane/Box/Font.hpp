#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Font/Raw.hpp"
#include "Chicane/Box/Font/Family.hpp"
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
            static constexpr inline const char* DEFAULT_SOURCE        = "Assets/Engine/Fonts/Lexend.bfon";

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

            const FontFamily& getData() const;
            const FontFamily& getData(float inWeight) const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const FontRaw& inValue);

            std::vector<const FontFamily*> getInstances() const;

        private:
            void fetchVendorFromXML();
            void fetchDataFromXML();
            void rebuildInstances();

            FontFamily parseData(const FontRaw& inValue) const;
            FontFamily parseData(const FontRaw& inValue, float inWeight) const;

        private:
            FontVendor                          m_vendor;
            FontRaw                             m_raw;
            FontFamily                          m_data;
            std::unordered_map<int, FontFamily> m_instances;
        };
    }
}