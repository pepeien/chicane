#pragma once

#include "Core.hpp"
#include "Core/Image.hpp"
#include "Box/Asset/Instance.hpp"
#include "Box/Asset/Texture/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class CHICANE Instance : public Asset::Instance
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