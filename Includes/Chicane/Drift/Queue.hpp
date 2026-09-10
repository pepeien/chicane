#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Player.hpp"

namespace Chicane
{
    namespace Drift
    {
        class CHICANE_DRIFT Queue
        {
        public:
            Queue();

        public:
            void add(const Clip& inClip);
            void remove(const String& inName);
            void clear();

            bool has(const String& inName) const;
            const Clip* get(const String& inName) const;

            void play(const String& inName);
            void enqueue(const String& inName);
            void stop();

            void tick(float inDeltaTime);

            bool isPlaying() const;
            bool isIdle() const;
            const String& getPlaying() const;

            Player* getPlayer();
            const Player* getPlayer() const;

            std::vector<float> sample(const String& inTrack) const;

        private:
            void start(const String& inName);
            void advance();

        private:
            std::unordered_map<String, Clip> m_clips;
            std::vector<String>              m_pending;
            String                           m_playing;
            Player                           m_player;
        };
    }
}
