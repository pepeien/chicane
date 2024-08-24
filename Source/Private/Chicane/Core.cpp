#include "Chicane/Core.hpp"

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    Controller* m_controller = nullptr;

    Level* m_level = nullptr;
    std::unique_ptr<Observable<Level*>> m_levelObservable = std::make_unique<Observable<Level*>>();

    Camera* m_camera = nullptr;
    std::unique_ptr<Observable<Camera*>> m_cameraObservable = std::make_unique<Observable<Camera*>>();

    std::vector<ActorComponent*> m_components {};

    Window* m_window = nullptr;

    bool hasActiveCamera()
    {
        return m_camera != nullptr;
    }

    Camera* getActiveCamera()
    {
        return m_camera;
    }

    void setActiveCamera(Camera* inCamera)
    {
        m_camera = inCamera;

        m_cameraObservable->next(inCamera);
    }

    void watchActiveCamera(
        std::function<void (Camera*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_cameraObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }

    bool hasActiveController()
    {
        return m_controller != nullptr;
    }

    Controller* getActiveController()
    {
        return m_controller;
    }

    void setActiveController(Controller* inController)
    {
        m_controller = inController;
    }

    bool hasActiveLevel()
    {
        return m_level != nullptr;
    }

    Level* getActiveLevel()
    {
        return m_level;
    }

    void setActiveLevel(Level* inLevel)
    {
        m_level = inLevel;

        m_levelObservable->next(inLevel);
    }

    void watchActiveLevel(
        std::function<void (Level*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_levelObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }

    void addActor(Actor* inActor)
    {
        if (!hasActiveLevel())
        {
            return;
        }

        m_level->addActor(inActor);
    }

    std::vector<ActorComponent*> getComponents()
    {
        return m_components;
    }

    void addComponent(ActorComponent* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        m_components.push_back(inComponent);
    }

    Window* getWindow()
    {
        return m_window;
    }

    void setWindow(Window* inWindow)
    {
        m_window = inWindow;
    }

    bool isWindowFocused()
    {
        if (!m_window)
        {
            return false;
        }

        return m_window->isFocused();
    }

    void setWindowFocus(bool inIsFocused)
    {
        if (!m_window)
        {
            return;
        }

        if (inIsFocused)
        {
            m_window->focus();

            return;
        }

        m_window->blur();
    }   

    Vec<2, int> getCursorPosition()
    {
        Vec<2, int> position(0, 0);

        SDL_GetMouseState(
            &position.x,
            &position.y
        );

        return position;
    }

    Vec<2, int> getResolution()
    {
        if (m_window == nullptr)
        {
            return {};
        }

        return m_window->getDrawableSize();
    }

    void setResolution(const Vec<2, int>& inResolution)
    {
        if (m_window == nullptr)
        {
            return;
        }

        m_window->setSize(inResolution);
    }

    void setViewport(
        const Vec<2, std::uint32_t>& inSize,
        const Vec<2, float>& inPosition
    )
    {
        if (m_window == nullptr)
        {
            return;
        }

        m_window->setViewport(inSize, inPosition);
    }

    Telemetry getTelemetry()
    {
        if (m_window == nullptr)
        {
            return {};
        }

        return m_window->getTelemetry();
    }
}