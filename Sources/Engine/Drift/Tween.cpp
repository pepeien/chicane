#include "Chicane/Drift/Tween.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Drift/Interpolation.hpp"

namespace Chicane
{
    namespace Drift
    {
        Tween::Tween()
            : m_from({}),
              m_to({}),
              m_duration(0.0f),
              m_delay(0.0f),
              m_elapsed(0.0f),
              m_speed(1.0f),
              m_loop(Loop::Once),
              m_status(Status::Stopped),
              m_easing(EasingCurve::ease()),
              m_bFinished(false),
              m_onComplete()
        {}

        void Tween::play()
        {
            if (m_status == Status::Playing)
            {
                return;
            }

            if (m_status == Status::Stopped)
            {
                reset();
            }

            m_status    = Status::Playing;
            m_bFinished = false;
        }

        void Tween::pause()
        {
            if (m_status != Status::Playing)
            {
                return;
            }

            m_status = Status::Paused;
        }

        void Tween::stop()
        {
            m_status = Status::Stopped;

            reset();
        }

        void Tween::reset()
        {
            m_elapsed   = -std::max(0.0f, m_delay);
            m_bFinished = false;
        }

        void Tween::tick(float inDeltaTime)
        {
            if (m_status != Status::Playing)
            {
                return;
            }

            if (m_duration <= 0.0f)
            {
                m_elapsed = m_duration;

                finish();

                return;
            }

            m_elapsed += inDeltaTime * std::max(0.0f, m_speed);

            if (m_elapsed < m_duration)
            {
                return;
            }

            if (m_loop == Loop::Once)
            {
                m_elapsed = m_duration;

                finish();

                return;
            }

            if (m_loop == Loop::Repeat)
            {
                m_elapsed = std::fmod(m_elapsed, m_duration);

                return;
            }

            const float span = m_duration * 2.0f;

            m_elapsed = std::fmod(m_elapsed, span);
        }

        bool Tween::isPlaying() const
        {
            return m_status == Status::Playing;
        }

        bool Tween::isPaused() const
        {
            return m_status == Status::Paused;
        }

        bool Tween::isStopped() const
        {
            return m_status == Status::Stopped;
        }

        bool Tween::isFinished() const
        {
            return m_bFinished;
        }

        bool Tween::hasStarted() const
        {
            return m_elapsed >= 0.0f;
        }

        Status Tween::getStatus() const
        {
            return m_status;
        }

        void Tween::setFrom(const std::vector<float>& inValue)
        {
            m_from = inValue;
        }

        const std::vector<float>& Tween::getFrom() const
        {
            return m_from;
        }

        void Tween::setTo(const std::vector<float>& inValue)
        {
            m_to = inValue;
        }

        const std::vector<float>& Tween::getTo() const
        {
            return m_to;
        }

        void Tween::setDuration(float inValue)
        {
            m_duration = std::max(0.0f, inValue);
        }

        float Tween::getDuration() const
        {
            return m_duration;
        }

        void Tween::setDelay(float inValue)
        {
            m_delay = std::max(0.0f, inValue);
        }

        float Tween::getDelay() const
        {
            return m_delay;
        }

        void Tween::setSpeed(float inValue)
        {
            m_speed = std::max(0.0f, inValue);
        }

        float Tween::getSpeed() const
        {
            return m_speed;
        }

        void Tween::setLoop(Loop inValue)
        {
            m_loop = inValue;
        }

        Loop Tween::getLoop() const
        {
            return m_loop;
        }

        void Tween::setEasing(const EasingCurve& inValue)
        {
            m_easing = inValue;
        }

        const EasingCurve& Tween::getEasing() const
        {
            return m_easing;
        }

        float Tween::getElapsed() const
        {
            return m_elapsed;
        }

        float Tween::getProgress() const
        {
            if (m_duration <= 0.0f)
            {
                return 1.0f;
            }

            if (m_elapsed <= 0.0f)
            {
                return 0.0f;
            }

            if (m_loop == Loop::PingPong)
            {
                const float span  = m_duration * 2.0f;
                const float cycle = std::fmod(m_elapsed, span);
                const float linear =
                    cycle <= m_duration ? (cycle / m_duration) : (1.0f - ((cycle - m_duration) / m_duration));

                return std::clamp(linear, 0.0f, 1.0f);
            }

            return std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
        }

        std::vector<float> Tween::getValue() const
        {
            return mix(m_from, m_to, easedProgress());
        }

        Tween::CompleteSubscription Tween::watchComplete(
            CompleteSubscription::EmptyCallback    inNext,
            CompleteSubscription::ErrorCallback    inError,
            CompleteSubscription::CompleteCallback inComplete
        )
        {
            return m_onComplete.subscribe(inNext, inError, inComplete);
        }

        void Tween::finish()
        {
            if (m_bFinished)
            {
                return;
            }

            m_status    = Status::Stopped;
            m_bFinished = true;

            m_onComplete.next();
        }

        float Tween::easedProgress() const
        {
            return m_easing.evaluate(getProgress());
        }
    }
}
