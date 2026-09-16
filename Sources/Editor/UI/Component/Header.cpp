#include "Editor/UI/Component/Header.reflected.hpp"

#include <mutex>

#include <Chicane/Core/Input/Keyboard/Button.hpp>
#include <Chicane/Core/Input/Keyboard/Event.hpp>
#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Grid/Component/Button.hpp>
#include <Chicane/Grid/Component/Input/Select.hpp>
#include <Chicane/Grid/Component/Input/Select/Option.hpp>
#include <Chicane/Runtime/Application.hpp>

#include "Editor/UI/Component/Header/Menu.hpp"
#include "Editor/UI/Component/Logo.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Header::Header(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          maximizeState("restored"),
          menus({}),
          theme(Chicane::String::empty()),
          viewportTabState(Chicane::String::empty()),
          assetsTabState(Chicane::String::empty()),
          m_moveWindow(nullptr),
          m_moveHitMutex(),
          m_moveBounds({}),
          m_moveControls({})
    {
        import <Logo>();
        import <HeaderMenu>();

        load("Assets/Editor/UI/Components/Header.grid", "Assets/Editor/UI/Components/Header.decal");

        initFileMenu();
        initSettingsMenu();

        Prop::bind(this, THEME_ATTRIBUTE, theme);
        Prop::bind(this, VIEWPORT_TAB_STATE_ATTRIBUTE, viewportTabState);
        Prop::bind(this, ASSETS_TAB_STATE_ATTRIBUTE, assetsTabState);
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

    void Header::tick(float inDeltaTime)
    {
        Chicane::Grid::Container::tick(inDeltaTime);

        publishMoveHitSnapshot();
    }

    void Header::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Chicane::Window* window = Chicane::Application::getInstance().getWindow();
        maximizeState           = window && window->isMaximized() ? "maximized" : "restored";

        Prop::copy(this, THEME_ATTRIBUTE, theme);
        Prop::copy(this, VIEWPORT_TAB_STATE_ATTRIBUTE, viewportTabState);
        Prop::copy(this, ASSETS_TAB_STATE_ATTRIBUTE, assetsTabState);

        syncMenuChecks();
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

    void Header::onWorkspaceViewport()
    {
        Prop::invoke(this, ON_WORKSPACE_VIEWPORT_ATTRIBUTE);
    }

    void Header::onWorkspaceAssets()
    {
        Prop::invoke(this, ON_WORKSPACE_ASSETS_ATTRIBUTE);
    }

    bool Header::isControl(const Chicane::Grid::Component* inComponent) const
    {
        if (!inComponent)
        {
            return false;
        }

        return inComponent->getTag().equals(
            Chicane::Grid::Button::TAG_ID,
            Chicane::Grid::InputSelect::TAG_ID,
            Chicane::Grid::InputSelectOption::TAG_ID,
            HeaderMenu::TAG_ID
        );
    }

    bool Header::isControlHit(const Chicane::Vec2& inLocation) const
    {
        Chicane::Grid::Component* node = getHitAt(inLocation);
        while (node && node != this)
        {
            if (isControl(node))
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

    void Header::publishMoveHitSnapshot()
    {
        Chicane::Bounds2D              bounds;
        std::vector<Chicane::Bounds2D> controls;

        if (isDisplayable())
        {
            bounds = getDrawBounds();

            for (Chicane::Grid::Component* child : getChildrenFlat())
            {
                if (!isControl(child) || !child->isDisplayable())
                {
                    continue;
                }

                const Chicane::Bounds2D box = child->getDrawBounds();
                if (!box.isEmpty())
                {
                    controls.push_back(box);
                }
            }
        }

        std::lock_guard<std::mutex> lock(m_moveHitMutex);
        m_moveBounds   = bounds;
        m_moveControls = std::move(controls);
    }

    bool Header::isMoveRegion(int inX, int inY) const
    {
        std::unique_lock<std::mutex> lock(m_moveHitMutex, std::try_to_lock);
        if (!lock.owns_lock())
        {
            return false;
        }

        const Chicane::Vec2 location(static_cast<float>(inX), static_cast<float>(inY));
        if (m_moveBounds.isEmpty() || !m_moveBounds.contains(location))
        {
            return false;
        }

        for (const Chicane::Bounds2D& box : m_moveControls)
        {
            if (box.contains(location))
            {
                return false;
            }
        }

        return true;
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

        window->setMoveHitTest([this](int inX, int inY) { return isMoveRegion(inX, inY); });
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

    void Header::initFileMenu()
    {
        // Level
        HeaderMenuItem level = {};
        level.label          = "Level";

        HeaderMenuItem create = {};
        create.label          = "New";
        create.shortcut       = "Ctrl+N";
        create.action         = "onTrackNew()";
        level.children.push_back(create);

        HeaderMenuItem open = {};
        open.label          = "Open";
        open.shortcut       = "Ctrl+O";
        open.action         = "onTrackOpen()";
        level.children.push_back(open);

        HeaderMenuItem save = {};
        save.label          = "Save";
        save.shortcut       = "Ctrl+S";
        save.action         = "onTrackSave()";
        level.children.push_back(save);

        HeaderMenuItem saveAs = {};
        saveAs.label          = "Save As";
        saveAs.action         = "onTrackSaveAs()";
        level.children.push_back(saveAs);

        // Root
        HeaderMenuItem file = {};
        file.label          = "File";
        file.children.push_back(level);

        menus.push_back(file);
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

    void Header::syncMenuChecks()
    {
        for (HeaderMenuItem& root : menus)
        {
            for (HeaderMenuItem& group : root.children)
            {
                if (!group.label.equals("Themes"))
                {
                    continue;
                }

                for (HeaderMenuItem& item : group.children)
                {
                    item.isChecked = theme.equals(item.label.toLower());
                }
            }
        }
    }
}
