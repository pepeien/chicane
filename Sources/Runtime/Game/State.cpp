#include "Runtime/Game/State.hpp"

#include "Runtime/Core.hpp"

namespace Chicane
{
    namespace State
    {
        Window* m_window;

        Window* getWindow()
        {
            return m_window;
        }

        void setWindow(Window* inWindow)
        {
            m_window = inWindow;
        }

        Controller* getController()
        {
            if (m_window == nullptr)
            {
                return nullptr;
            }

            return m_window->getController();
        }

        void setController(Controller* inController)
        {
            if (m_window == nullptr)
            {
                return;
            }

            m_window->setController(inController);
        }

        Telemetry getTelemetry()
        {
            if (m_window == nullptr)
            {
                return {};
            }

            return m_window->getTelemetry();
        }

        void setResolution(const Vec2& inResolution)
        {
            if (m_window == nullptr)
            {
                return;
            }

            m_window->setResolution(inResolution);
        }

        Vec2 getResolution()
        {
            if (m_window == nullptr)
            {
                return {};
            }

            return m_window->getResolution();
        }

        void setLevel(Level* inLevel)
        {
            if (m_window == nullptr)
            {
                return;
            }

            m_window->setLevel(inLevel);
        }

        Level* getLevel()
        {
            return m_window->getLevel();
        }

        void addModel(const Box::Instance& inBox)
        {
            
        }
    }
}