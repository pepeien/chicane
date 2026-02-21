#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Model/Raw.hpp"
#include "Chicane/Box/Model/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Model : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bmdl";
            static constexpr inline const char* TAG       = "Model";

            static constexpr inline const char* VENDOR_ATTRIBUTE_NAME = "vendor";

        public:
            static ModelVendor parseVendor(const String& inValue);
            static const String& getVendorExtension(ModelVendor inValue);

        public:
            Model(const FileSystem::Path& inFilepath);
            virtual ~Model() = default;

        public:
            ModelVendor getVendor() const;
            void setVendor(const String& inValue);
            void setVendor(ModelVendor inValue);

            const ModelRaw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const ModelRaw& inData);

        private:
            void fetchVendor();
            void fetchData();

        private:
            ModelVendor m_vendor;
            ModelRaw    m_data;
        };
    }
}