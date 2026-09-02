#include "Editor/UI/Component/Header.reflected.hpp"

#include <Chicane/Core/Input/Keyboard/Button.hpp>
#include <Chicane/Core/Input/Keyboard/Event.hpp>
#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Grid/Component/Button.hpp>
#include <Chicane/Grid/Component/Select.hpp>
#include <Chicane/Grid/Component/Select/Option.hpp>
#include <Chicane/Runtime/Application.hpp>

#include "Editor/UI/Component/Header/Menu.hpp"
#include "Editor/UI/Component/Logo.hpp"

namespace Editor
{
    Header::Header(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          maximizeState("restored"),
          menus({}),
          m_moveWindow(nullptr)
    {
        import <Logo>();
        import <HeaderMenu>();

        load("Assets/Editor/UI/Components/Header.grid", "Assets/Editor/UI/Components/Header.decal");

        initSettingsMenu();
    }

    Header::~Header()
    {
        unbindMoveHitTest();
    }

    bool Header::isFocusable() const
    {
        return true;
    }

    bool Header::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type == Chicane::WindowEventType::KeyDown && inEvent.data)
        {
            const Chicane::Input::KeyboardEvent event = *static_cast<Chicane::Input::KeyboardEvent*>(inEvent.data);

            if (event.button == Chicane::Input::KeyboardButton::Escape)
            {
                closeMenus();

                return true;
            }

            return false;
        }

        if (inEvent.type != Chicane::WindowEventType::MouseButtonDown)
        {
            return false;
        }

        const Chicane::Input::MouseButtonEvent event = *static_cast<Chicane::Input::MouseButtonEvent*>(inEvent.data);

        if (event.button != Chicane::Input::MouseButton::Left || isControlHit(event.location))
        {
            return false;
        }

        if (event.clicks < 2)
        {
            return false;
        }

        onMaximize();

        return true;
    }

    void Header::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Chicane::Window* window = Chicane::Application::getInstance().getWindow();
        maximizeState           = window && window->isMaximized() ? "maximized" : "restored";

        bindMoveHitTest();
    }

    void Header::onBlur()
    {
        closeMenus();
    }

    void Header::onMinimize()
    {
        if (Chicane::Window* window = Chicane::Application::getInstance().getWindow())
        {
            window->minimize();
        }
    }

    void Header::onMaximize()
    {
        if (Chicane::Window* window = Chicane::Application::getInstance().getWindow())
        {
            window->maximize();
        }
    }

    void Header::onClose()
    {
        if (Chicane::Window* window = Chicane::Application::getInstance().getWindow())
        {
            window->close();
        }
    }

    bool Header::isControlHit(const Chicane::Vec2& inLocation) const
    {
        Chicane::Grid::Component* node = getHitAt(inLocation);
        while (node && node != this)
        {
            if (node->getTag().equals(
                    Chicane::Grid::Button::TAG_ID,
                    Chicane::Grid::Select::TAG_ID,
                    Chicane::Grid::SelectOption::TAG_ID,
                    HeaderMenu::TAG_ID
                ))
            {
                return true;
            }

            if (node->isRoot())
            {
                break;
            }

            node = node->getParent();
        }

        return false;
    }

    void Header::closeMenus()
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child))
            {
                menu->close();
            }
        }
    }

    void Header::bindMoveHitTest()
    {
        Chicane::Window* window = Chicane::Application::getInstance().getWindow();
        if (!window || window->getInstance() == m_moveWindow)
        {
            return;
        }

        m_moveWindow = window->getInstance();

        window->setMoveHitTest(
            [this](int inX, int inY)
            {
                const Chicane::Vec2 location(static_cast<float>(inX), static_cast<float>(inY));

                return containsPoint(location) && !isControlHit(location);
            }
        );
    }

    void Header::unbindMoveHitTest()
    {
        Chicane::Window* window = Chicane::Application::getInstance().getWindow();

        if (window && window->getInstance() == m_moveWindow)
        {
            window->setMoveHitTest({});
        }

        m_moveWindow = nullptr;
    }

    void Header::initSettingsMenu()
    {
        // Themes
        HeaderMenuItem themes = {};
        themes.label          = "Themes";

        for (const Chicane::String& theme : {"light", "dark"})
        {
            HeaderMenuItem submenu = {};
            submenu.label          = theme.toUpper().substr(0, 1) + theme.substr(1);
            submenu.action         = "onThemeSwitch(\"" + theme + "\")";

            themes.children.push_back(submenu);
        }

        HeaderMenuItem root = {};
        root.label          = "Settings";
        root.children.push_back(themes);

        menus.push_back(root);
    }
}
