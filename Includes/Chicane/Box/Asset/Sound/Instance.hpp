#pragma once

#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sound
        {
            class CHICANE_BOX Instance : public Asset::Instance
            {
            public:
                Instance(const FileSystem::Path& inFilepath);
                virtual ~Instance() = default;

            public:
                Audio::Vendor getVendor() const;
                void setVendor(Audio::Vendor inVendor);

                const Audio::Raw& getData() const;
                void setData(const FileSystem::Path& inFilepath);
                void setData(const Audio::Raw& inData);

            private:
                void fetchVendor();
                void fetchData();

            private:
                Audio::Vendor m_vendor;
                Audio::Raw    m_data;
            };
        }
    }
}