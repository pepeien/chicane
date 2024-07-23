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
        bool hasSkybox();
        Box::Instance getSkybox();
        void setSkybox(const Box::Instance& inSkybox);

        bool hasActors();
        std::vector<Actor*> getActors();
        void addActor(Actor* inActor);
        Subscription<Actor*>* addActorSubscription(
            std::function<void (Actor*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    private:
        Box::Instance m_skybox;
        std::vector<Actor*> m_actors;
        Observable<Actor*>* m_observable;
    };
}