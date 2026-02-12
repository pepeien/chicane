#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Screech/Sound.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CSound : public Component
    {
    public:
        CSound();

    public:
        void load(const FileSystem::Path& inFilePath);
        void play();

    protected:
        Screech::Sound m_sound;
    };
}