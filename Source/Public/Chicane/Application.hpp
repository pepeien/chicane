#pragma once

#include "Chicane/Application/CreateInfo.hpp"
#include "Chicane/Application/Instance.hpp"
#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        // Setup
        void run(const CreateInfo& inCreateInfo);
        void stop();

        // Telemetry
        const Telemetry& getTelemetry();

        // Game
        bool hasController();
        Controller::Instance* getController();
        void setController(Controller::Instance* inController);
        Subscription<Controller::Instance*>* watchController(
            std::function<void (Controller::Instance*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        bool hasLevel();
        Level* getLevel();
        void setLevel(Level* inLevel);
        Subscription<Level*>* watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        bool hasCamera();
        CCamera* getCamera();
        Subscription<CCamera*>* watchCamera(
            std::function<void (CCamera*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // UI
        bool hasView();
        void addView(Grid::View* inView);
        void setView(const std::string& inId);
        Grid::View* getView(const std::string& inId = "");
        Subscription<Grid::View*>* watchView(
            std::function<void (Grid::View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Render
        bool hasWindow();
        Window::Instance* getWindow();
        template<class T>
        T* getWindow()
        {
            if (!hasWindow())
            {
                return nullptr;
            }

            return static_cast<T*>(getWindow());
        }

        bool hasRenderer();
        Renderer::Instance* getRenderer();
        template<class T>
        T* getRenderer()
        {
            if (!hasRenderer())
            {
                return nullptr;
            }

            return static_cast<T*>(getRenderer());
        }
    }
}