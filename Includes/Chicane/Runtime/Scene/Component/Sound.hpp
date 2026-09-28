#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Screech/Sound.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Component | Sound")
    class CHICANE_RUNTIME CSound : public Component
    {
    public:
        // Attributes
        static constexpr inline const char* SOUND_ATTRIBUTE_NAME = "sound";

    public:
        CH_CONSTRUCTOR()
        CSound();

    protected:
        void onPropertyEdited(const String& inName) override;
        void onAttachment(Object* inParent) override;

    public:
        CH_FUNCTION()
        void load(const FileSystem::Path& inFilePath);

        CH_FUNCTION()
        void play();

    public:
        CH_FIELD()
        FileSystem::Path sound;

    protected:
        Screech::Sound m_instance;
    };
}