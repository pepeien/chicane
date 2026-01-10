#include "Chicane/Screech/Manager.hpp"

#include "Chicane/Screech/Device.hpp"
#include "Chicane/Screech/Sound.hpp"

#include <miniaudio.h>

namespace Chicane
{
    namespace Screech
    {
        static bool       g_bWasContextInitialized = false;
        static ma_context g_context;

        static bool      g_bWasEngineInitialized = false;
        static ma_engine g_engine;

        Manager::Manager()
        {
            initContext();
            initEngine();
        }

        Manager::~Manager()
        {
            destroyEngine();
            destroyContext();
        }

        void* Manager::getContext()
        {
            return &g_context;
        }

        void* Manager::getEngine()
        {
            return &g_engine;
        }

        std::vector<Device> Manager::getDevices() const
        {
            if (!g_bWasContextInitialized)
            {
                throw std::runtime_error("Sound context wasn't initialized");
            }

            ma_device_info* outputs     = nullptr;
            ma_uint32       outputCount = 0U;

            ma_device_info* inputs     = nullptr;
            ma_uint32       inputCount = 0U;

            if (ma_context_get_devices(
                    &g_context, &outputs, &outputCount, &inputs, &inputCount
                ) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to fetch audio devices");
            }

            std::vector<Device> result = {};

            for (ma_uint32 i = 0; i < outputCount; i += 1)
            {
                result.push_back(
                    Device((void*)&outputs[i], DeviceType::Capture)
                );
            }

            for (ma_uint32 i = 0; i < inputCount; i += 1)
            {
                result.push_back(
                    Device((void*)&inputs[i], DeviceType::Playback)
                );
            }

            return result;
        }

        void Manager::initContext()
        {
            if (g_bWasContextInitialized)
            {
                return;
            }

            if (ma_context_init(NULL, 0, NULL, &g_context) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize audio context");
            }

            g_bWasContextInitialized = true;
        }

        void Manager::destroyContext()
        {
            if (!g_bWasContextInitialized)
            {
                return;
            }

            ma_context_uninit(&g_context);

            g_bWasContextInitialized = false;
        }

        void Manager::initEngine()
        {
            if (g_bWasEngineInitialized)
            {
                return;
            }

            if (ma_engine_init(NULL, &g_engine) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize audio engine");
            }

            g_bWasEngineInitialized = true;
        }

        void Manager::destroyEngine()
        {
            if (!g_bWasEngineInitialized)
            {
                return;
            }

            ma_engine_uninit(&g_engine);

            g_bWasEngineInitialized = false;
        }
    }
}