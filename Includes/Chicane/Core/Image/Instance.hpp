#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Types.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

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

            Instance(const Instance& inInstance);

            virtual ~Instance();

        public:
            int getWidth() const;
            int getHeight() const;
            int getChannel() const;
            int getFormat() const;
            const Pixels getPixels() const;

            int getPitch() const;
            int getSize() const;

        protected:
            Vendor m_type;

            int    m_width;
            int    m_height;
            int    m_channel;
            int    m_format;
            Pixels m_pixels;
        };
    }
}