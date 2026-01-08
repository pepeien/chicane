#include "Chicane/Screech.hpp"

#include "Chicane/Screech/Manager.hpp"

namespace Chicane
{
    namespace Screech
    {
        static const std::unique_ptr<Manager> g_manager = std::make_unique<Manager>();
        static       Device                   g_device  = {};

        void* getContext()
        {
            return g_manager->getContext();
        }

        void* getEngine()
        {
            return g_manager->getEngine();
        }

        std::vector<Device> getDevices()
        {
            return g_manager->getDevices();
        }
    }
}