#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Model/Types.hpp"
#include "Chicane/Box/Asset/Model/Vendor.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Model : public Asset
        {
        public:
            static inline constexpr const char* EXTENSION = ".bmdl";
            static inline constexpr const char* TAG       = "Model";

            static inline constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

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