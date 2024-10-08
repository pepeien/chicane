#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Game/Actor/Component.hpp"

namespace Chicane
{
    class Level
    {
    public:
        Level();
        virtual ~Level();

    public:
        virtual void onActivation() { return; }

    public:
        bool hasActors() const;
        const std::vector<Actor*>& getActors() const;
        void addActor(Actor* inActor);
        void watchActors(
            std::function<void (Actor*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        template<class T>
        std::vector<T*> getActors() const
        {
            std::vector<T*> result {};

            for (Actor* actor : getActors())
            {
                if (typeid(*actor) != typeid(T))
                {
                    continue;
                }

                result.push_back(static_cast<T*>(actor));
            }

            return result;
        }

        bool hasComponents() const;
        const std::vector<ActorComponent*>& getComponents() const;
        void addComponent(ActorComponent* inComponent);
        void removeComponent(ActorComponent* inComponent);
        void watchComponents(
            std::function<void (const std::vector<ActorComponent*>&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        template<class T>
        std::vector<T*> getComponents() const
        {
            std::vector<T*> result {};

            for (ActorComponent* component : getComponents())
            {
                if (typeid(*component) != typeid(T))
                {
                    continue;
                }

                result.push_back(static_cast<T*>(component));
            }

            return result;
        }

    private:
        std::vector<Actor*> m_actors;
        std::unique_ptr<Observable<Actor*>> m_actorObservable;

        std::vector<ActorComponent*> m_components;
        std::unique_ptr<Observable<const std::vector<ActorComponent*>&>> m_componentObservable;
    };
}