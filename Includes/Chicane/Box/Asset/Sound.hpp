#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Sound : public Asset
        {
        public:
            static inline constexpr const char* EXTENSION = ".bsnd";
            static inline constexpr const char* TAG       = "Sound";

            static inline constexpr const char* VENDOR_ATTRIBUTE_NAME = "vendor";

        public:
            Sound(const FileSystem::Path& inFilepath);
            virtual ~Sound() = default;

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