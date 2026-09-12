#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Sound : public Asset
        {
        public:
            using Raw = std::vector<unsigned char>;

        public:
            // File
            static constexpr inline const char* EXTENSION = ".bsnd";

            // Tag
            static constexpr inline const char* TAG = "Sound";

        public:
            Sound(const FileSystem::Path& inFilepath);
            virtual ~Sound() = default;

        public:
            const Raw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Raw& inData);

        private:
            void fetchDataFromXML();
            void bakePreview();

        private:
            Raw m_data;
        };
    }
}