#pragma once

#include <vector>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Easing.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Status.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT Tween
        {
        public:
            using CompleteObservable   = EventObservable<>;
            using CompleteSubscription = EventSubscription<>;

        public:
            Tween();

        public:
            void play();
            void pause();
            void stop();
            void reset();

            void tick(float inDeltaTime);

            bool isPlaying() const;
            bool isPaused() const;
            bool isStopped() const;
            bool isFinished() const;
            bool hasStarted() const;

            Status getStatus() const;

            void setFrom(const std::vector<float>& inValue);
            const std::vector<float>& getFrom() const;

            void setTo(const std::vector<float>& inValue);
            const std::vector<float>& getTo() const;

            void setDuration(float inValue);
            float getDuration() const;

            void setDelay(float inValue);
            float getDelay() const;

            void setSpeed(float inValue);
            float getSpeed() const;

            void setLoop(Loop inValue);
            Loop getLoop() const;

            void setEasing(const EasingCurve& inValue);
            const EasingCurve& getEasing() const;

            float getElapsed() const;
            float getProgress() const;
            std::vector<float> getValue() const;

            CompleteSubscription watchComplete(
                CompleteSubscription::EmptyCallback    inNext,
                CompleteSubscription::ErrorCallback    inError    = nullptr,
                CompleteSubscription::CompleteCallback inComplete = nullptr
            );

        private:
            void finish();
            float easedProgress() const;

        private:
            std::vector<float> m_from;
            std::vector<float> m_to;
            float              m_duration;
            float              m_delay;
            float              m_elapsed;
            float              m_speed;
            Loop               m_loop;
            Status             m_status;
            EasingCurve        m_easing;
            bool               m_bFinished;
            CompleteObservable m_onComplete;
        };
    }
}
