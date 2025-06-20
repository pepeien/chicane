#pragma once

#include "Chicane/Core/Audio/Specification.hpp"
#include "Chicane/Core/Audio/Vendor.hpp"
#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    struct CHICANE_CORE Audio
    {
    public:
        using Raw = std::vector<unsigned char>;

    protected:
        using Super = Audio;

    public:
        Audio(const FileSystem::Path& inFilepath);
        Audio(const Raw& inData);
        Audio();

        Audio(const Audio& inInstance);

        virtual ~Audio();

    public:
        void play();

    protected:
        AudioVendor        m_vendor;
        AudioSpecification m_specification;
        std::uint32_t      m_bufferLength;
        std::uint8_t*      m_bufferData;
        void*              m_stream; // SDL_AudioStream
    };
}