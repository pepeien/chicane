#pragma once

#include <algorithm>

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

constexpr inline float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
    class CHICANE_RUNTIME Scene
    {
    public:
        using ActorsObservable   = EventObservable<std::vector<Actor*>>;
        using ActorsSubscription = EventSubscription<std::vector<Actor*>>;

        using ComponentsObservable   = EventObservable<std::vector<Component*>>;
        using ComponentsSubscription = EventSubscription<std::vector<Component*>>;

    public:
        Scene();
        virtual ~Scene();

    public:
        inline virtual void onActivation() { return; }
        inline virtual void onTick(float inDeltaTime) { return; }
        inline virtual void onDeactivation() { return; }

    public:
        // Lifecycle
        void activate();
        void tick(float inDeltaTime);
        void deactivate();

        // Actors
        bool hasActors() const;

        template <class T>
        inline bool hasActors() const
        {
            for (Actor* actor : getActors())
            {
                if (typeid(*actor) != typeid(T))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        const std::vector<Actor*>& getActors() const;

        template <class T>
        inline std::vector<T*> getActors() const
        {
            std::vector<T*> result{};

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

        template <class T = Actor, typename... Params>
        inline T* createActor(Params... inParams)
        {
            m_actors.push_back(new T(inParams...));

            m_actorsObservable.next(m_actors);

            return static_cast<T*>(m_actors.back());
        }

        void removeActor(Actor* inActor);

        ActorsSubscription watchActors(
            ActorsSubscription::NextCallback     inNext,
            ActorsSubscription::ErrorCallback    inError    = nullptr,
            ActorsSubscription::CompleteCallback inComplete = nullptr
        );

        // Components
        bool hasComponents() const;

        template <class T>
        inline bool hasComponents() const
        {
            for (Component* component : getComponents())
            {
                if (typeid(*component) != typeid(T))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        const std::vector<Component*>& getComponents() const;

        template <class T>
        inline std::vector<T*> getComponents() const
        {
            std::vector<T*> result{};

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

        template <class T>
        inline std::vector<T*> getActiveComponents() const
        {
            std::vector<T*> result{};

            for (Component* component : getComponents())
            {
                if (typeid(*component) != typeid(T) || !component->isActive())
                {
                    continue;
                }

                result.push_back(static_cast<T*>(component));
            }

            return result;
        }

        template <class T = Component, typename... Params>
        inline T* createComponent(Params... inParams)
        {
            m_components.push_back(new T(inParams...));

            m_componentsObservable.next(m_components);

            return static_cast<T*>(m_components.back());
        }

        void removeComponent(Component* inComponent);

        ComponentsSubscription watchComponents(
            ComponentsSubscription::NextCallback     inNext,
            ComponentsSubscription::ErrorCallback    inError    = nullptr,
            ComponentsSubscription::CompleteCallback inComplete = nullptr
        );

        // Helper
        template <typename T = Actor>
        inline std::vector<T*> traceLine(
            const Vec3& inOrigin, const Vec3& inDestination, const std::vector<Actor*>& inIgnoredActors
        ) const
        {
            if (!hasActors())
            {
                return {};
            }

            Vec3  point = inOrigin;
            Vec3  delta = inDestination - inOrigin;

            float maxDistance = glm::length(glm::vec3(delta.x, delta.y, delta.z));

            if (maxDistance == 0.0f)
            {
                return {};
            }

            Vec3            direction = delta / maxDistance;

            std::vector<T*> result{};
            float           traveled = 0.0f;

            while (traveled <= maxDistance)
            {
                for (Actor* actor : m_actors)
                {
                    if (typeid(*actor) != typeid(T))
                    {
                        continue;
                    }

                    if (!actor->isCollidingWith(point))
                    {
                        continue;
                    }

                    if (std::find(inIgnoredActors.begin(), inIgnoredActors.end(), actor) != inIgnoredActors.end())
                    {
                        continue;
                    }

                    if (std::find(result.begin(), result.end(), actor) != result.end())
                    {
                        continue;
                    }

                    result.push_back(static_cast<T*>(actor));
                }

                point += direction * LINE_TRACE_STEP_SIZE;
                traveled += LINE_TRACE_STEP_SIZE;
            }

            return result;
        }

    protected:
        void tickActors(float inDeltaTime);
        void deleteActors();

        void tickComponents(float inDeltaTime);
        void deleteComponents();

    private:
        std::vector<Actor*>     m_actors;
        ActorsObservable        m_actorsObservable;

        std::vector<Component*> m_components;
        ComponentsObservable    m_componentsObservable;
    };
}