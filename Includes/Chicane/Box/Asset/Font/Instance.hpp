#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"
#include "Chicane/Box/Asset/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);
                virtual ~Instance() = default;

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