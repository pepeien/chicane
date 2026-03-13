#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Screech/Sound.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME CSound : public Component
    {
    public:
        CSound();

    public:
        void load(const FileSystem::Path& inFilePath);
        void play();

    protected:
        Screech::Sound m_instance;
    };
}