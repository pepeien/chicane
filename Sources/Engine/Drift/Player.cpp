#include "Chicane/Drift/Player.hpp"

#include <algorithm>
#include <cmath>

namespace Chicane
{
    namespace Drift
    {
        Player::Player()
            : m_clip(),
              m_elapsed(0.0f),
              m_delay(0.0f),
              m_remainingDelay(0.0f),
              m_speed(1.0f),
              m_iteration(0),
              m_direction(Direction::Forward),
              m_status(Status::Stopped),
              m_bFillBackwards(false),
              m_bFillForwards(false),
              m_bFinished(false),
              m_onComplete()
        {}

        void Player::play()
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

        void Player::pause()
        {
            if (m_status != Status::Playing)
            {
                return;
            }

            m_status = Status::Paused;
        }

        void Player::stop()
        {
            m_status = Status::Stopped;

            reset();
        }

        void Player::reset()
        {
            m_elapsed        = 0.0f;
            m_remainingDelay = std::max(0.0f, m_delay);
            m_iteration      = 0;
            m_bFinished      = false;
        }

        void Player::tick(float inDeltaTime)
        {
            if (m_status != Status::Playing)
            {
                return;
            }

            const float step = inDeltaTime * std::max(0.0f, m_speed);

            if (m_remainingDelay > 0.0f)
            {
                m_remainingDelay -= step;

                if (m_remainingDelay > 0.0f)
                {
                    return;
                }

                m_elapsed        = -m_remainingDelay;
                m_remainingDelay = 0.0f;
            }
            else
            {
                m_elapsed += step;
            }

            const float duration = std::max(0.0f, m_clip.duration);

            if (duration <= 0.0f)
            {
                finish();

                return;
            }

            while (m_elapsed >= duration)
            {
                m_iteration++;

                const bool bInfinite = m_clip.iterations <= 0;
                const bool bOnce     = m_clip.loop == Loop::Once || (!bInfinite && m_iteration >= m_clip.iterations);

                if (bOnce)
                {
                    m_elapsed = duration;

                    finish();

                    return;
                }

                if (m_clip.loop == Loop::PingPong)
                {
                    m_direction =
                        m_direction == Direction::Forward ? Direction::Reverse : Direction::Forward;
                }

                m_elapsed -= duration;
            }
        }

        bool Player::isPlaying() const
        {
            return m_status == Status::Playing;
        }

        bool Player::isPaused() const
        {
            return m_status == Status::Paused;
        }

        bool Player::isStopped() const
        {
            return m_status == Status::Stopped;
        }

        bool Player::isWaiting() const
        {
            return m_remainingDelay > 0.0f;
        }

        bool Player::isFinished() const
        {
            return m_bFinished;
        }

        Status Player::getStatus() const
        {
            return m_status;
        }

        void Player::setClip(const Clip& inValue)
        {
            m_clip = inValue;

            if (m_clip.loop == Loop::PingPong)
            {
                m_direction = Direction::Forward;
            }
        }

        const Clip& Player::getClip() const
        {
            return m_clip;
        }

        void Player::setSpeed(float inValue)
        {
            m_speed = std::max(0.0f, inValue);
        }

        float Player::getSpeed() const
        {
            return m_speed;
        }

        void Player::setDelay(float inValue)
        {
            m_delay          = std::max(0.0f, inValue);
            m_remainingDelay = m_delay;
        }

        float Player::getDelay() const
        {
            return m_delay;
        }

        void Player::setDirection(Direction inValue)
        {
            m_direction = inValue;
        }

        Direction Player::getDirection() const
        {
            return m_direction;
        }

        void Player::setFillBackwards(bool inValue)
        {
            m_bFillBackwards = inValue;
        }

        bool Player::fillsBackwards() const
        {
            return m_bFillBackwards;
        }

        void Player::setFillForwards(bool inValue)
        {
            m_bFillForwards = inValue;
        }

        bool Player::fillsForwards() const
        {
            return m_bFillForwards;
        }

        float Player::getElapsed() const
        {
            return m_elapsed;
        }

        float Player::getProgress() const
        {
            if (m_clip.duration <= 0.0f)
            {
                return 1.0f;
            }

            return std::clamp(playbackTime() / m_clip.duration, 0.0f, 1.0f);
        }

        std::vector<float> Player::sample(const String& inTrack) const
        {
            const Track* track = m_clip.getTrack(inTrack);

            if (!track)
            {
                return {};
            }

            return track->sample(playbackTime());
        }

        Player::CompleteSubscription Player::watchComplete(
            CompleteSubscription::EmptyCallback    inNext,
            CompleteSubscription::ErrorCallback    inError,
            CompleteSubscription::CompleteCallback inComplete
        )
        {
            return m_onComplete.subscribe(inNext, inError, inComplete);
        }

        void Player::finish()
        {
            if (m_bFinished)
            {
                return;
            }

            m_status    = Status::Stopped;
            m_bFinished = true;

            m_onComplete.next();
        }

        float Player::playbackTime() const
        {
            const float duration = std::max(0.0f, m_clip.duration);

            if (isWaiting())
            {
                return m_bFillBackwards ? (m_direction == Direction::Reverse ? duration : 0.0f) : 0.0f;
            }

            const float elapsed = std::clamp(m_elapsed, 0.0f, duration);

            if (m_direction == Direction::Reverse)
            {
                return duration - elapsed;
            }

            return elapsed;
        }
    }
}
