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

    void setResolution(const Vec<int>::Two& inResolution)
    {
        if (m_window == nullptr)
        {
            return;
        }

        m_window->setResolution(inResolution);
    }

    Vec<int>::Two getResolution()
    {
        if (m_window == nullptr)
        {
            return {};
        }

        return m_window->getResolution();
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