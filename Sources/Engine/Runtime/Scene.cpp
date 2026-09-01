#include "Chicane/Runtime/Scene.hpp"

#include "Chicane/Runtime/Scene/Component/Camera.hpp"

namespace Chicane
{
    Scene::Scene()
        : m_bIsLoaded(false),
          m_actorCount(0),
          m_actors({}),
          m_actorsObservable({}),
          m_componentCount(0),
          m_components({}),
          m_componentsObservable({}),
          m_cells({}),
          m_objectCells({})
    {}

    Scene::~Scene()
    {
        deleteComponents();
        deleteActors();
    }

    void Scene::load()
    {
        onLoad();

        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->onLoad();
            }
        }

        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->onLoad();
            }
        }

        m_bIsLoaded = true;
    }

    void Scene::unload()
    {
        m_bIsLoaded = false;

        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->onUnload();
            }
        }

        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->onUnload();
            }
        }

        onUnload();
    }

    void Scene::tick(float inDeltaTime)
    {
        CCamera* camera = nullptr;

        auto cameras = m_components.find(std::type_index(typeid(CCamera)));
        if (cameras != m_components.end())
        {
            for (Component* component : cameras->second)
            {
                if (!component->isActive())
                {
                    continue;
                }

                camera = static_cast<CCamera*>(component);
            }
        }

        if (!camera)
        {
            tickActors(inDeltaTime);
            tickComponents(inDeltaTime);
            onTick(inDeltaTime);

            return;
        }

        std::vector<Object*> visible;
        forEachInFrustum(
            camera->getFrustum(),
            [&visible](Object* inObject)
            {
                visible.push_back(inObject);
            }
        );

        for (Object* object : visible)
        {
            object->tick(inDeltaTime);
        }

        onTick(inDeltaTime);
    }

    bool Scene::hasActors() const
    {
        return m_actorCount > 0;
    }

    std::vector<Actor*> Scene::getActors() const
    {
        std::vector<Actor*> result;
        result.reserve(m_actorCount);

        for (const auto& [type, actors] : m_actors)
        {
            result.insert(result.end(), actors.begin(), actors.end());
        }

        return result;
    }

    void Scene::removeActor(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        auto typed = m_actors.find(std::type_index(typeid(*inActor)));
        if (typed == m_actors.end())
        {
            return;
        }

        auto found = std::find(typed->second.begin(), typed->second.end(), inActor);
        if (found == typed->second.end())
        {
            return;
        }

        typed->second.erase(found);
        m_actorCount--;

        if (typed->second.empty())
        {
            m_actors.erase(typed);
        }

        if (!m_actorsObservable.isEmpty())
        {
            m_actorsObservable.next(getActors());
        }
    }

    Scene::ActorsSubscription Scene::watchActors(
        ActorsSubscription::NextCallback     inNext,
        ActorsSubscription::ErrorCallback    inError,
        ActorsSubscription::CompleteCallback inComplete
    )
    {
        return m_actorsObservable.subscribe(inNext, inError, inComplete).next(getActors());
    }

    bool Scene::hasComponents() const
    {
        return m_componentCount > 0;
    }

    std::vector<Component*> Scene::getComponents() const
    {
        std::vector<Component*> result;
        result.reserve(m_componentCount);

        for (const auto& [type, components] : m_components)
        {
            result.insert(result.end(), components.begin(), components.end());
        }

        return result;
    }

    void Scene::removeComponent(Component* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        auto typed = m_components.find(std::type_index(typeid(*inComponent)));
        if (typed == m_components.end())
        {
            return;
        }

        auto found = std::find(typed->second.begin(), typed->second.end(), inComponent);
        if (found == typed->second.end())
        {
            return;
        }

        typed->second.erase(found);
        m_componentCount--;

        if (typed->second.empty())
        {
            m_components.erase(typed);
        }

        if (!m_componentsObservable.isEmpty())
        {
            m_componentsObservable.next(getComponents());
        }
    }

    Scene::ComponentsSubscription Scene::watchComponents(
        ComponentsSubscription::NextCallback     inNext,
        ComponentsSubscription::ErrorCallback    inError,
        ComponentsSubscription::CompleteCallback inComplete
    )
    {
        return m_componentsObservable.subscribe(inNext, inError, inComplete).next(getComponents());
    }

    bool Scene::isLoaded() const
    {
        return m_bIsLoaded;
    }

    void Scene::tickActors(float inDeltaTime)
    {
        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->tick(inDeltaTime);
            }
        }
    }

    void Scene::deleteActors()
    {
        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                delete actor;
                actor = nullptr;
            }
        }

        m_actors.clear();
        m_actorCount = 0;
    }

    void Scene::tickComponents(float inDeltaTime)
    {
        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->tick(inDeltaTime);
            }
        }
    }

    void Scene::deleteComponents()
    {
        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                delete component;
                component = nullptr;
            }
        }

        m_components.clear();
        m_componentCount = 0;
    }

    void Scene::updateSpatial(Object* inObject)
    {
        if (!inObject)
        {
            return;
        }

        std::vector<std::uint64_t> keys;
        collectCellKeys(inObject, keys);

        auto found = m_objectCells.find(inObject);
        if (found != m_objectCells.end() && found->second == keys)
        {
            return;
        }

        if (found != m_objectCells.end())
        {
            for (std::uint64_t key : found->second)
            {
                eraseFromCell(inObject, key);
            }

            found->second = keys;
        }
        else
        {
            found = m_objectCells.emplace(inObject, keys).first;
        }

        for (std::uint64_t key : found->second)
        {
            insertIntoCell(inObject, key);
        }
    }

    void Scene::removeSpatial(Object* inObject)
    {
        if (!inObject)
        {
            return;
        }

        auto found = m_objectCells.find(inObject);
        if (found == m_objectCells.end())
        {
            return;
        }

        for (std::uint64_t key : found->second)
        {
            eraseFromCell(inObject, key);
        }

        m_objectCells.erase(found);
    }

    std::uint64_t Scene::makeCellKey(int inX, int inY, int inZ) const
    {
        return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(inX) & 0x1FFFFF) << 42) |
               (static_cast<std::uint64_t>(static_cast<std::uint32_t>(inY) & 0x1FFFFF) << 21) |
               (static_cast<std::uint64_t>(static_cast<std::uint32_t>(inZ) & 0x1FFFFF));
    }

    std::uint64_t Scene::makeCellKey(const Vec3& inPosition) const
    {
        return makeCellKey(
            static_cast<int>(std::floor(inPosition.x / SPATIAL_CELL_SIZE)),
            static_cast<int>(std::floor(inPosition.y / SPATIAL_CELL_SIZE)),
            static_cast<int>(std::floor(inPosition.z / SPATIAL_CELL_SIZE))
        );
    }

    void Scene::collectCellKeys(const Object* inObject, std::vector<std::uint64_t>& outKeys) const
    {
        const Bounds3D& bounds = inObject->getBounds();
        const Vec3&     min    = bounds.getMin().transformed;
        const Vec3&     max    = bounds.getMax().transformed;

        if (min.x > max.x || min.y > max.y || min.z > max.z)
        {
            outKeys.push_back(makeCellKey(inObject->getTranslation()));

            return;
        }

        int minX = static_cast<int>(std::floor(min.x / SPATIAL_CELL_SIZE));
        int minY = static_cast<int>(std::floor(min.y / SPATIAL_CELL_SIZE));
        int minZ = static_cast<int>(std::floor(min.z / SPATIAL_CELL_SIZE));
        int maxX = static_cast<int>(std::floor(max.x / SPATIAL_CELL_SIZE));
        int maxY = static_cast<int>(std::floor(max.y / SPATIAL_CELL_SIZE));
        int maxZ = static_cast<int>(std::floor(max.z / SPATIAL_CELL_SIZE));

        static constexpr int kMaxSpan = 32;
        maxX = std::min(maxX, minX + kMaxSpan);
        maxY = std::min(maxY, minY + kMaxSpan);
        maxZ = std::min(maxZ, minZ + kMaxSpan);

        outKeys.reserve(static_cast<std::size_t>(maxX - minX + 1) * static_cast<std::size_t>(maxY - minY + 1) *
                        static_cast<std::size_t>(maxZ - minZ + 1));

        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                for (int z = minZ; z <= maxZ; z++)
                {
                    outKeys.push_back(makeCellKey(x, y, z));
                }
            }
        }
    }

    void Scene::insertIntoCell(Object* inObject, std::uint64_t inKey)
    {
        SpatialCell& cell = m_cells[inKey];

        if (cell.objects.empty())
        {
            const int x = static_cast<int>((inKey >> 42) & 0x1FFFFF);
            const int y = static_cast<int>((inKey >> 21) & 0x1FFFFF);
            const int z = static_cast<int>(inKey & 0x1FFFFF);

            const auto restore = [](int inValue) -> int
            {
                return inValue >= 0x100000 ? inValue - 0x200000 : inValue;
            };

            const int cellX = restore(x);
            const int cellY = restore(y);
            const int cellZ = restore(z);

            cell.min = Vec3(
                static_cast<float>(cellX) * SPATIAL_CELL_SIZE,
                static_cast<float>(cellY) * SPATIAL_CELL_SIZE,
                static_cast<float>(cellZ) * SPATIAL_CELL_SIZE
            );
            cell.max = Vec3(
                cell.min.x + SPATIAL_CELL_SIZE,
                cell.min.y + SPATIAL_CELL_SIZE,
                cell.min.z + SPATIAL_CELL_SIZE
            );
        }

        cell.objects.push_back(inObject);
    }

    void Scene::eraseFromCell(Object* inObject, std::uint64_t inKey)
    {
        auto found = m_cells.find(inKey);
        if (found == m_cells.end())
        {
            return;
        }

        std::vector<Object*>& objects = found->second.objects;
        auto                  object  = std::find(objects.begin(), objects.end(), inObject);

        if (object != objects.end())
        {
            *object = objects.back();
            objects.pop_back();
        }

        if (objects.empty())
        {
            m_cells.erase(found);
        }
    }
}
