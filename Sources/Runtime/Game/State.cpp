#include "Runtime/Game/State.hpp"

namespace Chicane
{
    namespace State
    {
        Controller* m_controller;
        Camera* m_camera;
        Stats m_stats;
        Vec2 m_resolution;

        void setCamera(Camera* inCamera)
        {
            m_camera = inCamera;
        }

        Camera* getCamera()
        {
            return m_camera;
        }

        void setController(Controller* inController)
        {
            m_controller = inController;
        }

        Controller* getController()
        {
            return m_controller;
        }

        void setStats(const Stats& inStats)
        {
            m_stats = inStats;
        }

        Stats getStats()
        {
            return m_stats;
        }

        void setResolution(const Vec2& inResolution)
        {
            m_resolution = inResolution;
        }

        Vec2 getResolution()
        {
            return m_resolution;
        }
    }
}