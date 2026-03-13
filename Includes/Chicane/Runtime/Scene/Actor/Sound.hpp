#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME ASound : public Actor
    {
    public:
        ASound();

    protected:
        void onLoad() override;

    public:
        void load(const FileSystem::Path& inFilePath);
        void play() const;

    protected:
        CSound* m_sound;
    };
}