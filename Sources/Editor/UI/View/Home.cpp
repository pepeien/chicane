#include "Editor/UI/View/Home.reflected.hpp"

#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection/Enum/Registry.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>
#include <Chicane/Runtime/Scene/Actor/Light.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn.hpp>
#include <Chicane/Runtime/Scene/Actor/Pawn/Character.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>
#include <Chicane/Runtime/Scene/Actor/Sound.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Light.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/Physics.hpp>
#include <Chicane/Runtime/Scene/Component/Sound.hpp>
#include <Chicane/Runtime/Scene/Component/View.hpp>
#include <Chicane/Runtime/Track.hpp>

#include "Editor/Scene.hpp"
#include "Editor/UI/Component/Asset/Manager.hpp"
#include "Editor/UI/Component/Attributes.hpp"
#include "Editor/UI/Component/Explorer.hpp"
#include "Editor/UI/Component/Header.hpp"
#include "Editor/UI/Component/Outliner.hpp"
#include "Editor/UI/Component/ShowFlags.hpp"
#include "Editor/UI/Component/Toolbar.hpp"
#include "Editor/UI/Component/Telemetry.hpp"

namespace Editor
{
    namespace
    {
        Chicane::String typeTail(const Chicane::String& inName)
        {
            const std::size_t split = inName.lastOf(':');
            if (split == Chicane::String::npos)
            {
                return inName;
            }

            return inName.substr(split + 1);
        }

        const Chicane::ReflectionEnumInfo* findEnum(const Chicane::String& inTypeName)
        {
            Chicane::ReflectionEnumRegistry& registry = Chicane::ReflectionEnumRegistry::getInstance();
            if (const Chicane::ReflectionEnumInfo* found = registry.find(inTypeName))
            {
                return found;
            }

            return registry.find(typeTail(inTypeName));
        }

        Chicane::String formatVec3(const Chicane::Vec3& inValue)
        {
            return Chicane::String::sprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
        }

        bool isTransformField(const Chicane::String& inName)
        {
            return inName.equals("translation", "rotation", "scale");
        }

        std::shared_ptr<Scene> editorScene()
        {
            return Chicane::Application::getInstance().getScene<Scene>();
        }

        static constexpr inline const char* OUTLINER_EXPAND_LEAF      = "leaf";
        static constexpr inline const char* OUTLINER_EXPAND_COLLAPSED = "collapsed";
        static constexpr inline const char* OUTLINER_EXPAND_EXPANDED  = "expanded";

        static constexpr inline const char* OUTLINER_ICON_ACTOR     = "Person";
        static constexpr inline const char* OUTLINER_ICON_PAWN      = "PersonSimpleRun";
        static constexpr inline const char* OUTLINER_ICON_CHARACTER = "PersonSimpleThrow";
        static constexpr inline const char* OUTLINER_ICON_SKY       = "Globe";
        static constexpr inline const char* OUTLINER_ICON_MESH      = "Cube";
        static constexpr inline const char* OUTLINER_ICON_LIGHT     = "Lightbulb";
        static constexpr inline const char* OUTLINER_ICON_SOUND     = "SpeakerHigh";
        static constexpr inline const char* OUTLINER_ICON_VIEW      = "Eye";
        static constexpr inline const char* OUTLINER_ICON_CAMERA    = "Camera";
        static constexpr inline const char* OUTLINER_ICON_PHYSICS   = "Weight";

        Chicane::String outlinerIcon(const Chicane::Object* inObject)
        {
            if (!inObject)
            {
                return OUTLINER_ICON_ACTOR;
            }

            if (dynamic_cast<const Chicane::ACharacter*>(inObject))
            {
                return OUTLINER_ICON_CHARACTER;
            }

            if (dynamic_cast<const Chicane::APawn*>(inObject))
            {
                return OUTLINER_ICON_PAWN;
            }

            if (dynamic_cast<const Chicane::ASky*>(inObject))
            {
                return OUTLINER_ICON_SKY;
            }

            if (dynamic_cast<const Chicane::ACamera*>(inObject))
            {
                return OUTLINER_ICON_CAMERA;
            }

            if (dynamic_cast<const Chicane::ALight*>(inObject))
            {
                return OUTLINER_ICON_LIGHT;
            }

            if (dynamic_cast<const Chicane::ASound*>(inObject))
            {
                return OUTLINER_ICON_SOUND;
            }

            if (dynamic_cast<const Chicane::CMesh*>(inObject))
            {
                return OUTLINER_ICON_MESH;
            }

            if (dynamic_cast<const Chicane::CLight*>(inObject))
            {
                return OUTLINER_ICON_LIGHT;
            }

            if (dynamic_cast<const Chicane::CSound*>(inObject))
            {
                return OUTLINER_ICON_SOUND;
            }

            if (dynamic_cast<const Chicane::CCamera*>(inObject))
            {
                return OUTLINER_ICON_CAMERA;
            }

            if (dynamic_cast<const Chicane::CPhysics*>(inObject))
            {
                return OUTLINER_ICON_PHYSICS;
            }

            if (dynamic_cast<const Chicane::CView*>(inObject))
            {
                return OUTLINER_ICON_VIEW;
            }

            if (dynamic_cast<const Chicane::Component*>(inObject))
            {
                return OUTLINER_ICON_MESH;
            }

            return OUTLINER_ICON_ACTOR;
        }

        bool hasOutlinerChildren(const Chicane::Object* inObject)
        {
            if (!inObject)
            {
                return false;
            }

            for (Chicane::Component* attachment : inObject->getAttachments())
            {
                if (attachment && !attachment->isTransient())
                {
                    return true;
                }
            }

            return false;
        }

        void commitAttributeField(Chicane::Object& inItem, AttributeField& inField)
        {
            Chicane::String value = inField.text;
            if (inField.type == AttributeFieldType::Bool)
            {
                value = inField.bIsChecked ? "true" : "false";
            }

            Chicane::Track::applyField(inItem, inField.name, value);
        }

        void syncAttributeField(
            Chicane::Object& inItem, const Chicane::ReflectionTypeInfo& inType, AttributeField& ioField
        )
        {
            const Chicane::ReflectionFieldAccessor accessor = inType.resolve(ioField.name);
            if (!accessor.isValid())
            {
                return;
            }

            if (ioField.type == AttributeFieldType::Bool)
            {
                const bool* value  = accessor.getValue<bool>(&inItem);
                ioField.bIsChecked = value && *value;

                return;
            }

            if (ioField.type == AttributeFieldType::Vec3)
            {
                if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem))
                {
                    ioField.vector = *value;
                    ioField.text   = formatVec3(*value);
                }

                return;
            }

            if (accessor.isType<Chicane::FileSystem::Path>())
            {
                const Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(&inItem);
                ioField.text                          = path ? path->toString() : Chicane::String::empty();

                return;
            }

            if (ioField.type == AttributeFieldType::Text || ioField.type == AttributeFieldType::Float ||
                ioField.type == AttributeFieldType::Enum)
            {
                ioField.text = accessor.toString(&inItem);
            }
        }
    }

    HomeView::HomeView()
        : Chicane::Grid::View(),
          outlinerNodes({}),
          attributeFields({}),
          attributeGroups({}),
          bIsItemSelected(false),
          selectedItem(nullptr),
          theme("dark"),
          workspace(WORKSPACE_VIEWPORT),
          bIsViewportWorkspace(true),
          bIsAssetsWorkspace(false),
          viewportTabState(STATE_ACTIVE),
          assetsTabState(STATE_IDLE),
          translateState(STATE_ACTIVE),
          rotateState(STATE_IDLE),
          scaleState(STATE_IDLE),
          m_collapsedOutlinerItems({})
    {
        import <AssetManager>();
        import <Attributes>();
        import <Explorer>();
        import <Header>();
        import <Outliner>();
        import <ShowFlags>();
        import <Telemetry>();
        import <Toolbar>();

        load("Assets/Editor/UI/Views/Home.grid", "Assets/Editor/UI/Views/Home.decal");

        bindScene();
    }

    void HomeView::onTick(float inDeltaTime)
    {
        Chicane::Grid::View::onTick(inDeltaTime);

        if (std::shared_ptr<Scene> scene = editorScene())
        {
            if (Gizmo* gizmo = scene->getGizmo())
            {
                translateState = gizmo->getType() == GizmoType::Translation ? STATE_ACTIVE : STATE_IDLE;
                rotateState    = gizmo->getType() == GizmoType::Rotation ? STATE_ACTIVE : STATE_IDLE;
                scaleState     = gizmo->getType() == GizmoType::Scale ? STATE_ACTIVE : STATE_IDLE;
            }
        }
    }

    void HomeView::bindScene()
    {
        Chicane::Application::getInstance().watchScene(
            [&](std::shared_ptr<Chicane::Scene> inScene)
            {
                if (!inScene)
                {
                    outlinerNodes.clear();
                    m_collapsedOutlinerItems.clear();
                    onItemSelection(nullptr);

                    return;
                }

                inScene->watchActors([&](std::vector<Chicane::Actor*>) { rebuildOutliner(); });

                inScene->watchComponents([&](std::vector<Chicane::Component*>) { rebuildOutliner(); });
            }
        );
    }

    void HomeView::onAssetImport()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.location       = "/";
        dialog.title          = "Select mesh";
        dialog.addFilter("Meshes", {Chicane::Box::Mesh::EXTENSION});

        dialog.open(
            [](const Chicane::FileSystem::Item::List& inFiles)
            {
                std::shared_ptr<Scene> scene = editorScene();
                if (!scene)
                {
                    return;
                }

                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    if (item.type != Chicane::FileSystem::ItemType::File)
                    {
                        continue;
                    }

                    if (item.extension.equals(Chicane::Box::Mesh::EXTENSION))
                    {
                        scene->spawnMeshActor(item.path);
                    }
                }
            }
        );
    }

    void HomeView::onThemeSwitch(Chicane::String inValue)
    {
        theme = inValue;
    }

    void HomeView::onItemSelection(Chicane::Object* inItem)
    {
        selectedItem    = inItem;
        bIsItemSelected = selectedItem != nullptr;

        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->setSelection(selectedItem);
        }

        expandOutlinerAncestors(selectedItem);
        rebuildOutliner();
        rebuildAttributes();
    }

    void HomeView::onItemToggle(Chicane::Object* inItem)
    {
        if (!inItem || !hasOutlinerChildren(inItem))
        {
            return;
        }

        if (m_collapsedOutlinerItems.erase(inItem) == 0)
        {
            m_collapsedOutlinerItems.insert(inItem);
        }

        rebuildOutliner();
    }

    void HomeView::onWorkspaceViewport()
    {
        setWorkspace(WORKSPACE_VIEWPORT);
    }

    void HomeView::onWorkspaceAssets()
    {
        setWorkspace(WORKSPACE_ASSETS);
    }

    void HomeView::setWorkspace(const Chicane::String& inValue)
    {
        workspace            = inValue;
        bIsViewportWorkspace = workspace.equals(WORKSPACE_VIEWPORT);
        bIsAssetsWorkspace   = workspace.equals(WORKSPACE_ASSETS);
        viewportTabState     = bIsViewportWorkspace ? STATE_ACTIVE : STATE_IDLE;
        assetsTabState       = bIsAssetsWorkspace ? STATE_ACTIVE : STATE_IDLE;
    }

    void HomeView::onTrackNew()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->open({});
            onItemSelection(nullptr);
        }
    }

    void HomeView::onTrackOpen()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.title          = "Open Track";
        dialog.addFilter("Tracks", {Chicane::Track::EXTENSION});

        dialog.open(
            [this](const Chicane::FileSystem::Item::List& inFiles)
            {
                std::shared_ptr<Scene> scene = editorScene();
                if (!scene)
                {
                    return;
                }

                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    if (item.type != Chicane::FileSystem::ItemType::File)
                    {
                        continue;
                    }

                    scene->open(item.path);
                    onItemSelection(nullptr);

                    return;
                }
            }
        );
    }

    void HomeView::onTrackSave()
    {
        std::shared_ptr<Scene> scene = editorScene();
        if (!scene)
        {
            return;
        }

        if (scene->getFilepath().isEmpty())
        {
            onTrackSaveAs();

            return;
        }

        scene->save();
    }

    void HomeView::onTrackSaveAs()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.title          = "Save Track";
        dialog.addFilter("Tracks", {Chicane::Track::EXTENSION});

        dialog.open(
            [](const Chicane::FileSystem::Item::List& inFiles)
            {
                std::shared_ptr<Scene> scene = editorScene();
                if (!scene)
                {
                    return;
                }

                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    Chicane::FileSystem::Path path = item.path;
                    if (!path.extension().toString().equals(Chicane::Track::EXTENSION))
                    {
                        path = path.withExtension(Chicane::Track::EXTENSION);
                    }

                    scene->save(path);
                    scene->setFilepath(path);

                    return;
                }
            }
        );
    }

    void HomeView::onAttributeCommit(Chicane::String inName)
    {
        if (!selectedItem || inName.isEmpty())
        {
            return;
        }

        for (AttributeField& field : attributeFields)
        {
            if (!field.name.equals(inName))
            {
                continue;
            }

            commitAttributeField(*selectedItem, field);

            return;
        }

        for (AttributeGroup& group : attributeGroups)
        {
            for (AttributeField& field : group.fields)
            {
                if (!field.name.equals(inName))
                {
                    continue;
                }

                commitAttributeField(*selectedItem, field);

                return;
            }
        }
    }

    void HomeView::onSpawnActor()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            onItemSelection(scene->createActor<Chicane::Actor>());
        }
    }

    void HomeView::onSpawnMesh()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            onItemSelection(scene->spawnMeshActor(Chicane::Box::Mesh::DEFAULT_SOURCE));
        }
    }

    void HomeView::onGizmoTranslate()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->setGizmoType(GizmoType::Translation);
        }
    }

    void HomeView::onGizmoRotate()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->setGizmoType(GizmoType::Rotation);
        }
    }

    void HomeView::onGizmoScale()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->setGizmoType(GizmoType::Scale);
        }
    }

    void HomeView::rebuildOutliner()
    {
        outlinerNodes.clear();

        std::shared_ptr<Scene> scene = editorScene();
        if (!scene)
        {
            m_collapsedOutlinerItems.clear();

            return;
        }

        std::unordered_set<Chicane::Object*> live;
        for (Chicane::Actor* actor : scene->getActors())
        {
            if (!actor || actor->isTransient())
            {
                continue;
            }

            appendOutlinerNode(actor, 0, true, live);
        }

        for (auto it = m_collapsedOutlinerItems.begin(); it != m_collapsedOutlinerItems.end();)
        {
            if (live.find(*it) == live.end())
            {
                it = m_collapsedOutlinerItems.erase(it);

                continue;
            }

            ++it;
        }

        if (selectedItem && live.find(selectedItem) == live.end())
        {
            selectedItem    = nullptr;
            bIsItemSelected = false;
            scene->setSelection(nullptr);
            rebuildAttributes();
        }
    }

    void HomeView::appendOutlinerNode(
        Chicane::Object* inObject, int inDepth, bool inIsVisible, std::unordered_set<Chicane::Object*>& outLive
    )
    {
        if (!inObject)
        {
            return;
        }

        outLive.insert(inObject);

        const bool bHasChildren = hasOutlinerChildren(inObject);
        const bool bIsCollapsed = m_collapsedOutlinerItems.find(inObject) != m_collapsedOutlinerItems.end();

        if (inIsVisible)
        {
            OutlinerNode node;
            node.item          = inObject;
            node.label         = inObject->getId();
            node.icon          = outlinerIcon(inObject);
            node.indent        = Chicane::String::sprint("%.2fem", static_cast<float>(inDepth) * 0.85f);
            node.expandState   = !bHasChildren  ? OUTLINER_EXPAND_LEAF
                                 : bIsCollapsed ? OUTLINER_EXPAND_COLLAPSED
                                                : OUTLINER_EXPAND_EXPANDED;
            node.selectedState = inObject == selectedItem ? "selected" : "idle";
            node.bHasChildren  = bHasChildren;
            node.bIsLeaf       = !bHasChildren;
            outlinerNodes.push_back(node);
        }

        for (Chicane::Component* attachment : inObject->getAttachments())
        {
            if (!attachment || attachment->isTransient())
            {
                continue;
            }

            appendOutlinerNode(attachment, inDepth + 1, inIsVisible && !bIsCollapsed, outLive);
        }
    }

    void HomeView::expandOutlinerAncestors(Chicane::Object* inItem)
    {
        Chicane::Object* current = inItem;
        while (current)
        {
            if (Chicane::Component* component = dynamic_cast<Chicane::Component*>(current))
            {
                current = component->getParent();
                if (current)
                {
                    m_collapsedOutlinerItems.erase(current);
                }

                continue;
            }

            break;
        }
    }

    void HomeView::rebuildAttributes()
    {
        attributeFields.clear();
        attributeGroups.clear();

        if (!selectedItem)
        {
            return;
        }

        const Chicane::ReflectionTypeInfo* type =
            Chicane::ReflectionTypeRegistry::getInstance().find(typeid(*selectedItem));
        if (!type)
        {
            return;
        }

        for (const Chicane::ReflectionFieldInfo& info : type->fields)
        {
            if (info.names.empty() || info.bIsPointer || info.bIsIterable)
            {
                continue;
            }

            const Chicane::String name = info.names.front();
            if (isTransformField(name))
            {
                continue;
            }

            const Chicane::ReflectionFieldAccessor accessor = type->resolve(name);
            if (!accessor.isValid())
            {
                continue;
            }

            AttributeField field;
            field.name  = name;
            field.label = name;
            field.group = info.group;
            field.type  = AttributeFieldType::Text;

            if (const Chicane::ReflectionEnumInfo* enumeration = findEnum(accessor.typeName))
            {
                field.type = AttributeFieldType::Enum;
                field.text = accessor.toString(selectedItem);

                for (const Chicane::ReflectionEnumeratorInfo& enumerator : enumeration->enumerators)
                {
                    field.options.push_back(typeTail(enumerator.name));
                    if (enumerator.value == 0 && field.text.isEmpty())
                    {
                        field.text = typeTail(enumerator.name);
                    }
                }
            }
            else if (accessor.isType<bool>())
            {
                field.type        = AttributeFieldType::Bool;
                const bool* value = accessor.getValue<bool>(selectedItem);
                field.bIsChecked  = value && *value;
            }
            else if (accessor.isType<Chicane::Vec3>())
            {
                field.type = AttributeFieldType::Vec3;
                if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(selectedItem))
                {
                    field.vector = *value;
                    field.text   = formatVec3(*value);
                }
            }
            else if (accessor.isType<float>())
            {
                field.type = AttributeFieldType::Float;
                field.text = accessor.toString(selectedItem);
            }
            else if (accessor.isType<Chicane::String>() || accessor.isType<Chicane::FileSystem::Path>() ||
                     accessor.isType<int>())
            {
                field.type = AttributeFieldType::Text;
                if (accessor.isType<Chicane::FileSystem::Path>())
                {
                    const Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(selectedItem);
                    field.text                            = path ? path->toString() : Chicane::String::empty();
                }
                else
                {
                    field.text = accessor.toString(selectedItem);
                }
            }
            else
            {
                continue;
            }

            if (field.group.isEmpty())
            {
                attributeFields.push_back(field);

                continue;
            }

            AttributeGroup* group = nullptr;
            for (AttributeGroup& candidate : attributeGroups)
            {
                if (!candidate.label.equals(field.group))
                {
                    continue;
                }

                group = &candidate;

                break;
            }

            if (!group)
            {
                attributeGroups.push_back({});
                group        = &attributeGroups.back();
                group->label = field.group;
            }

            group->fields.push_back(field);
        }
    }

    void HomeView::syncAttributeValues()
    {
        if (!selectedItem)
        {
            return;
        }

        const Chicane::ReflectionTypeInfo* type =
            Chicane::ReflectionTypeRegistry::getInstance().find(typeid(*selectedItem));
        if (!type)
        {
            return;
        }

        for (AttributeField& field : attributeFields)
        {
            syncAttributeField(*selectedItem, *type, field);
        }

        for (AttributeGroup& group : attributeGroups)
        {
            for (AttributeField& field : group.fields)
            {
                syncAttributeField(*selectedItem, *type, field);
            }
        }
    }

    bool HomeView::hasSelectedItem() const
    {
        return selectedItem != nullptr;
    }
}
