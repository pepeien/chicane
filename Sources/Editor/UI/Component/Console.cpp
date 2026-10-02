#include "Editor/UI/Component/Console.reflected.hpp"

#include <exception>

#include <Chicane/Core/Color.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Input/Keyboard/Event.hpp>
#include <Chicane/Core/Log.hpp>
#include <Chicane/Core/Reflection/Type/Method.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Core/Script/Context.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Input/Text.hpp>
#include <Chicane/Grid/Component/View.hpp>

#include "Editor/UI/Component/Console/Command.hpp"
#include "Editor/UI/Component/Dock/Header.hpp"

namespace Editor
{
    static constexpr inline const char* CONSOLE_ID = "Console";

    Chicane::String Console::sCommandNameFromMethod(const Chicane::String& inMethod)
    {
        Chicane::String name = inMethod;
        if (name.startsWith("on") && name.size() > 2)
        {
            name = name.substr(2);
        }

        if (name.isEmpty())
        {
            return name;
        }

        return name.substr(0, 1).toLower() + name.substr(1);
    }

    bool Console::sIsConsoleMethod(const Chicane::ReflectionTypeMethodInfo& inMethod)
    {
        if (inMethod.getName().isEmpty() || inMethod.paramTypes.size() > 1)
        {
            return false;
        }

        if (inMethod.paramTypes.empty())
        {
            return true;
        }

        const Chicane::String& type = inMethod.paramTypes.front();

        return type.contains("String") && !type.contains("*");
    }

    Console::Console(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          command(Chicane::String::sEmpty()),
          suggestions({}),
          bHasSuggestions(false),
          m_commands({}),
          m_commandIndex({}),
          m_highlighted(0)
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Console/Index.grid", "Assets/Editor/UI/Components/Console/Index.decal");
    }

    bool Console::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type != Chicane::WindowEventType::KeyDown || !inEvent.data)
        {
            return Chicane::Grid::Container::onEvent(inEvent);
        }

        Chicane::Grid::Component* input = findCommandInput();
        if (!input || !input->isFocused())
        {
            return false;
        }

        const Chicane::Input::KeyboardEvent event = *static_cast<const Chicane::Input::KeyboardEvent*>(inEvent.data);

        if (event.button == Chicane::Input::KeyboardButton::Tab)
        {
            complete();

            return true;
        }

        if (event.button == Chicane::Input::KeyboardButton::Return)
        {
            submit();

            return true;
        }

        if (event.button == Chicane::Input::KeyboardButton::Escape && bHasSuggestions)
        {
            suggestions.clear();
            bHasSuggestions = false;

            return true;
        }

        if (event.button == Chicane::Input::KeyboardButton::Down)
        {
            moveHighlight(1);

            return true;
        }

        if (event.button == Chicane::Input::KeyboardButton::Up)
        {
            moveHighlight(-1);

            return true;
        }

        return false;
    }

    void Console::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        if (m_commands.empty())
        {
            refreshCommands();
        }

        applyHighlight();
    }

    void Console::onCommandInput()
    {
        refreshSuggestions();
    }

    void Console::onPickSuggestion(Chicane::String inValue)
    {
        command = inValue.trim();
        suggestions.clear();
        bHasSuggestions = false;
        m_highlighted   = 0;
    }

    void Console::refreshCommands()
    {
        m_commands.clear();
        m_commandIndex.clear();

        auto add = [this](const Chicane::String& inName, const Chicane::String& inMethod, bool bTakesArgument)
        {
            if (inName.isEmpty() || m_commandIndex.find(inName) != m_commandIndex.end())
            {
                return;
            }

            m_commandIndex[inName] = m_commands.size();
            m_commands.push_back({inName, inMethod, bTakesArgument});
        };

        add("help", Chicane::String::sEmpty(), false);
        add("clear", Chicane::String::sEmpty(), false);
        add("source", Chicane::String::sEmpty(), true);

        Chicane::Grid::View* view = dynamic_cast<Chicane::Grid::View*>(getRoot());
        if (!view)
        {
            return;
        }

        const Chicane::ReflectionTypeInfo* type = Chicane::ReflectionTypeRegistry::sInstance().find(typeid(*view));
        if (!type)
        {
            return;
        }

        for (const Chicane::ReflectionTypeMethodInfo& method : type->methods)
        {
            if (!sIsConsoleMethod(method))
            {
                continue;
            }

            add(sCommandNameFromMethod(method.getName()), method.getName(), !method.paramTypes.empty());
        }
    }

    void Console::refreshSuggestions()
    {
        suggestions.clear();
        m_highlighted = 0;

        const Chicane::String line  = command.trim();
        const std::size_t     space = line.firstOf(' ');
        if (space != Chicane::String::npos)
        {
            bHasSuggestions = false;

            return;
        }

        const Chicane::String prefix = line.toLower();
        for (const ConsoleCommand& entry : m_commands)
        {
            if (prefix.isEmpty() || entry.name.toLower().startsWith(prefix))
            {
                suggestions.push_back(entry.name);
            }
        }

        bHasSuggestions = !suggestions.empty() && !prefix.isEmpty();
    }

    void Console::complete()
    {
        refreshSuggestions();
        if (suggestions.empty())
        {
            return;
        }

        if (m_highlighted >= suggestions.size())
        {
            m_highlighted = 0;
        }

        command         = suggestions.at(m_highlighted);
        bHasSuggestions = suggestions.size() > 1;
        if (!bHasSuggestions)
        {
            suggestions.clear();
        }
    }

    void Console::submit()
    {
        const Chicane::String line = command.trim();
        suggestions.clear();
        bHasSuggestions = false;
        m_highlighted   = 0;
        command         = Chicane::String::sEmpty();

        if (line.isEmpty())
        {
            return;
        }

        execute(line);
    }

    void Console::execute(const Chicane::String& inLine)
    {
        Chicane::Log::emmit(Chicane::Color::HEX_COLOR_LIME, CONSOLE_ID, "> " + inLine);

        const std::size_t     space = inLine.firstOf(' ');
        const Chicane::String name  = space == Chicane::String::npos ? inLine : inLine.substr(0, space);
        const Chicane::String argument =
            space == Chicane::String::npos ? Chicane::String::sEmpty() : inLine.substr(space + 1).trim();

        const ConsoleCommand* found = findCommand(name);
        if (!found)
        {
            Chicane::Log::error("Unknown command [%s]. Type help for a list of commands.", name.toChar());

            return;
        }

        if (found->method.isEmpty())
        {
            executeBuiltin(found->name, argument);

            return;
        }

        executeMethod(*found, argument);
    }

    void Console::executeBuiltin(const Chicane::String& inName, const Chicane::String& inArgument)
    {
        if (inName.equals("clear"))
        {
            Chicane::Log::clear();

            return;
        }

        if (inName.equals("source"))
        {
            source(inArgument);

            return;
        }

        Chicane::Log::info("Commands:");
        for (const ConsoleCommand& entry : m_commands)
        {
            Chicane::Log::emmit(
                Chicane::Color::HEX_COLOR_WHITE,
                CONSOLE_ID,
                entry.bTakesArgument ? entry.name + " <argument>" : entry.name
            );
        }
    }

    void Console::executeMethod(const ConsoleCommand& inCommand, const Chicane::String& inArgument)
    {
        Chicane::Grid::View* view = dynamic_cast<Chicane::Grid::View*>(getRoot());
        if (!view)
        {
            return;
        }

        const Chicane::ReflectionTypeInfo* type = Chicane::ReflectionTypeRegistry::sInstance().find(typeid(*view));
        const Chicane::ReflectionTypeMethodInfo* method = type ? type->findMethod(inCommand.method) : nullptr;
        if (!method)
        {
            Chicane::Log::error("Command [%s] is no longer available.", inCommand.name.toChar());

            return;
        }

        if (inCommand.bTakesArgument && inArgument.isEmpty())
        {
            Chicane::Log::error("Command [%s] expects an argument.", inCommand.name.toChar());

            return;
        }

        Chicane::ReflectionTypeMethod call(method);
        call.bind(view);
        if (inCommand.bTakesArgument)
        {
            call.addParam(inArgument);
        }

        try
        {
            call.invoke();
        }
        catch (const std::exception& exception)
        {
            Chicane::Log::error("%s", exception.what());
        }
    }

    void Console::source(const Chicane::String& inPath)
    {
        if (inPath.isEmpty())
        {
            Chicane::Log::error("source expects a file path.");

            return;
        }

        const Chicane::FileSystem::Path path(inPath);
        if (!Chicane::FileSystem::exists(path))
        {
            Chicane::Log::error("File [%s] does not exist.", inPath.toChar());

            return;
        }

        Chicane::Script::Context context;
        if (!context.open(Chicane::Script::Context::FLAG_MODULE))
        {
            Chicane::Log::error("Failed to open a script context.");

            return;
        }

        if (!context.loadFile(path))
        {
            return;
        }

        Chicane::Log::info("Sourced [%s].", inPath.toChar());
    }

    void Console::moveHighlight(int inDelta)
    {
        if (suggestions.empty())
        {
            refreshSuggestions();
        }

        if (suggestions.empty())
        {
            return;
        }

        const int  count  = static_cast<int>(suggestions.size());
        int        next   = static_cast<int>(m_highlighted) + inDelta;
        const bool bNextNegative = static_cast<bool>(next < 0);

        if (bNextNegative)
        {
            next = count - 1;
        }

        const bool bNextCount = !bNextNegative && (next >= count);

        if (bNextCount)
        {
            next = 0;
        }

        m_highlighted = static_cast<std::size_t>(next);
        command       = suggestions.at(m_highlighted);
    }

    void Console::applyHighlight()
    {
        Chicane::Grid::Component* list = findSuggestions();
        if (!list)
        {
            return;
        }

        const std::vector<Chicane::Grid::Component*>& rows = list->getChildren();
        for (std::size_t i = 0; i < rows.size(); i++)
        {
            if (Chicane::Grid::Component* row = rows.at(i))
            {
                row->setSelected(bHasSuggestions && i == m_highlighted);
            }
        }
    }

    Chicane::Grid::Component* Console::findCommandInput() const
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->getTag().equals(Chicane::Grid::InputText::TAG_ID))
            {
                return child;
            }
        }

        return nullptr;
    }

    Chicane::Grid::Component* Console::findSuggestions() const
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->getClassName().contains("console__suggestions") &&
                !child->getClassName().contains("console__suggestions__"))
            {
                return child;
            }
        }

        return nullptr;
    }

    const ConsoleCommand* Console::findCommand(const Chicane::String& inName) const
    {
        const Chicane::String lower = inName.toLower();
        for (const ConsoleCommand& entry : m_commands)
        {
            if (entry.name.toLower().equals(lower))
            {
                return &entry;
            }
        }

        return nullptr;
    }
}
