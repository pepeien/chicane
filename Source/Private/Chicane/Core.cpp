#include "Chicane/Core.hpp"

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    Camera* m_camera = nullptr;
    std::unique_ptr<Observable<Camera*>> m_cameraObservable = std::make_unique<Observable<Camera*>>();

    Controller* m_controller = nullptr;

    Level* m_level = nullptr;
    std::unique_ptr<Observable<Level*>> m_levelObservable = std::make_unique<Observable<Level*>>();

    Window* m_window = nullptr;

    bool hasCamera()
    {
        return m_camera != nullptr;
    }

    Camera* getCamera()
    {
        return m_camera;
    }

    void setCamera(Camera* inCamera)
    {
        m_camera = inCamera;

        m_cameraObservable->next(inCamera);
    }

    void subscribeToCamera(
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

    bool hasController()
    {
        return m_controller != nullptr;
    }

    Controller* getController()
    {
        return m_controller;
    }

    void setController(Controller* inController)
    {
        m_controller = inController;
        m_controller->observeAttachment(
            [](Pawn* inPawn)
            {
                setCamera(inPawn->getCamera<Camera>());
            }
        );
    }

    bool hasLevel()
    {
        return m_level != nullptr;
    }

    Level* getLevel()
    {
        return m_level;
    }

    void setLevel(Level* inLevel)
    {
        m_level = inLevel;

        m_levelObservable->next(inLevel);
    }

    void subscribeToLevel(
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
        if (!hasLevel())
        {
            return;
        }

        m_level->addActor(inActor);
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

    Vec<int>::Two getCursorPosition()
    {
        Vec<int>::Two position(0, 0);

        SDL_GetMouseState(
            &position.x,
            &position.y
        );

        return position;
    }

    Vec<int>::Two getResolution()
    {
        if (m_window == nullptr)
        {
            return {};
        }

        return m_window->getDrawableSize();
    }

    void setResolution(const Vec<int>::Two& inResolution)
    {
        if (m_window == nullptr)
        {
            return;
        }

        m_window->setSize(inResolution);
    }

    void setViewport(
        const Vec<std::uint32_t>::Two& inSize,
        const Vec<float>::Two& inPosition
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