#include "Chicane/Kerb.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace Chicane
{
    namespace Kerb
    {
        static bool g_bWasInitialized = false;

        void init()
        {
            if (g_bWasInitialized)
            {
                return;
            }

            JPH::RegisterDefaultAllocator();

            JPH::Factory::sInstance = new JPH::Factory();

            JPH::RegisterTypes();

            g_bWasInitialized = true;
        }

        void shutdown()
        {
            JPH::UnregisterTypes();

            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }
}