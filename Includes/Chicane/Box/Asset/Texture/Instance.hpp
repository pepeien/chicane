#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Texture/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);

            public:
                Vendor getVendor() const;
                void setVendor(Vendor inVendor);

                const Image::Raw& getData() const;
                void setData(const std::string& inFilepath);
                void setData(const Image::Raw& inData);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor     m_vendor;
                Image::Raw m_data;
            };
        }
    }
}