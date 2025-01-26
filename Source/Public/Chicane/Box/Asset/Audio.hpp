#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Audio.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Renderer/Audio/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        class Audio : public Asset::Instance
        {
        public:
            static inline std::string EXTENSION = ".baud";
            static inline std::string TAG       = "Audio";

        public:
            Audio(const std::string& inFilepath);

        public:
            Chicane::Audio::Vendor getVendor() const;
            void setVendor(Chicane::Audio::Vendor inVendor);

            const Chicane::Audio::Instance& getData() const;
            void setData(const Chicane::Audio::Instance& inData);
            void setData(const std::string& inFilepath);

        private:
            void fetchVendor();
            void fetchData();

        private:
            Chicane::Audio::Vendor   m_vendor;
            Chicane::Audio::Instance m_data;
        };
    }
}