#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASound : public Actor
    {
    public:
        ASound();
        virtual ~ASound() = default;

    public:
        void load(const String& inSound);
        void play() const;

    protected:
        CSound* m_sound;
    };
}