#pragma once

#include "Chicane/Base.hpp"
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

                const Image::Instance& getData() const;
                void setData(const Image::Instance& inData);
                void setData(const std::string& inFilepath);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor          m_vendor;
                Image::Instance m_data;
            };
        }
    }
}