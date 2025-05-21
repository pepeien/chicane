#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Data.hpp"

namespace Chicane
{
    namespace Image
    {
        struct CHICANE_CORE Instance
        {
        public:
            using Super = Instance;

        public:
            Instance(const FileSystem::Path& inFilepath);
            Instance(const Raw& inData);
            Instance();

            ~Instance();

        public:
            int getWidth() const;
            int getHeight() const;
            int getChannel() const;
            int getFormat() const;
            const Pixels getPixels() const;

            int getPitch() const;
            int getSize() const;

            void free();

        protected:
            int    m_width   = 0;
            int    m_height  = 0;
            int    m_channel = 0;
            int    m_format  = 0;
            Pixels m_pixels  = nullptr;
        };
    }
}