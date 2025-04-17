#pragma once

#include "Core.hpp"
#include "Box/Asset/Audio/Data.hpp"
#include "Box/Asset/Audio/Vendor.hpp"
#include "Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            class CHICANE Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

            public:
                Vendor getVendor() const;
                void setVendor(Vendor inVendor);

                const RawData& getData() const;
                void setData(const RawData& inData);
                void setData(const std::string& inFilepath);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor  m_vendor;
                RawData m_data;
            };
        }
    }
}