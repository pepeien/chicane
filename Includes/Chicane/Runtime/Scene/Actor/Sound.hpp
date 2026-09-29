#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Actor | Sound")
    class CHICANE_RUNTIME ASound : public Actor
    {
    public:
        CH_CONSTRUCTOR()
        ASound();

    protected:
        void onLoad() override;

    public:
        CH_FUNCTION()
        void load(const FileSystem::Path& inFilePath);

        CH_FUNCTION()
        void play() const;

    protected:
        CSound* m_sound;
    };
}