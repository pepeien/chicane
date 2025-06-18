#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CAudio : public Component
    {
    public:
        CAudio();
        virtual ~CAudio() = default;

    public:
        void load(const String& inAudio);
        void play() const;

    protected:
        String m_audio;

    private:
        Box::SoundManager* m_manager;
    }; 
}