#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Audio.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CAudio : public Component
    {
    public:
        CAudio(const std::string& inAudio);
        virtual ~CAudio() = default;

    public:
        void play() const;

    protected:
        std::string m_audio;
    }; 
}