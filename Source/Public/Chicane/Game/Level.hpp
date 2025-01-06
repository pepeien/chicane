#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Game/Transformable/Actor.hpp"
#include "Chicane/Game/Transformable/Component.hpp"

namespace Chicane
{
    class Level
    {
    public:
        Level();
        virtual ~Level();

    public:
        virtual void onActivation() { return; }
        virtual void onTick(float inDeltaTime);

    public:
        void activate();

        bool hasActors() const;
        const std::vector<Actor*>& getActors() const;
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
        void addActor(Actor* inActor);
        void watchActors(
            std::function<void (Actor*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        bool hasComponents() const;
        const std::vector<Component*>& getComponents() const;
        template<class T>
        std::vector<T*> getComponents() const
        {
            std::vector<T*> result {};

            for (Component* component : getComponents())
            {
                if (typeid(*component) != typeid(T))
                {
                    continue;
                }

                result.push_back(static_cast<T*>(component));
            }

            return result;
        }
        void addComponent(Component* inComponent);
        void removeComponent(Component* inComponent);
        void watchComponents(
            std::function<void (const std::vector<Component*>&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    private:
        std::vector<Actor*> m_actors;
        std::unique_ptr<Observable<Actor*>> m_actorObservable;

        std::vector<Component*> m_components;
        std::unique_ptr<Observable<const std::vector<Component*>&>> m_componentObservable;
    };
}