#pragma once

#include "Chicane/Core/Audio/Types.hpp"
#include "Chicane/Core/Audio/Specification.hpp"
#include "Chicane/Core/Audio/Vendor.hpp"
#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Audio
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
            void play();

        protected:
            Vendor        m_vendor;

            Specification m_specification;
            std::uint32_t m_bufferLength;
            std::uint8_t* m_bufferData;
            void*         m_stream; // SDL_AudioStream
        };
    }
}