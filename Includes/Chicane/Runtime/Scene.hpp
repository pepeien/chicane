#pragma once

#include <algorithm>

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"

static inline constexpr const float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
    class CHICANE_RUNTIME Scene
    {
    public:
        using ActorsObservable       = EventObservable<std::vector<Actor*>>;
        using ActorsSubscription     = EventSubscription<std::vector<Actor*>>;

        using ComponentsObservable   = EventObservable<std::vector<Component*>>;
        using ComponentsSubscription = EventSubscription<std::vector<Component*>>;

    public:
        Scene();
        virtual ~Scene();

    public:
        virtual void onActivation() { return; }
        virtual void onTick(float inDeltaTime) { return; }
        virtual void onDeactivation() { return; }

    public:
        // Lifecycle
        void activate();
        void tick(float inDeltaTime);
        void deactivate();

        // Actors
        bool hasActors() const;

        template <class T>
        bool hasActors() const
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
        std::vector<T*> getActors() const
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

        template <class T = Actor>
        T* createActor()
        {
            m_actors.push_back(new T());

            m_actorsObservable.next(m_actors);

            refreshDefaultCamera();

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
        bool hasComponents() const
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
        std::vector<T*> getComponents() const
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
        std::vector<T*> getActiveComponents() const
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

        template <class T = Component>
        T* createComponent()
        {
            m_components.push_back(new T());

            m_componentsObservable.next(m_components);

            refreshDefaultCamera();

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
        std::vector<T*> traceLine(
            const Vec3& inOrigin, const Vec3& inDestination, const std::vector<Actor*>& inIgnoredActors
        ) const
        {
            if (!hasActors())
            {
                return {};
            }

            Vec3            point                   = inOrigin;

            const Vec3      direction               = glm::normalize(inDestination - inOrigin);

            const bool      bIsXPositive            = inDestination.x >= inOrigin.x;
            const bool      bIsYPositive            = inDestination.y >= inOrigin.y;
            const bool      bIsZPositive            = inDestination.z >= inOrigin.z;

            bool            bHasXReachedDestination = false;
            bool            bHasYReachedDestination = false;
            bool            bHasZReachedDestination = false;

            std::vector<T*> result{};
            while (!bHasXReachedDestination || !bHasYReachedDestination || !bHasZReachedDestination)
            {
                for (Actor* actor : m_actors)
                {
                    if (typeid(*actor) != typeid(T))
                    {
                        continue;
                    }

                    // Checking collision first is more computational cost
                    // effective
                    if (!actor->isCollidingWith(point))
                    {
                        continue;
                    }

                    bool bWillIgnored = std::find(inIgnoredActors.begin(), inIgnoredActors.end(), actor) !=
                                        inIgnoredActors.end();
                    bool bWasTraced = std::find(result.begin(), result.end(), actor) != result.end();

                    if (bWillIgnored || bWasTraced)
                    {
                        continue;
                    }

                    result.push_back(static_cast<T*>(actor));
                }

                point.x += bHasXReachedDestination ? 0.0f : direction.x * LINE_TRACE_STEP_SIZE;
                point.y += bHasYReachedDestination ? 0.0f : direction.y * LINE_TRACE_STEP_SIZE;
                point.z += bHasZReachedDestination ? 0.0f : direction.z * LINE_TRACE_STEP_SIZE;

                bHasXReachedDestination =
                    bIsXPositive ? point.x >= inDestination.x : point.x <= inDestination.x;
                bHasYReachedDestination =
                    bIsYPositive ? point.y >= inDestination.y : point.y <= inDestination.y;
                bHasZReachedDestination =
                    bIsZPositive ? point.z >= inDestination.z : point.z <= inDestination.z;
            }

            return result;
        }

    protected:
        void tickActors(float inDeltaTime);
        void deleteActors();

        void tickComponents(float inDeltaTime);
        void deleteComponents();

        void createDefaultCamera();
        void removeDefaultCamera();
        void refreshDefaultCamera();

    private:
        std::vector<Actor*>     m_actors;
        ActorsObservable        m_actorsObservable;

        std::vector<Component*> m_components;
        ComponentsObservable    m_componentsObservable;

        CCamera*                m_defaultCamera;
    };
}