#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Renderer/Model.hpp"

namespace Chicane
{
    namespace Box
    {
        class Model : public Asset
        {
        public:
            static inline std::string EXTENSION = ".bmdl";
            static inline std::string TAG       = "Model";

        public:
            Model(const std::string& inFilepath);

        public:
            Chicane::Model::Vendor::Type getVendor() const;
            void setVendor(Chicane::Model::Vendor::Type inVendor);

            const std::vector<unsigned char>& getData() const;
            void setData(const std::vector<unsigned char>& inData);
            void setData(const std::string& inFilepath);

        private:
            void fetchVendor();
            void fetchData();

        private:
            Chicane::Model::Vendor::Type m_vendor;
            std::vector<unsigned char> m_data;
        };
    }
}