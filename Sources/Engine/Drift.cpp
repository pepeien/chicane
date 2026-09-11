#include "Chicane/Drift.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Engine.hpp"
#include "Chicane/Drift/Player.hpp"
#include "Chicane/Drift/Queue.hpp"
#include "Chicane/Drift/Track.hpp"

namespace Chicane
{
    namespace Drift
    {
        void applyPose(Transformable& inTarget, const std::vector<float>& inValue);

        constexpr const char* TRANSFORM_TRACK = "transform";

        static Engine& engine()
        {
            static Engine instance;

            return instance;
        }

        static bool g_bWasInitialized = false;

        static const Track* findApplyTrack(const Clip& inClip)
        {
            if (const Track* track = inClip.getTrack(TRANSFORM_TRACK))
            {
                return track;
            }

            if (inClip.tracks.size() == 1)
            {
                return &inClip.tracks.front();
            }

            return nullptr;
        }

        static void apply(Transformable& inTarget, const Queue& inQueue)
        {
            const Player* player = inQueue.getPlayer();
            if (!player)
            {
                return;
            }

            const Track* track = findApplyTrack(player->getClip());
            if (!track)
            {
                return;
            }

            applyPose(inTarget, inQueue.sample(track->name));
        }

        static Queue* binding(Transformable& inTarget, bool bInCreate)
        {
            Engine& state = engine();
            auto    found = state.bindings.find(&inTarget);
            if (found != state.bindings.end())
            {
                return found->second.get();
            }

            if (!bInCreate)
            {
                return nullptr;
            }

            return state.bindings.insert_or_assign(&inTarget, std::make_unique<Queue>()).first->second.get();
        }

        static bool bindClip(Queue& outQueue, const String& inName)
        {
            const auto found = engine().clips.find(inName);
            if (found == engine().clips.end())
            {
                return false;
            }

            outQueue.add(found->second);

            return true;
        }

        void init()
        {
            if (g_bWasInitialized)
            {
                return;
            }

            g_bWasInitialized = true;
        }

        void shutdown()
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            engine().clips.clear();
            engine().bindings.clear();
            g_bWasInitialized = false;
        }

        void tick(float inDeltaTime)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            for (auto& [target, queue] : engine().bindings)
            {
                queue->tick(inDeltaTime);
                apply(*target, *queue);
            }
        }

        void add(const Clip& inClip)
        {
            if (!g_bWasInitialized || inClip.name.isEmpty())
            {
                return;
            }

            engine().clips.insert_or_assign(inClip.name, inClip);
        }

        void remove(const String& inName)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            engine().clips.erase(inName);

            for (auto& [target, queue] : engine().bindings)
            {
                queue->remove(inName);
            }
        }

        bool has(const String& inName)
        {
            if (!g_bWasInitialized)
            {
                return false;
            }

            return engine().clips.find(inName) != engine().clips.end();
        }

        void play(Transformable& inTarget, const String& inClip)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            Queue* queue = binding(inTarget, true);
            if (!queue || !bindClip(*queue, inClip))
            {
                return;
            }

            queue->play(inClip);
        }

        void enqueue(Transformable& inTarget, const String& inClip)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            Queue* queue = binding(inTarget, true);
            if (!queue || !bindClip(*queue, inClip))
            {
                return;
            }

            queue->enqueue(inClip);
        }

        void stop(Transformable& inTarget)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            if (Queue* queue = binding(inTarget, false))
            {
                queue->stop();
            }
        }

        void unbind(Transformable& inTarget)
        {
            if (!g_bWasInitialized)
            {
                return;
            }

            engine().bindings.erase(&inTarget);
        }

        bool isPlaying(const Transformable& inTarget)
        {
            if (!g_bWasInitialized)
            {
                return false;
            }

            const auto found = engine().bindings.find(const_cast<Transformable*>(&inTarget));
            if (found == engine().bindings.end())
            {
                return false;
            }

            return found->second->isPlaying();
        }
    }
}
