#include "Chicane/Kerb.hpp"

#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace Chicane
{
    namespace Kerb
    {
        void init()
        {
            JPH::RegisterDefaultAllocator();

            JPH::Factory::sInstance = new JPH::Factory();

            JPH::RegisterTypes();
        }

        void exit()
        {
            JPH::UnregisterTypes();

            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }
}