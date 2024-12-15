#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Renderer/Texture.hpp"

namespace Chicane
{
    namespace Box
    {
        class Texture : public Asset
        {
        public:
            static inline std::string EXTENSION = ".btex";
            static inline std::string TAG       = "Texture";

        public:
            Texture(const std::string& inFilepath);

        public:
            Chicane::Texture::Vendor getVendor() const;
            const std::vector<unsigned char>& getData() const;

        private:
            void fetchVendor();
            void fetchData();

        private:
            Chicane::Texture::Vendor m_vendor;
            std::vector<unsigned char> m_data;
        };
    }
}