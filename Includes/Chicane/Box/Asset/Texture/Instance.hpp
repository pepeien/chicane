#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Texture/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class Instance : public Asset::Instance
            {
            public:
                Instance(const std::string& inFilepath);

            public:
                Vendor getVendor() const;
                void setVendor(Vendor inVendor);

                const Image::RawData& getData() const;
                void setData(const Image::RawData& inData);
                void setData(const std::string& inFilepath);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor         m_vendor;
                Image::RawData m_data;
            };
        }
    }
}