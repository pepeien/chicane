#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Mailbox.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Script/Bus.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/View/Frustum.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"
#include "Chicane/Runtime/Scene/SpatialCell.hpp"
#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Response.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

namespace Chicane
{
    class SceneScript;

    class CHICANE_RUNTIME Scene
    {
        friend Object;

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

        void claim();
        bool ownsObjects() const;
        void runOnOwner(std::function<void()> inWork);

        std::uint64_t subscribe(const String& inName, std::function<void(const String&)> inCallback);
        void unsubscribe(std::uint64_t inToken);
        void send(const String& inName, const String& inData = {});
        void receive(const String& inName, const String& inData);
        void pumpEvents();
        void loadSceneScript(const FileSystem::Path& inTrack);

        void open(const FileSystem::Path& inFilepath);
        void save(const FileSystem::Path& inFilepath) const;
        void save() const;
        const FileSystem::Path& getFilepath() const;
        void setFilepath(const FileSystem::Path& inFilepath);
        void clearSerializable();

        Actor* createActorFromTag(const String& inTypeName);
        Component* createComponentFromTag(const String& inTypeName);
        Actor* adoptActor(Actor* inActor);
        Component* adoptComponent(Component* inComponent);

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

        bool hasActor(const String& inId) const;

        template <class T>
        inline bool hasActor(const String& inId) const
        {
            return getActor<T>(inId) != nullptr;
        }

        Actor* getActor(const String& inId) const;

        template <class T>
        inline T* getActor(const String& inId) const
        {
            return dynamic_cast<T*>(getActor(inId));
        }

        template <class T = Actor, typename... Params>
        inline T* createActor(Params... inParams)
        {
            return static_cast<T*>(adoptActor(new T(inParams...)));
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

        Component* getComponent(const String& inId) const;

        template <class T>
        inline T* getComponent(const String& inId) const
        {
            return dynamic_cast<T*>(getComponent(inId));
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
            return static_cast<T*>(adoptComponent(new T(inParams...)));
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
        inline bool trace(
            SceneTraceRequest& outRequest, const Vec2& inLocation, const Bounds2D& inViewport, const CView* inView
        ) const
        {
            if (inViewport.isEmpty() || !inView)
            {
                return false;
            }

            const Vec2  size(inViewport.right - inViewport.left, inViewport.bottom - inViewport.top);
            const Vec2  local(inLocation.x - inViewport.left, inLocation.y - inViewport.top);

            const View& data = inView->getData();
            Vec3        nearPoint;
            Vec3        farPoint;
            if (!Mat4::sFromPosition(local, data.view, data.projection, size, nearPoint, farPoint))
            {
                return false;
            }

            outRequest = SceneTraceRequest::sLine(nearPoint, farPoint);

            return outRequest.isValid();
        }

        template <typename T = Object>
        inline bool trace(
            SceneTraceResponse&               outResponse,
            const SceneTraceRequest&          inRequest,
            const std::vector<const Object*>& inIgnored = {}
        ) const
        {
            std::vector<SceneTraceResponse> responses;
            if (!traceMulti<T>(responses, inRequest, inIgnored) || responses.empty())
            {
                return false;
            }

            outResponse = responses.front();

            return true;
        }

        template <typename T = Object>
        inline bool trace(
            SceneTraceResponse&               outResponse,
            const Vec2&                       inLocation,
            const Bounds2D&                   inViewport,
            const CView*                      inView,
            const std::vector<const Object*>& inIgnored = {}
        ) const
        {
            std::vector<SceneTraceResponse> responses;
            if (!traceMulti<T>(responses, inLocation, inViewport, inView, inIgnored) || responses.empty())
            {
                return false;
            }

            outResponse = responses.front();

            return true;
        }

        template <typename T = Object>
        inline bool traceMulti(
            std::vector<SceneTraceResponse>&  outResponses,
            const Vec2&                       inLocation,
            const Bounds2D&                   inViewport,
            const CView*                      inView,
            const std::vector<const Object*>& inIgnored = {}
        ) const
        {
            SceneTraceRequest request;
            if (!trace(request, inLocation, inViewport, inView))
            {
                return false;
            }

            return traceMulti<T>(outResponses, request, inIgnored);
        }

        template <typename T = Object>
        inline bool traceMulti(
            std::vector<SceneTraceResponse>&  outResponses,
            const SceneTraceRequest&          inRequest,
            const std::vector<const Object*>& inIgnored = {}
        ) const
        {
            outResponses.clear();
            if (!inRequest.isValid())
            {
                return false;
            }

            std::unordered_set<const Object*> ignored(inIgnored.begin(), inIgnored.end());

            auto                              append = [&](Object* object)
            {
                if (!object || ignored.find(object) != ignored.end())
                {
                    return;
                }

                if (const Component* component = dynamic_cast<const Component*>(object))
                {
                    if (!component->isActive())
                    {
                        return;
                    }
                }

                float enter = 0.0f;
                if (!inRequest.intersects(object->getBounds(), enter))
                {
                    return;
                }

                SceneTraceResponse response;
                SceneTraceShapeUtility::fillResponse(
                    response,
                    inRequest.origin,
                    inRequest.destination,
                    object->getBounds(),
                    enter
                );
                response.object = object;
                outResponses.push_back(response);
            };

            if constexpr (std::is_same_v<T, Object>)
            {
                for (const auto& entry : m_actors)
                {
                    for (Actor* actor : entry.second)
                    {
                        append(actor);
                    }
                }

                for (const auto& entry : m_components)
                {
                    for (Component* component : entry.second)
                    {
                        append(component);
                    }
                }
            }

            if constexpr (!std::is_same_v<T, Object> && std::is_same_v<T, Actor>)
            {
                for (const auto& entry : m_actors)
                {
                    for (Actor* actor : entry.second)
                    {
                        append(actor);
                    }
                }
            }

            if constexpr (!std::is_same_v<T, Object> && !std::is_same_v<T, Actor> && std::is_same_v<T, Component>)
            {
                for (const auto& entry : m_components)
                {
                    for (Component* component : entry.second)
                    {
                        append(component);
                    }
                }
            }

            if constexpr (!std::is_same_v<T, Object> && !std::is_same_v<T, Actor> && !std::is_same_v<T, Component> &&
                          std::is_base_of_v<Actor, T>)
            {
                auto found = m_actors.find(std::type_index(typeid(T)));
                if (found == m_actors.end() || found->second.empty())
                {
                    return false;
                }

                for (Actor* actor : found->second)
                {
                    append(actor);
                }
            }

            if constexpr (!std::is_same_v<T, Object> && !std::is_same_v<T, Actor> && !std::is_same_v<T, Component> &&
                          !std::is_base_of_v<Actor, T> && std::is_base_of_v<Component, T>)
            {
                auto found = m_components.find(std::type_index(typeid(T)));
                if (found == m_components.end() || found->second.empty())
                {
                    return false;
                }

                for (Component* component : found->second)
                {
                    append(component);
                }
            }

            if (outResponses.empty())
            {
                return false;
            }

            std::sort(
                outResponses.begin(),
                outResponses.end(),
                [](const SceneTraceResponse& inLeft, const SceneTraceResponse& inRight)
                { return inLeft.distance < inRight.distance; }
            );

            return true;
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
                    if (!object || !seen.insert(object).second)
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

        void flushSpatial();
        void updateSpatial(Object* inObject);
        void removeSpatial(Object* inObject);

        float getCellSize() const;
        void setCellSize(float inValue);
        std::uint64_t makeCellKey(int inX, int inY, int inZ) const;
        std::uint64_t makeCellKey(const Vec3& inPosition) const;
        void collectCellKeys(const Object* inObject, std::vector<std::uint64_t>& outKeys) const;
        void insertIntoCell(Object* inObject, std::uint64_t inKey);
        void eraseFromCell(Object* inObject, std::uint64_t inKey);

    private:
        void attachObject(Object* inObject, const String& inFallback);
        void assignUniqueId(Object* inObject, const String& inFallback);
        String makeUniqueId(const String& inBase) const;
        void ensureUniqueId(const String& inId, const Object* inIgnored) const;
        void drainMutations();
        void unlinkActor(Actor* inActor, const std::type_index& inType);
        void unlinkComponent(Component* inComponent, const std::type_index& inType);

    private:
        bool                                                         m_bIsLoaded;

        std::size_t                                                  m_actorCount;
        std::unordered_map<std::type_index, std::vector<Actor*>>     m_actors;
        ActorsObservable                                             m_actorsObservable;

        std::size_t                                                  m_componentCount;
        std::unordered_map<std::type_index, std::vector<Component*>> m_components;
        ComponentsObservable                                         m_componentsObservable;

        FileSystem::Path                                             m_filepath;

        float                                                        m_cellSize;
        std::unordered_map<std::uint64_t, SceneSpatialCell>          m_cells;
        std::unordered_map<Object*, std::vector<std::uint64_t>>      m_objectCells;

        std::atomic<std::thread::id>                                 m_owner;
        Mailbox<std::function<void()>>                               m_mutations;

        Script::Bus                                                  m_bus;
        std::unique_ptr<SceneScript>                                 m_sceneScript;
    };
}

#include "Chicane/Runtime/Scene/Object.inl"
