#include "Chicane/Screech.hpp"

#include "Chicane/Screech/Manager.hpp"

namespace Chicane
{
    namespace Screech
    {
        static const std::unique_ptr<Manager> g_manager = std::make_unique<Manager>();

        std::vector<Device> getDevices()
        {
            return g_manager->getDevices();
        }
    }
}