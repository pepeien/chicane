#include "Chicane/Core.hpp"

#include "Chicane/Base.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    Controller* m_controller = nullptr;

    Level* m_level = nullptr;
    std::unique_ptr<Observable<Level*>> m_levelObservable = std::make_unique<Observable<Level*>>();

    CameraComponent* m_camera = nullptr;
    std::unique_ptr<Observable<CameraComponent*>> m_cameraObservable = std::make_unique<Observable<CameraComponent*>>();

    Window* m_window = nullptr;

    bool hasActiveCamera()
    {
        return m_camera != nullptr;
    }

    CameraComponent* getActiveCamera()
    {
        return m_camera;
    }

    void setActiveCamera(CameraComponent* inCamera)
    {
        if (inCamera == getActiveCamera())
        {
            return;
        }

        m_camera = inCamera;

        m_cameraObservable->next(inCamera);
    }

    void watchActiveCamera(
        std::function<void (CameraComponent*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_cameraObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_camera);
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
        )->next(m_level);
    }

    void addActor(Actor* inActor)
    {
        if (!hasActiveLevel())
        {
            return;
        }

        m_level->addActor(inActor);
    }

    std::vector<Actor*> getActors()
    {
        if (!hasActiveLevel())
        {
            return {};
        }

        return m_level->getActors();
    }

    void addComponent(ActorComponent* inComponent)
    {
        if (!hasActiveLevel())
        {
            return;
        }

        m_level->addComponent(inComponent);
    }

    void removeComponent(ActorComponent* inComponent)
    {
        if (!hasActiveLevel())
        {
            return;
        }

        m_level->removeComponent(inComponent);
    }

    std::vector<ActorComponent*> getComponents()
    {
        if (!hasActiveLevel())
        {
            return {};
        }

        return m_level->getComponents();
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
        return m_window->isFocused();
    }

    void setWindowFocus(bool bInIsFocused)
    {
        if (bInIsFocused)
        {
            m_window->focus();

            return;
        }

        m_window->blur();
    }   

    Vec<2, float> getCursorPosition()
    {
        Vec<2, float> position(0, 0);

        SDL_GetMouseState(
            &position.x,
            &position.y
        );

        return position;
    }

    const Vec<2, int>& getResolution()
    {
        return m_window->getDrawableSize();
    }

    void setResolution(const Vec<2, int>& inResolution)
    {
        m_window->setSize(inResolution);
    }

    void setViewport(
        const Vec<2, std::uint32_t>& inSize,
        const Vec<2, float>& inPosition
    )
    {
        m_window->setViewport(inSize, inPosition);
    }

    const Telemetry& getTelemetry()
    {
        return m_window->getTelemetry();
    }
}