#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Renderer/Texture/Vendor.hpp"

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
            void setVendor(Chicane::Texture::Vendor inVendor);

            const Image::Instance& getData() const;
            void setData(const Image::Instance& inData);
            void setData(const std::string& inFilepath);

        private:
            void fetchVendor();
            void fetchData();

        private:
            Chicane::Texture::Vendor m_vendor;
            Image::Instance          m_data;
        };
    }
}