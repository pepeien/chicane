#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CAudio : public Component
    {
    public:
        CAudio();
        virtual ~CAudio() = default;

    public:
        void load(const std::string& inAudio);
        void play() const;

    protected:
        std::string m_audio;
    }; 
}