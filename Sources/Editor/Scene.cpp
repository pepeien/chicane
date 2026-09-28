#include "Editor/Scene.hpp"

#include <cmath>
#include <utility>

#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Math.hpp>
#include <Chicane/Renderer/Debug.hpp>
#include <Chicane/Renderer/Feature.hpp>
#include <Chicane/Renderer/Instance.hpp>
#include <Chicane/Renderer/Light/Type.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Light.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Trace/Request.hpp>
#include <Chicane/Runtime/Scene/Trace/Shape/Cone.hpp>
#include <Chicane/Runtime/Scene/Trace/Shape/Pyramid.hpp>
#include <Chicane/Runtime/Scene/Trace/Shape/Utility.hpp>
#include <Chicane/Runtime/Track.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Actor/Studio.hpp"
#include "Editor/Viewport/Overlay.hpp"

namespace Editor
{
    static constexpr inline const char* CAMERA_HELPER_MESH = "Assets/Editor/Meshes/Camera.bmsh";
    static constexpr inline const char* LIGHT_HELPER_MESH  = "Assets/Editor/Meshes/Gizmo/Arrow.bmsh";
    static constexpr inline float       LIGHT_HELPER_SCALE = 0.125f;

    Scene::Scene()
        : Chicane::Scene(),
          m_gizmo(nullptr),
          m_selected(nullptr),
          m_helperSubscription({}),
          m_helpers({}),
          m_bSyncingHelpers(false)
    {}

    Scene::~Scene()
    {
        m_helperSubscription.complete();
        m_helpers.clear();
        Chicane::Renderer::Debug::push({}, -1.0f);
    }

    void Scene::onLoad()
    {
        spawnHelpers();
        spawnLights();
        spawnCharacter();
        spawnGizmo();
        open(DEFAULT_TRACK);
    }

    void Scene::onTick(float inDeltaTime)
    {
        (void)inDeltaTime;

        pushLightTraces();
    }

    void Scene::destroyObject(Chicane::Object* inObject)
    {
        if (!inObject || inObject->isTransient())
        {
            return;
        }

        if (m_selected)
        {
            for (Chicane::Object* node = m_selected; node != nullptr; node = node->getParent())
            {
                if (node == inObject)
                {
                    setSelection(nullptr);
                    break;
                }
            }
        }

        destroyObjectTree(inObject);
    }

    void Scene::setSelection(Chicane::Object* inItem)
    {
        if (m_selected == inItem)
        {
            return;
        }

        m_selected = inItem;
        syncHelpers();

        for (Chicane::CMesh* mesh : getComponents<Chicane::CMesh>())
        {
            bool bSelected = inItem != nullptr && (mesh == inItem || mesh->getParent() == inItem);
            if (!bSelected && inItem)
            {
                for (auto& [target, helper] : m_helpers)
                {
                    if (helper.mesh != mesh)
                    {
                        continue;
                    }

                    bSelected = helperBelongsTo(target, inItem);

                    break;
                }
            }

            mesh->setIsOutlined(bSelected);
        }

        if (!m_gizmo)
        {
            return;
        }

        m_gizmo->setTarget(inItem);
    }

    Gizmo* Scene::getGizmo() const
    {
        return m_gizmo;
    }

    void Scene::setGizmoType(GizmoType inType)
    {
        if (!m_gizmo)
        {
            return;
        }

        m_gizmo->setType(inType);
    }

    Chicane::Actor* Scene::spawnMeshActor(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::Actor* actor = createActor<Chicane::Actor>();
        Chicane::CMesh* mesh  = createComponent<Chicane::CMesh>();
        mesh->setMesh(inMesh.isEmpty() ? Chicane::Box::Mesh::DEFAULT_SOURCE : inMesh);
        mesh->attachTo(actor);
        mesh->activate();

        return actor;
    }

    void Scene::spawnLights()
    {
        Studio* studio = createActor<Studio>();
        studio->setIsTransient(true);
    }

    void Scene::spawnCharacter()
    {
        Character* character = createActor<Character>();
        character->setIsTransient(true);

        Chicane::Controller* controller = Chicane::Instance::sInstance().getController();
        if (!controller || controller->isAttached())
        {
            return;
        }

        controller->attachTo(character);
    }

    void Scene::spawnGizmo()
    {
        m_gizmo = createComponent<Gizmo>(GizmoType::Translation);
        m_gizmo->setIsTransient(true);
    }

    void Scene::spawnHelpers()
    {
        m_helperSubscription.complete();
        m_helperSubscription = watchComponents([this](std::vector<Chicane::Component*>) { syncHelpers(); });
    }

    void Scene::destroyObjectTree(Chicane::Object* inObject)
    {
        if (!inObject)
        {
            return;
        }

        const std::vector<Chicane::Object*> attachments = inObject->getAttachments();
        for (Chicane::Object* child : attachments)
        {
            destroyObjectTree(child);
        }

        inObject->detach();

        if (Chicane::Actor* actor = dynamic_cast<Chicane::Actor*>(inObject))
        {
            removeActor(actor);
            delete actor;

            return;
        }

        if (Chicane::Component* component = dynamic_cast<Chicane::Component*>(inObject))
        {
            removeComponent(component);
            delete component;
        }
    }

    void Scene::syncHelpers()
    {
        if (m_bSyncingHelpers)
        {
            return;
        }

        m_bSyncingHelpers = true;

        std::unordered_map<Chicane::Object*, SceneHelper> next;

        auto keep = [this, &next](Chicane::Object* inTarget, const Chicane::FileSystem::Path& inMesh)
        {
            auto found = m_helpers.find(inTarget);
            if (found != m_helpers.end() && found->second.mesh)
            {
                if (found->second.mesh->mesh != inMesh)
                {
                    found->second.mesh->setMesh(inMesh);
                }

                poseHelper(found->second.mesh, inTarget);
                next[inTarget] = std::move(found->second);
                m_helpers.erase(found);

                return;
            }

            SceneHelper helper;
            helper.mesh         = createHelper(inMesh);
            helper.subscription = inTarget->watchChanges([this, inTarget]() { poseHelper(inTarget); });
            poseHelper(helper.mesh, inTarget);
            next[inTarget] = std::move(helper);
        };

        for (Chicane::CCamera* camera : getComponents<Chicane::CCamera>())
        {
            if (!shouldVisualize(camera))
            {
                continue;
            }

            keep(camera, CAMERA_HELPER_MESH);
        }

        for (Chicane::CLight* light : getComponents<Chicane::CLight>())
        {
            if (!shouldVisualize(light) || !isSelectedVisual(light))
            {
                continue;
            }

            keep(light, LIGHT_HELPER_MESH);
        }

        for (auto& [target, helper] : m_helpers)
        {
            (void)target;

            helper.subscription.complete();
            if (!helper.mesh)
            {
                continue;
            }

            helper.mesh->detach();
            removeComponent(helper.mesh);
            delete helper.mesh;
        }

        m_helpers         = std::move(next);
        m_bSyncingHelpers = false;
    }

    void Scene::poseHelper(Chicane::Object* inTarget)
    {
        auto found = m_helpers.find(inTarget);
        if (found == m_helpers.end())
        {
            return;
        }

        poseHelper(found->second.mesh, inTarget);
    }

    void Scene::poseHelper(Chicane::CMesh* inMesh, Chicane::Object* inTarget)
    {
        if (!inMesh || !inTarget)
        {
            return;
        }

        inMesh->setAbsoluteTranslation(inTarget->getTranslation());
        inMesh->setAbsoluteRotation(inTarget->getRotation());

        const float scale = dynamic_cast<Chicane::CLight*>(inTarget) ? LIGHT_HELPER_SCALE : 1.0f;
        inMesh->setAbsoluteScale(Chicane::Vec3(scale));
    }

    Chicane::CMesh* Scene::createHelper(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::CMesh* mesh = createComponent<Chicane::CMesh>();
        mesh->setCanCastShadows(false);
        mesh->setIsLit(false);
        mesh->setIsForeground(true);
        mesh->setIsTransient(true);
        mesh->setMesh(inMesh);
        mesh->activate();

        return mesh;
    }

    void Scene::pushLightTraces()
    {
        Chicane::Vertex::List vertices;

        Chicane::Renderer::Instance* renderer = Chicane::Instance::sInstance().getRenderer();
        if (renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Traces))
        {
            const Chicane::Vec4 color = ViewportOverlay::sInstance().tracerColor;

            for (Chicane::CCamera* camera : getComponents<Chicane::CCamera>())
            {
                if (!shouldVisualize(camera) || !isSelectedVisual(camera))
                {
                    continue;
                }

                Chicane::Vec3 forward = camera->getForward();
                const float   length  = std::sqrt(forward.dot(forward));
                if (length <= 1e-8f)
                {
                    continue;
                }

                const float         depth       = std::max(camera->getFarClip(), 1e-8f);
                const Chicane::Vec3 origin      = camera->getTranslation();
                const Chicane::Vec3 destination = origin + (forward / length) * depth;
                float               aspect      = camera->getAspectRatio();
                if (aspect <= 1e-4f)
                {
                    aspect = 1.0f;
                }

                const float halfVertical =
                    std::tan(camera->getFieldOfView() * 0.5f * Chicane::Math::DEG_TO_RAD) * depth;
                const Chicane::SceneTraceRequest request = Chicane::SceneTraceRequest::sPyramid(
                    origin,
                    destination,
                    Chicane::Vec2(halfVertical * aspect, halfVertical)
                );
                const Chicane::SceneTraceShapePyramid* pyramid =
                    dynamic_cast<const Chicane::SceneTraceShapePyramid*>(request.shape.get());
                if (!pyramid)
                {
                    continue;
                }

                Chicane::Renderer::Debug::appendPyramid(
                    vertices,
                    origin,
                    destination,
                    pyramid->getHalfExtentsAt(1.0f),
                    color
                );
            }

            for (Chicane::CLight* light : getComponents<Chicane::CLight>())
            {
                if (!shouldVisualize(light) || !isSelectedVisual(light))
                {
                    continue;
                }

                const Chicane::LightType type = light->getType();
                if (type != Chicane::LightType::Directional && type != Chicane::LightType::Spot)
                {
                    continue;
                }

                Chicane::Vec3 forward = light->getForward();
                const float   length  = std::sqrt(forward.dot(forward));
                if (length <= 1e-8f)
                {
                    continue;
                }

                const Chicane::Vec3 origin      = light->getTranslation();
                const Chicane::Vec3 destination = origin + (forward / length) * std::max(light->getRange(), 1e-8f);
                const float         angle       = std::max(light->getOuterAngle(), 0.0f) * Chicane::Math::DEG_TO_RAD;

                const Chicane::SceneTraceRequest request =
                    Chicane::SceneTraceRequest::sCone(origin, destination, angle);
                const Chicane::SceneTraceShapeCone* cone =
                    dynamic_cast<const Chicane::SceneTraceShapeCone*>(request.shape.get());
                if (!cone)
                {
                    continue;
                }

                const float axis = Chicane::SceneTraceShapeUtility::axisLength(origin, destination);
                Chicane::Renderer::Debug::appendRadial(
                    vertices,
                    origin,
                    destination,
                    cone->getRadiusAt(0.0f, axis),
                    cone->getRadiusAt(1.0f, axis),
                    color,
                    cone->segmentCount
                );
            }
        }

        Chicane::Renderer::Debug::push(vertices, -1.0f);
    }

    bool Scene::shouldVisualize(const Chicane::Component* inComponent) const
    {
        if (!inComponent || inComponent->isTransient())
        {
            return false;
        }

        if (Chicane::Object* parent = inComponent->getParent())
        {
            return !parent->isTransient();
        }

        return true;
    }

    bool Scene::isSelectedVisual(const Chicane::Object* inTarget) const
    {
        return helperBelongsTo(inTarget, m_selected);
    }

    bool Scene::helperBelongsTo(const Chicane::Object* inTarget, const Chicane::Object* inItem) const
    {
        if (!inTarget || !inItem)
        {
            return false;
        }

        if (inTarget == inItem)
        {
            return true;
        }

        const Chicane::Component* component = dynamic_cast<const Chicane::Component*>(inTarget);
        while (component)
        {
            const Chicane::Object* parent = component->getParent();
            if (parent == inItem)
            {
                return true;
            }

            component = dynamic_cast<const Chicane::Component*>(parent);
        }

        return false;
    }
}
