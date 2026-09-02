#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Core/View/Frustum.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

constexpr inline float LINE_TRACE_STEP_SIZE = 0.1f;
constexpr inline float SPATIAL_CELL_SIZE    = 64.0f;

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

            auto            found = m_actors.find(std::type_index(typeid(T)));
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
            m_actorCount++;

            attachObject(added, "Actor");

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

            auto            found = m_components.find(std::type_index(typeid(T)));
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

            auto            found = m_components.find(std::type_index(typeid(T)));
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
            m_componentCount++;

            attachObject(added, "Component");

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

        // Objects
        bool hasObject(const String& inId) const;
        Object* getObject(const String& inId) const;
        void setObjectId(Object* inObject, const String& inId);

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

        template <class Function>
        inline void forEachInFrustum(const ViewFrustum& inFrustum, Function&& inFunction) const
        {
            std::unordered_set<Object*> seen;

            for (const auto& [key, cell] : m_cells)
            {
                if (!inFrustum.contains(cell.min, cell.max))
                {
                    continue;
                }

                for (Object* object : cell.objects)
                {
                    if (!seen.insert(object).second)
                    {
                        continue;
                    }

                    inFunction(object);
                }
            }
        }

    protected:
        bool isLoaded() const;

        void tickActors(float inDeltaTime);
        void deleteActors();

        void tickComponents(float inDeltaTime);
        void deleteComponents();

        void updateSpatial(Object* inObject);
        void removeSpatial(Object* inObject);

        std::uint64_t makeCellKey(int inX, int inY, int inZ) const;
        std::uint64_t makeCellKey(const Vec3& inPosition) const;
        void collectCellKeys(const Object* inObject, std::vector<std::uint64_t>& outKeys) const;
        void insertIntoCell(Object* inObject, std::uint64_t inKey);
        void eraseFromCell(Object* inObject, std::uint64_t inKey);

    private:
        friend Object;

        void attachObject(Object* inObject, const String& inFallback);
        void assignUniqueId(Object* inObject, const String& inFallback);
        String makeUniqueId(const String& inBase) const;
        void ensureUniqueId(const String& inId, const Object* inIgnored) const;

        struct SpatialCell
        {
            Vec3                 min     = Vec3::Zero();
            Vec3                 max     = Vec3::Zero();
            std::vector<Object*> objects = {};
        };

        bool                                                         m_bIsLoaded;

        std::size_t                                                  m_actorCount;
        std::unordered_map<std::type_index, std::vector<Actor*>>     m_actors;
        ActorsObservable                                             m_actorsObservable;

        std::size_t                                                  m_componentCount;
        std::unordered_map<std::type_index, std::vector<Component*>> m_components;
        ComponentsObservable                                         m_componentsObservable;

        std::unordered_map<std::uint64_t, SpatialCell>               m_cells;
        std::unordered_map<Object*, std::vector<std::uint64_t>>      m_objectCells;
    };
}
