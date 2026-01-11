#pragma once

#include "Chicane/Box/Asset/Sound/Manager.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Screech/Sound.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CSound : public Component
    {
    public:
        CSound();
        virtual ~CSound() = default;

    public:
        void load(const String& inSound);
        void play();

    protected:
        Screech::Sound m_sound;

    private:
        Box::SoundManager* m_manager;
    };
}