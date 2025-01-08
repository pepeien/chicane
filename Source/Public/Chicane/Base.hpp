#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#define SDL_MAIN_HANDLED

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#ifdef NDEBUG
    #define IS_DEBUGGING false
#else
    #define IS_DEBUGGING true
#endif

#ifdef _unix__
    #define OS_LINUX
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define OS_WINDOWS
#endif

// System
#include <algorithm>
#include <any>
#include <array>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <math.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>

// Vendor
#include "base64.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "pugixml.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"

#include "stb/stb_image.h"

// Settings
static constexpr auto APPLICATION_NAME = "Chicane Engine";
static constexpr auto ENGINE_NAME      = "Chicane";

// Assets
static constexpr std::uint32_t TEXTURE_MAX_COUNT   = 100000;
static constexpr std::uint32_t TEXTURE_IMAGE_COUNT = 1;

// Z-Up Right Handed
static constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f, 0.0f, 1.0f);

namespace Chicane
{
    struct FrameTelemetry
    {
    public:
        static float deltaToMs(std::clock_t inDelta)
        {
            return (inDelta / (float)CLOCKS_PER_SEC) * 1000.0f;
        }

        static float deltaToTick(std::clock_t inDelta)
        {
            return inDelta / 1000.0f;
        }

    public:
        float deltaToMs() const
        {
            return FrameTelemetry::deltaToMs(delta);
        }

        float deltaToTick() const
        {
            return FrameTelemetry::deltaToTick(delta);
        }

        void startCapture()
        {
            m_beginFrame = std::clock();
        }

        void endCapture()
        {
            m_endFrame = std::clock();

            delta += m_endFrame - m_beginFrame;
            count += 1;

            if (delta < CLOCKS_PER_SEC)
            {
                return;
            }

            rate  = std::uint32_t((count * 0.5) + (rate * 0.5));
            count = 0;
            time  = 1000.0f / float(rate ==0 ? 0.001 : rate);
            delta = 0;
        }

    public:
        std::clock_t  delta = 0;
        std::uint32_t count = 0;
        std::uint32_t rate  = 0;
        float         time  = 0.0f;

    private:
        std::clock_t  m_beginFrame;
        std::clock_t  m_endFrame;
    };

    struct Telemetry
    {
    public:
        FrameTelemetry frame {};

    public:

    public:
        void startCapture()
        {
            frame.startCapture();
        }

        void endCapture()
        {
            frame.endCapture();
        }
    };
}