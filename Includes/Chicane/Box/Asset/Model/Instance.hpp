#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Model/Data.hpp"
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