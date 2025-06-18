#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Audio.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME AAudio : public Actor
    {
    public:
        AAudio();
        virtual ~AAudio() = default;

    public:
        void load(const String& inAudio);
        void play() const;

    protected:
        CAudio* m_audio;
    };
}