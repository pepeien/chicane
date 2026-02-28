#include "Chicane/Screech.hpp"

#include <memory>

#include "Chicane/Screech/Device.hpp"
#include "Chicane/Screech/Manager.hpp"

namespace Chicane
{
    namespace Screech
    {
        static bool g_bWasInitialized = false;

        static std::unique_ptr<Manager> g_manager;
        static Device                   g_device;

        void init()
        {
            g_manager = std::make_unique<Manager>();
            g_device  = {};

            g_bWasInitialized = true;
        }

        void* getContext()
        {
            if (!g_bWasInitialized)
            {
                return nullptr;
            }

            return g_manager->getContext();
        }

        void* getEngine()
        {
            if (!g_bWasInitialized)
            {
                return nullptr;
            }

            return g_manager->getEngine();
        }
    }
}