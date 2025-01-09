#pragma once

#include "Chicane/Base.hpp"

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