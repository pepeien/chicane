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
            using Raw = std::vector<unsigned char>;

        public:
            static inline constexpr const char* EXTENSION = ".bsnd";
            static inline constexpr const char* TAG       = "Sound";

        public:
            Sound(const Raw& inData);
            Sound(const FileSystem::Path& inFilepath);
            virtual ~Sound() = default;

        public:
            const Raw& getData() const;
            void setData(const FileSystem::Path& inFilepath);
            void setData(const Raw& inData);

        private:
            void fetchData();

        private:
            Raw m_data;
        };
    }
}