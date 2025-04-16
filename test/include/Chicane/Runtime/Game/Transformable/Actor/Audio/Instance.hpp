#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Audio.hpp"

namespace Chicane
{
    class CHICANE AAudio : public Actor
    {
    public:
        AAudio();
        virtual ~AAudio() = default;

    public:
        void load(const std::string& inAudio);
        void play() const;

    protected:
        CAudio* m_audio;
    };
}