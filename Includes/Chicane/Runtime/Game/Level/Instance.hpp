#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Camera.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"

static constexpr float LINE_TRACE_STEP_SIZE = 0.1f;

namespace Chicane
{
    class Level
    {
    public:
        Level();
        virtual ~Level();

    public:
        virtual void onActivation() { return; }
        virtual void onTick(float inDeltaTime) { return; };

    public:
        // Lifecycle
        void activate();
        void tick(float inDeltaTime);

        // Actors
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

        template<class T = Actor>
        T* createActor()
        {
            m_actors.push_back(new T());

            m_actorObservable->next(m_actors);

            return static_cast<T*>(m_actors.back());
        }

        Subscription<const std::vector<Actor*>&>* watchActors(
            std::function<void (const std::vector<Actor*>&)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Components
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

        template<class T = Component>
        T* createComponent()
        {
            m_components.push_back(new T());

            m_componentObservable->next(m_components);

            return static_cast<T*>(m_components.back());
        }

        void removeComponent(Component* inComponent);

        Subscription<const std::vector<Component*>&>* watchComponents(
            std::function<void (const std::vector<Component*>&)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Camera
        bool hasCamera() const;
        CCamera* getCamera() const;

        template<typename T = Actor>
        std::vector<T*> traceLine(
            const Vec<3, float>& inOrigin,
            const Vec<3, float>& inDestination,
            const std::vector<Actor*>& inIgnoredActors
        ) const
        {
            if (!hasActors())
            {
                return {};
            }

            Vec<3, float> point = inOrigin;

            const Vec<3, float> direction = glm::normalize(
                inDestination - inOrigin
            );

            const bool bIsXPositive = inDestination.x >= inOrigin.x;
            const bool bIsYPositive = inDestination.y >= inOrigin.y;
            const bool bIsZPositive = inDestination.z >= inOrigin.z;

            bool bHasXReachedDestination = false;
            bool bHasYReachedDestination = false;
            bool bHasZReachedDestination = false;

            std::vector<T*> result {};
            while (!bHasXReachedDestination || !bHasYReachedDestination || !bHasZReachedDestination)
            {
                for (Actor* actor : m_actors)
                {
                    if (typeid(*actor) != typeid(T))
                    {
                        continue;
                    }

                    // Checking collision first is more computational cost effective
                    if (!actor->isCollidingWith(point))
                    {
                        continue;
                    }

                    bool bWillIgnored = std::find(
                        inIgnoredActors.begin(),
                        inIgnoredActors.end(),
                        actor
                    ) != inIgnoredActors.end();
                    bool bWasTraced = std::find(
                        result.begin(),
                        result.end(),
                        actor
                    ) != result.end();

                    if (bWillIgnored || bWasTraced)
                    {
                        continue;
                    }

                    result.push_back(static_cast<T*>(actor));
                }

                point.x += bHasXReachedDestination ? 0.0f : direction.x * LINE_TRACE_STEP_SIZE;
                point.y += bHasYReachedDestination ? 0.0f : direction.y * LINE_TRACE_STEP_SIZE;
                point.z += bHasZReachedDestination ? 0.0f : direction.z * LINE_TRACE_STEP_SIZE;

                bHasXReachedDestination = bIsXPositive ? (point.x >= inDestination.x) : (point.x <= inDestination.x);
                bHasYReachedDestination = bIsYPositive ? (point.y >= inDestination.y) : (point.y <= inDestination.y);
                bHasZReachedDestination = bIsZPositive ? (point.z >= inDestination.z) : (point.z <= inDestination.z);
            }

            return result;
        }

    private:
        std::vector<Actor*>                                         m_actors;
        std::unique_ptr<Observable<const std::vector<Actor*>&>>     m_actorObservable;

        std::vector<Component*>                                     m_components;
        std::unique_ptr<Observable<const std::vector<Component*>&>> m_componentObservable;
    };
}