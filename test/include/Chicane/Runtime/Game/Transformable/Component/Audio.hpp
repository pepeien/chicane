#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CHICANE CAudio : public Component
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