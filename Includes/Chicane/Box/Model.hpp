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
            Model(const FileSystem::Path& inFilepath);
            virtual ~Model() = default;

        public:
            ModelVendor getVendor() const;
            void setVendor(ModelVendor inVendor);

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