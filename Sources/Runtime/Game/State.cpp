#include "Runtime/Game/State.hpp"

namespace Chicane
{
    namespace State
    {
        Controller* m_controller;
        Camera* m_camera;
        Telemetry m_telemetry;
        Vec2 m_resolution;
        Level* m_level;

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

        void setTelemetry(const Telemetry& inTelemetry)
        {
            m_telemetry = inTelemetry;
        }

        Telemetry getTelemetry()
        {
            return m_telemetry;
        }

        void setResolution(const Vec2& inResolution)
        {
            m_resolution = inResolution;
        }

        Vec2 getResolution()
        {
            return m_resolution;
        }

        void setLevel(Level* inLevel)
        {
            m_level = inLevel;
        }

        Level* getLevel()
        {
            return m_level;
        }

        void addModel(const Box::Instance& inBox)
        {
            
        }
    }
}