#include "Chicane/Runtime/Application/Essential.hpp"

#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    static const Telemetry EMPTY_TELEMETRY = {};

    namespace Application
    {
        std::unique_ptr<Instance> m_application = nullptr;

        void run(const CreateInfo& inCreateInfo)
        {
            if (m_application)
            {
                return;
            }

            m_application = std::make_unique<Instance>();
            m_application->setup(inCreateInfo);
            m_application->run();
        }

        void stop()
        {
            if (!m_application)
            {
                return;
            }

            m_application.reset();
        }

        const Telemetry& getTelemetry()
        {
            if (!m_application)
            {
                return EMPTY_TELEMETRY;
            }

            return m_application->getTelemetry();
        }

        bool hasController()
        {
            if (!m_application)
            {
                return false;
            }

            return  m_application->hasController();
        }

        Controller* getController()
        {
            if (!m_application)
            {
                return nullptr;
            }

            return  m_application->getController();
        }

        void setController(Controller* inController)
        {
            if (!m_application)
            {
                return;
            }

            m_application->setController(inController);
        }

        ControllerSubscription* watchController(
            ControllerSubscription::NextCallback inNext,
            ControllerSubscription::ErrorCallback inError,
            ControllerSubscription::CompleteCallback inComplete
        )
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->watchController(inNext, inError, inComplete);
        }

        bool hasLevel()
        {
            if (!m_application)
            {
                return false;
            }

            return m_application->hasLevel();
        }

        Level* getLevel()
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->getLevel();
        }

        void setLevel(Level* inLevel)
        {
            if (!m_application)
            {
                return;
            }

            m_application->setLevel(inLevel);
        }

        LevelSubscription* watchLevel(
            LevelSubscription::NextCallback inNext,
            LevelSubscription::ErrorCallback inError,
            LevelSubscription::CompleteCallback inComplete 
        )
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->watchLevel(inNext, inError, inComplete);
        }

        bool hasView()
        {
            if (!m_application)
            {
                return false;
            }

            return m_application->hasView();
        }

        Grid::View* getView()
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->getView();
        }

        void setView(Grid::View* inView)
        {
            if (!m_application)
            {
                return;
            }
    
            m_application->setView(inView);
        }

        ViewSubscription* watchView(
            ViewSubscription::NextCallback inNext,
            ViewSubscription::ErrorCallback inError,
            ViewSubscription::CompleteCallback inComplete
        )
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->watchView(inNext, inError, inComplete);
        }

        bool hasWindow()
        {
            if (!m_application)
            {
                return false;
            }

            return m_application->hasWindow();
        }

        Window::Instance* getWindow()
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->getWindow();
        }

        bool hasRenderer()
        {
            if (!m_application)
            {
                return false;
            }

            return m_application->hasRenderer();
        }

        Renderer::Instance* getRenderer()
        {
            if (!m_application)
            {
                return nullptr;
            }

            return m_application->getRenderer();
        }
    }
}