#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Audio.hpp"

namespace Chicane
{
    class AAudio : public Actor
    {
    public:
        AAudio(const std::string& inAudio);
        virtual ~AAudio() = default;

    public:
        void play() const;

    protected:
        CAudio* m_audio;
    };
}