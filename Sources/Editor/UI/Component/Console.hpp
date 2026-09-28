#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Reflection/Type/Method/Info.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Editor/UI/Component/Console/Command.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Console))
    class Console : public Chicane::Grid::Container
    {
    private:
        static Chicane::String sCommandNameFromMethod(const Chicane::String& inMethod);
        static bool sIsConsoleMethod(const Chicane::ReflectionTypeMethodInfo& inMethod);

    public:
        CH_CONSTRUCTOR()
        Console(const Chicane::XmlNode& inNode);

    public:
        bool onEvent(const Chicane::WindowEvent& inEvent) override;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onCommandInput();

        CH_FUNCTION()
        void onPickSuggestion(Chicane::String inValue);

    private:
        void refreshCommands();
        void refreshSuggestions();
        void complete();
        void submit();

        void execute(const Chicane::String& inLine);
        void executeBuiltin(const Chicane::String& inName, const Chicane::String& inArgument);
        void executeMethod(const ConsoleCommand& inCommand, const Chicane::String& inArgument);
        void source(const Chicane::String& inPath);

        void moveHighlight(int inDelta);
        void applyHighlight();

        Chicane::Grid::Component* findCommandInput() const;
        Chicane::Grid::Component* findSuggestions() const;
        const ConsoleCommand* findCommand(const Chicane::String& inName) const;

    public:
        CH_FIELD()
        Chicane::String command;

        CH_FIELD()
        std::vector<Chicane::String> suggestions;

        CH_FIELD()
        bool bHasSuggestions;

    private:
        std::vector<ConsoleCommand>                      m_commands;
        std::unordered_map<Chicane::String, std::size_t> m_commandIndex;

        std::size_t m_highlighted;
    };
}
