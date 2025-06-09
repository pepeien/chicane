#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

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
                virtual ~Instance() = default;

            public:
                Image::Vendor getVendor() const;
                void setVendor(Image::Vendor inVendor);

                const Image::Raw& getData() const;
                void setData(const String& inFilepath);
                void setData(const Image::Raw& inData);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Image::Vendor m_vendor;
                Image::Raw    m_data;
            };
        }
    }
}