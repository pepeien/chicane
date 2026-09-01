#pragma once

#include <algorithm>
#include <typeindex>
#include <unordered_map>

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
        inline virtual void onLoad() { return; }
        inline virtual void onUnload() { return; }

        inline virtual void onTick(float inDeltaTime) { return; }

    public:
        // Lifecycle
        void load();
        void unload();

        void tick(float inDeltaTime);

        // Actors
        bool hasActors() const;

        template <class T>
        inline bool hasActors() const
        {
            auto found = m_actors.find(std::type_index(typeid(T)));

            return found != m_actors.end() && !found->second.empty();
        }

        std::vector<Actor*> getActors() const;

        template <class T>
        inline std::vector<T*> getActors() const
        {
            std::vector<T*> result;

            auto found = m_actors.find(std::type_index(typeid(T)));
            if (found == m_actors.end())
            {
                return result;
            }

            result.reserve(found->second.size());

            for (Actor* actor : found->second)
            {
                result.push_back(static_cast<T*>(actor));
            }

            return result;
        }

        template <class T = Actor, typename... Params>
        inline T* createActor(Params... inParams)
        {
            auto& typed = m_actors[std::type_index(typeid(T))];
            typed.push_back(new T(inParams...));

            Actor* added = typed.back();
            added->setScene(this);

            m_actorCount++;

            String typeName = added->getTypeName();
            if (typeName.isEmpty())
            {
                typeName = "Actor";
            }

            if (typed.size() <= 1)
            {
                added->setId(typeName);
            }
            else
            {
                added->setId(String::sprint("%s%d", typeName.toChar(), typed.size()));
            }

            if (isLoaded())
            {
                added->onLoad();
            }

            if (!m_actorsObservable.isEmpty())
            {
                m_actorsObservable.next(getActors());
            }

            return static_cast<T*>(added);
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
            auto found = m_components.find(std::type_index(typeid(T)));

            return found != m_components.end() && !found->second.empty();
        }

        std::vector<Component*> getComponents() const;

        template <class T>
        inline std::vector<T*> getComponents() const
        {
            std::vector<T*> result;

            auto found = m_components.find(std::type_index(typeid(T)));
            if (found == m_components.end())
            {
                return result;
            }

            result.reserve(found->second.size());

            for (Component* component : found->second)
            {
                result.push_back(static_cast<T*>(component));
            }

            return result;
        }

        template <class T>
        inline std::vector<T*> getActiveComponents() const
        {
            std::vector<T*> result;

            auto found = m_components.find(std::type_index(typeid(T)));
            if (found == m_components.end())
            {
                return result;
            }

            result.reserve(found->second.size());

            for (Component* component : found->second)
            {
                if (!component->isActive())
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
            auto& typed = m_components[std::type_index(typeid(T))];
            typed.push_back(new T(inParams...));

            Component* added = typed.back();
            added->setScene(this);

            m_componentCount++;

            if (isLoaded())
            {
                added->onLoad();
            }

            if (!m_componentsObservable.isEmpty())
            {
                m_componentsObservable.next(getComponents());
            }

            return static_cast<T*>(added);
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
            auto found = m_actors.find(std::type_index(typeid(T)));
            if (found == m_actors.end() || found->second.empty())
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
                for (Actor* actor : found->second)
                {
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

        template <typename T = Actor>
        inline std::size_t getActorCount() const
        {
            if (typeid(T) == typeid(Actor))
            {
                return m_actorCount;
            }

            auto found = m_actors.find(std::type_index(typeid(T)));
            if (found == m_actors.end())
            {
                return 0;
            }

            return found->second.size();
        }

        template <typename T = Component>
        inline std::size_t getComponentCount() const
        {
            if (typeid(T) == typeid(Component))
            {
                return m_componentCount;
            }

            auto found = m_components.find(std::type_index(typeid(T)));
            if (found == m_components.end())
            {
                return 0;
            }

            return found->second.size();
        }

    protected:
        bool isLoaded() const;

        void tickActors(float inDeltaTime);
        void deleteActors();

        void tickComponents(float inDeltaTime);
        void deleteComponents();

    private:
        bool                                                         m_bIsLoaded;

        std::size_t                                                  m_actorCount;
        std::unordered_map<std::type_index, std::vector<Actor*>>     m_actors;
        ActorsObservable                                             m_actorsObservable;

        std::size_t                                                  m_componentCount;
        std::unordered_map<std::type_index, std::vector<Component*>> m_components;
        ComponentsObservable                                         m_componentsObservable;
    };
}
