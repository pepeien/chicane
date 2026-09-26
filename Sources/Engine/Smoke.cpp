#include "Chicane/Smoke.hpp"

#include "Chicane/Smoke/Engine.hpp"

namespace Chicane
{
    namespace Smoke
    {
        static bool g_bWasInitialized = false;

        void init()
        {
            if (g_bWasInitialized)
            {
                return;
            }

            Engine::sInstance();

            g_bWasInitialized = true;
        }

        void shutdown()
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            Engine::sInstance().clear();

            g_bWasInitialized = false;
        }

        void tick(float inDeltaTime)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            Engine::sInstance().tick(inDeltaTime);
        }
    }
}
