#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Model/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

            public:
                Vendor::Type getVendor() const;
                void setVendor(Vendor::Type inVendor);

                const std::vector<unsigned char>& getData() const;
                void setData(const std::vector<unsigned char>& inData);
                void setData(const std::string& inFilepath);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor::Type               m_vendor;
                std::vector<unsigned char> m_data;
            };
        }
    }
}