#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Audio/Data.hpp"
#include "Chicane/Box/Asset/Audio/Vendor.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);

            public:
                Vendor getVendor() const;
                void setVendor(Vendor inVendor);

                const Raw& getData() const;
                void setData(const FileSystem::Path& inFilepath);
                void setData(const Raw& inData);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Vendor m_vendor;
                Raw    m_data;
            };
        }
    }
}