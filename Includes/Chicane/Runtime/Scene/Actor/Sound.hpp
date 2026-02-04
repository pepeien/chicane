#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASound : public Actor
    {
    public:
        ASound();

    public:
        void load(const String& inSound);
        void play() const;

    protected:
        CSound* m_sound;
    };
}