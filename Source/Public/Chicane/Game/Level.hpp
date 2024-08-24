#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    class Level
    {
    public:
        Level();
        ~Level();

    public:
        bool hasActors() const;
        const std::vector<Actor*>& getActors() const;
        std::vector<Actor*> getDrawableActors() const;
        std::uint32_t getActorCount() const;
        void addActor(Actor* inActor);
        Subscription<Actor*>* watchActors(
            std::function<void (Actor*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    private:
        std::vector<Actor*> m_actors;
        Observable<Actor*>* m_observable;
    };
}