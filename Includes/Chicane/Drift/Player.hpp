#pragma once

#include <vector>

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Direction.hpp"
#include "Chicane/Drift/Status.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT Player
        {
        public:
            using CompleteObservable   = EventObservable<>;
            using CompleteSubscription = EventSubscription<>;

        public:
            Player();

        public:
            void play();
            void pause();
            void stop();
            void reset();

            void tick(float inDeltaTime);

            bool isPlaying() const;
            bool isPaused() const;
            bool isStopped() const;
            bool isWaiting() const;
            bool isFinished() const;

            Status getStatus() const;

            void setClip(const Clip& inValue);
            const Clip& getClip() const;

            void setSpeed(float inValue);
            float getSpeed() const;

            void setDelay(float inValue);
            float getDelay() const;

            void setDirection(Direction inValue);
            Direction getDirection() const;

            void setFillBackwards(bool inValue);
            bool fillsBackwards() const;

            void setFillForwards(bool inValue);
            bool fillsForwards() const;

            float getElapsed() const;
            float getProgress() const;

            std::vector<float> sample(const String& inTrack) const;

            CompleteSubscription watchComplete(
                CompleteSubscription::EmptyCallback    inNext,
                CompleteSubscription::ErrorCallback    inError    = nullptr,
                CompleteSubscription::CompleteCallback inComplete = nullptr
            );

        private:
            void finish();
            float playbackTime() const;

        private:
            Clip               m_clip;
            float              m_elapsed;
            float              m_delay;
            float              m_remainingDelay;
            float              m_speed;
            int                m_iteration;
            Direction          m_direction;
            Status             m_status;
            bool               m_bFillBackwards;
            bool               m_bFillForwards;
            bool               m_bFinished;
            CompleteObservable m_onComplete;
        };
    }
}
