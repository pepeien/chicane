#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Audio.hpp"

namespace Chicane
{
    class AAudio : public Actor
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