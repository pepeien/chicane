#include "Editor/UI/View/Home.reflected.hpp"

#include <cstdlib>
#include <exception>

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Color.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection/Enum/Registry.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Core/Reflection/Type/Info.hpp>
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
#include "Editor/Application.hpp"
#include "Editor/Viewer/Scene.hpp"
#include "Editor/UI/Component/Asset/Manager.hpp"
#include "Editor/UI/Component/Attributes.hpp"
#include "Editor/UI/Component/Explorer.hpp"
#include "Editor/UI/Component/Header.hpp"
#include "Editor/UI/Component/Outliner.hpp"
#include "Editor/UI/Component/Toolbar.hpp"
#include "Editor/UI/Component/Telemetry.hpp"

namespace Editor
{
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
    static constexpr inline const char* OUTLINER_ICON_PHYSICS   = "Bone";

    static constexpr inline const char* TRANSFORM_GROUP_LABEL  = "Transform";
    static constexpr inline const char* COORDINATE_SPACE_NAME  = "coordinateSpace";
    static constexpr inline const char* COORDINATE_SPACE_LABEL = "Coordinate Space";

    static Chicane::String typeTail(const Chicane::String& inName)
    {
        const std::size_t split = inName.lastOf(':');
        if (split == Chicane::String::npos)
        {
            return inName;
        }

        return inName.substr(split + 1);
    }

    static Chicane::String typeGroupLabel(const Chicane::String& inGroup)
    {
        if (inGroup.isEmpty())
        {
            return inGroup;
        }

        const std::vector<Chicane::String> parts = inGroup.split(" | ");
        if (parts.empty())
        {
            return inGroup;
        }

        return parts.back().trim();
    }

    static const Chicane::ReflectionEnumInfo* findEnum(const Chicane::String& inTypeName)
    {
        Chicane::ReflectionEnumRegistry& registry = Chicane::ReflectionEnumRegistry::getInstance();
        if (const Chicane::ReflectionEnumInfo* found = registry.find(inTypeName))
        {
            return found;
        }

        return registry.find(typeTail(inTypeName));
    }

    static Chicane::String formatVec3(const Chicane::Vec3& inValue)
    {
        return Chicane::String::sprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
    }

    static Chicane::Vec3 parseVec3(const Chicane::String& inValue, const Chicane::Vec3& inFallback)
    {
        Chicane::String raw = inValue.trim();
        if (raw.startsWith("["))
        {
            raw = raw.substr(1);
        }

        if (raw.endsWith("]"))
        {
            raw = raw.substr(0, raw.size() - 1);
        }

        const std::vector<Chicane::String> parts = raw.split(',');
        if (parts.size() < 3)
        {
            return inFallback;
        }

        try
        {
            return Chicane::Vec3(
                std::stof(parts.at(0).trim().toStandard()),
                std::stof(parts.at(1).trim().toStandard()),
                std::stof(parts.at(2).trim().toStandard())
            );
        }
        catch (const std::exception&)
        {
            return inFallback;
        }
    }

    static bool isCompleteFloat(const Chicane::String& inValue)
    {
        const Chicane::String trimmed = inValue.trim();
        if (trimmed.isEmpty() || trimmed.equals("-", ".", "-."))
        {
            return false;
        }

        char* end = nullptr;
        std::strtof(trimmed.toChar(), &end);

        return end && end != trimmed.toChar() && *end == '\0';
    }

    static std::shared_ptr<Scene> editorScene()
    {
        return Application::getInstance().getHomeScene();
    }

    static std::shared_ptr<Scene> workspaceScene(bool bIsAssetsWorkspace)
    {
        if (bIsAssetsWorkspace)
        {
            return Application::getInstance().getViewerScene();
        }

        return Application::getInstance().getHomeScene();
    }

    static Chicane::String outlinerIcon(const Chicane::Object* inObject)
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

    static bool hasOutlinerChildren(const Chicane::Object* inObject)
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

    static bool isCoordinateSpaceAttribute(const Chicane::String& inName)
    {
        return inName.equals(COORDINATE_SPACE_NAME);
    }

    static bool isTransformAttribute(const Chicane::String& inName)
    {
        return inName.equals("translation", "rotation", "scale");
    }

    static void assignTransformAttribute(Chicane::Object& inItem, AttributeField& ioField, CoordinateSpace inSpace)
    {
        const bool bIsRelative = inSpace == CoordinateSpace::Relative;
        if (ioField.name.equals("translation"))
        {
            ioField.vector = bIsRelative ? inItem.getRelativeTranslation() : inItem.getAbsoluteTranslation();
        }
        else if (ioField.name.equals("rotation"))
        {
            ioField.vector =
                bIsRelative ? inItem.getRelativeRotation().getAngles() : inItem.getAbsoluteRotation().getAngles();
        }
        else if (ioField.name.equals("scale"))
        {
            ioField.vector = bIsRelative ? inItem.getRelativeScale() : inItem.getAbsoluteScale();
        }

        ioField.text = formatVec3(ioField.vector);
    }

    static void applyTransformAttribute(Chicane::Object& inItem, const AttributeField& inField, CoordinateSpace inSpace)
    {
        const bool bIsRelative = inSpace == CoordinateSpace::Relative;
        if (inField.name.equals("translation"))
        {
            if (bIsRelative)
            {
                inItem.setRelativeTranslation(inField.vector);
            }
            else
            {
                inItem.setAbsoluteTranslation(inField.vector);
            }
        }
        else if (inField.name.equals("rotation"))
        {
            if (bIsRelative)
            {
                inItem.setRelativeRotation(inField.vector);
            }
            else
            {
                inItem.setAbsoluteRotation(inField.vector);
            }
        }
        else if (inField.name.equals("scale"))
        {
            if (bIsRelative)
            {
                inItem.setRelativeScale(inField.vector);
            }
            else
            {
                inItem.setAbsoluteScale(inField.vector);
            }
        }

        inItem.notifyPropertyEdited(inField.name);
    }

    static void insertCoordinateSpaceField(AttributeGroup::List& ioGroups, CoordinateSpace inSpace)
    {
        for (AttributeGroup& group : ioGroups)
        {
            if (!group.label.equals(TRANSFORM_GROUP_LABEL))
            {
                continue;
            }

            AttributeField field;
            field.name    = COORDINATE_SPACE_NAME;
            field.label   = COORDINATE_SPACE_LABEL;
            field.group   = group.label;
            field.type    = AttributeFieldType::Enum;
            field.text    = toString(inSpace);
            field.options = {"Absolute", "Relative"};
            group.fields.insert(group.fields.begin(), field);

            return;
        }
    }

    static void commitAttributeField(Chicane::Object& inItem, AttributeField& inField, CoordinateSpace& ioSpace)
    {
        if (isCoordinateSpaceAttribute(inField.name))
        {
            ioSpace = parseCoordinateSpace(inField.text);

            return;
        }

        if (isTransformAttribute(inField.name))
        {
            applyTransformAttribute(inItem, inField, ioSpace);

            return;
        }

        Chicane::String value = inField.text;
        if (inField.type == AttributeFieldType::Bool)
        {
            value = inField.bIsChecked ? "true" : "false";
        }
        else if (inField.type == AttributeFieldType::Vec3 || inField.type == AttributeFieldType::Color)
        {
            value = formatVec3(inField.vector);
        }

        Chicane::Track::applyField(inItem, inField.name, value);
    }

    static void syncAttributeField(
        Chicane::Object&                   inItem,
        const Chicane::ReflectionTypeInfo& inType,
        AttributeField&                    ioField,
        CoordinateSpace                    inSpace
    )
    {
        if (isCoordinateSpaceAttribute(ioField.name))
        {
            ioField.text = toString(inSpace);

            return;
        }

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
            if (isTransformAttribute(ioField.name))
            {
                assignTransformAttribute(inItem, ioField, inSpace);

                return;
            }

            if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem))
            {
                ioField.vector = *value;
                ioField.text   = formatVec3(*value);
            }
            else if (const Chicane::Rotator* rotator = accessor.getValue<Chicane::Rotator>(&inItem))
            {
                ioField.vector = rotator->getAngles();
                ioField.text   = formatVec3(ioField.vector);
            }

            return;
        }

        if (ioField.type == AttributeFieldType::Color)
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
            if (ioField.type == AttributeFieldType::Enum)
            {
                ioField.text = typeTail(ioField.text);
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
          selectedFolderPath(Chicane::String::empty()),
          selectedAssetName(Chicane::String::empty()),
          m_collapsedOutlinerItems({}),
          m_editingOutlinerItem(nullptr),
          m_outlinerEditId(Chicane::String::empty()),
          m_coordinateSpace(CoordinateSpace::Absolute),
          m_bOutlinerDirty(false),
          m_bAttributesDirty(false)
    {
        import <AssetManager>();
        import <Attributes>();
        import <Explorer>();
        import <Header>();
        import <Outliner>();
        import <Telemetry>();
        import <Toolbar>();

        load("Assets/Editor/UI/Views/Home.grid", "Assets/Editor/UI/Views/Home.decal");

        bindScene();
    }

    void HomeView::tick(float inDeltaTime)
    {
        flushPendingRebuilds();

        Chicane::Grid::View::tick(inDeltaTime);
    }

    void HomeView::onTick(float inDeltaTime)
    {
        syncAttributeValues();

        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            if (Gizmo* gizmo = scene->getGizmo())
            {
                translateState = gizmo->getType() == GizmoType::Translation ? STATE_ACTIVE : STATE_IDLE;
                rotateState    = gizmo->getType() == GizmoType::Rotation ? STATE_ACTIVE : STATE_IDLE;
                scaleState     = gizmo->getType() == GizmoType::Scale ? STATE_ACTIVE : STATE_IDLE;
            }
        }

        Chicane::Grid::View::onTick(inDeltaTime);
    }

    void HomeView::bindScene()
    {
        auto bind = [this](const std::shared_ptr<Scene>& scene)
        {
            if (!scene)
            {
                return;
            }

            scene->watchActors([this](std::vector<Chicane::Actor*>) { requestOutlinerRebuild(); });
            scene->watchComponents([this](std::vector<Chicane::Component*>) { requestOutlinerRebuild(); });
        };

        bind(Application::getInstance().getHomeScene());
        bind(Application::getInstance().getViewerScene());
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
        if (m_editingOutlinerItem && m_editingOutlinerItem != inItem)
        {
            commitOutlinerEdit(false);
        }

        selectedItem    = inItem;
        bIsItemSelected = selectedItem != nullptr;

        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            scene->setSelection(selectedItem);
        }

        requestOutlinerRebuild();
        requestAttributesRebuild();
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

        requestOutlinerRebuild();
    }

    void HomeView::onItemEdit()
    {
        if (!selectedItem)
        {
            return;
        }

        if (m_editingOutlinerItem == selectedItem)
        {
            return;
        }

        commitOutlinerEdit(false);

        m_editingOutlinerItem = selectedItem;
        m_outlinerEditId      = selectedItem->getId();

        requestOutlinerRebuild();
    }

    void HomeView::onItemDelete()
    {
        if (!selectedItem || selectedItem->isTransient())
        {
            return;
        }

        if (m_editingOutlinerItem)
        {
            commitOutlinerEdit(false);
        }

        std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace);
        if (!scene)
        {
            return;
        }

        Chicane::Object* item = selectedItem;
        onItemSelection(nullptr);
        scene->destroyObject(item);
    }

    void HomeView::onItemIdInput(Chicane::Object* inItem, Chicane::String inValue)
    {
        if (!inItem || inItem != m_editingOutlinerItem)
        {
            return;
        }

        m_outlinerEditId = inValue;

        for (OutlinerNode& node : outlinerNodes)
        {
            if (node.item == inItem)
            {
                node.label = inValue;

                break;
            }
        }
    }

    void HomeView::onItemIdCommit()
    {
        commitOutlinerEdit();
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
        if (workspace.equals(inValue))
        {
            return;
        }

        workspace            = inValue;
        bIsViewportWorkspace = workspace.equals(WORKSPACE_VIEWPORT);
        bIsAssetsWorkspace   = workspace.equals(WORKSPACE_ASSETS);
        viewportTabState     = bIsViewportWorkspace ? STATE_ACTIVE : STATE_IDLE;
        assetsTabState       = bIsAssetsWorkspace ? STATE_ACTIVE : STATE_IDLE;

        if (std::shared_ptr<Scene> home = Application::getInstance().getHomeScene())
        {
            home->setSelection(nullptr);
        }

        if (std::shared_ptr<ViewerScene> viewer = Application::getInstance().getViewerScene())
        {
            viewer->setSelection(nullptr);
        }

        selectedItem    = nullptr;
        bIsItemSelected = false;

        if (bIsAssetsWorkspace)
        {
            Application::getInstance().activateViewerScene();
        }
        else
        {
            Application::getInstance().activateHomeScene();
        }

        requestOutlinerRebuild();
        requestAttributesRebuild();
    }

    void HomeView::onTrackNew()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->open({});

            onItemSelection(nullptr);

            Application::getInstance().possess(scene);
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

                    Application::getInstance().possess(scene);

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

    void HomeView::onAttributeCommit(Chicane::String inName, Chicane::String inValue)
    {
        if (!selectedItem || inName.isEmpty())
        {
            return;
        }

        auto apply = [&](AttributeField& field) -> bool
        {
            if (!field.name.equals(inName))
            {
                return false;
            }

            if (field.type == AttributeFieldType::Bool)
            {
                field.bIsChecked = inValue.toBool() || inValue.equals("true", "1", "yes", "checked");
            }
            else if (field.type == AttributeFieldType::Vec3)
            {
                field.vector = parseVec3(inValue, field.vector);
                field.text   = formatVec3(field.vector);
            }
            else if (field.type == AttributeFieldType::Color)
            {
                const Chicane::String color = inValue.trim();
                if (color.startsWith("#") || color.startsWith("rgb"))
                {
                    const Chicane::Color::Rgba rgba = Chicane::Color::toRgba(color);
                    field.vector                    = Chicane::Vec3(
                        static_cast<float>(rgba.r) / 255.0f,
                        static_cast<float>(rgba.g) / 255.0f,
                        static_cast<float>(rgba.b) / 255.0f
                    );
                }
                else
                {
                    field.vector = parseVec3(inValue, field.vector);
                }

                field.text = formatVec3(field.vector);
            }
            else if (field.type == AttributeFieldType::Float)
            {
                field.text = inValue;
                if (!isCompleteFloat(inValue))
                {
                    return true;
                }
            }
            else
            {
                field.text = inValue;
            }

            commitAttributeField(*selectedItem, field, m_coordinateSpace);

            return true;
        };

        auto refreshTransformFields = [this]()
        {
            for (AttributeGroup& group : attributeGroups)
            {
                if (!group.label.equals(TRANSFORM_GROUP_LABEL))
                {
                    continue;
                }

                for (AttributeField& field : group.fields)
                {
                    if (isTransformAttribute(field.name))
                    {
                        assignTransformAttribute(*selectedItem, field, m_coordinateSpace);
                    }
                }

                return;
            }
        };

        for (AttributeField& field : attributeFields)
        {
            if (apply(field))
            {
                if (isCoordinateSpaceAttribute(inName))
                {
                    refreshTransformFields();
                }

                return;
            }
        }

        for (AttributeGroup& group : attributeGroups)
        {
            for (AttributeField& field : group.fields)
            {
                if (apply(field))
                {
                    if (isCoordinateSpaceAttribute(inName))
                    {
                        refreshTransformFields();
                    }

                    return;
                }
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

    void HomeView::onSpawn(Chicane::String inTypeName)
    {
        std::shared_ptr<Scene> scene = editorScene();
        if (!scene || inTypeName.isEmpty())
        {
            return;
        }

        try
        {
            onItemSelection(scene->createActorFromTag(inTypeName));

            return;
        }
        catch (const std::exception&)
        {}

        Chicane::Component* component = nullptr;
        try
        {
            component = scene->createComponentFromTag(inTypeName);
        }
        catch (const std::exception&)
        {
            return;
        }

        if (!component)
        {
            return;
        }

        Chicane::Object* parent = selectedItem;
        if (Chicane::Component* selectedComponent = dynamic_cast<Chicane::Component*>(parent))
        {
            parent = selectedComponent->getParent();
        }

        if (!parent || parent->isTransient())
        {
            parent = scene->createActor<Chicane::Actor>();
        }

        component->attachTo(parent);
        component->activate();
        onItemSelection(component);
    }

    void HomeView::onGizmoTranslate()
    {
        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            scene->setGizmoType(GizmoType::Translation);
        }
    }

    void HomeView::onGizmoRotate()
    {
        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            scene->setGizmoType(GizmoType::Rotation);
        }
    }

    void HomeView::onGizmoScale()
    {
        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            scene->setGizmoType(GizmoType::Scale);
        }
    }

    void HomeView::onExplorerFolder(Chicane::String inPath)
    {
        selectedFolderPath = inPath;
        selectedAssetName  = Chicane::String::empty();
    }

    void HomeView::onExplorerAsset(Chicane::String inName)
    {
        selectedAssetName = inName;
    }

    void HomeView::onExplorerAssetDrop(Chicane::String inPath)
    {
        if (inPath.isEmpty())
        {
            return;
        }

        std::shared_ptr<Scene> scene = editorScene();
        if (!scene)
        {
            return;
        }

        switch (Chicane::Box::getTypeFromExtension(inPath))
        {
        case Chicane::Box::AssetType::Mesh: {
            if (!bIsViewportWorkspace)
            {
                setWorkspace(WORKSPACE_VIEWPORT);
            }

            Chicane::Actor* actor = scene->spawnMeshActor(inPath);

            onItemSelection(actor);

            break;
        }

        default:
            break;
        }
    }

    void HomeView::rebuildOutliner()
    {
        expandOutlinerAncestors(selectedItem);

        outlinerNodes.clear();

        std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace);
        if (!scene)
        {
            m_collapsedOutlinerItems.clear();
            m_editingOutlinerItem = nullptr;
            m_outlinerEditId      = Chicane::String::empty();

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

            it++;
        }

        if (selectedItem && live.find(selectedItem) == live.end())
        {
            selectedItem    = nullptr;
            bIsItemSelected = false;

            scene->setSelection(nullptr);

            rebuildAttributes();
        }

        if (m_editingOutlinerItem && live.find(m_editingOutlinerItem) == live.end())
        {
            m_editingOutlinerItem = nullptr;
            m_outlinerEditId      = Chicane::String::empty();
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
            node.label         = inObject == m_editingOutlinerItem ? m_outlinerEditId : inObject->getId();
            node.icon          = outlinerIcon(inObject);
            node.indent        = Chicane::String::sprint("%.2fem", static_cast<float>(inDepth) * 0.85f);
            node.expandState   = !bHasChildren  ? OUTLINER_EXPAND_LEAF
                                 : bIsCollapsed ? OUTLINER_EXPAND_COLLAPSED
                                                : OUTLINER_EXPAND_EXPANDED;
            node.selectedState = inObject == selectedItem ? "selected" : "idle";
            node.bHasChildren  = bHasChildren;
            node.bIsLeaf       = !bHasChildren;
            node.bIsEditing    = inObject == m_editingOutlinerItem;
            node.bShowLabel    = !node.bIsEditing;
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

    void HomeView::commitOutlinerEdit(bool bShouldRebuild)
    {
        Chicane::Object* item = m_editingOutlinerItem;
        if (!item)
        {
            return;
        }

        const Chicane::String id = m_outlinerEditId.trim();
        m_editingOutlinerItem    = nullptr;
        m_outlinerEditId         = Chicane::String::empty();

        if (!id.isEmpty() && !id.equals(item->getId()))
        {
            std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace);
            if (scene)
            {
                Chicane::Object* existing = scene->getObject(id);
                if (!existing || existing == item)
                {
                    item->setId(id);
                }
            }
            else
            {
                item->setId(id);
            }
        }

        if (bShouldRebuild)
        {
            requestOutlinerRebuild();
            requestAttributesRebuild();
        }
    }

    void HomeView::requestOutlinerRebuild()
    {
        m_bOutlinerDirty.store(true, std::memory_order_release);
    }

    void HomeView::requestAttributesRebuild()
    {
        m_bAttributesDirty.store(true, std::memory_order_release);
    }

    void HomeView::flushPendingRebuilds()
    {
        const bool bOutliner   = m_bOutlinerDirty.exchange(false, std::memory_order_acq_rel);
        const bool bAttributes = m_bAttributesDirty.exchange(false, std::memory_order_acq_rel);

        if (bOutliner)
        {
            rebuildOutliner();
        }

        if (bAttributes)
        {
            rebuildAttributes();
        }
    }

    static bool isLeafAttribute(const Chicane::ReflectionFieldAccessor& inAccessor, const Chicane::String& inName)
    {
        if (findEnum(inAccessor.typeName))
        {
            return true;
        }

        return inAccessor.isType<bool>() || inAccessor.isType<float>() || inAccessor.isType<int>() ||
               inAccessor.isType<Chicane::Vec3>() || inAccessor.isType<Chicane::Rotator>() ||
               inAccessor.isType<Chicane::String>() || inAccessor.isType<Chicane::FileSystem::Path>() ||
               inName.equals("color");
    }

    static const Chicane::ReflectionTypeInfo* nestedAttributeType(const Chicane::ReflectionFieldInfo& inInfo)
    {
        if (!inInfo.typeIndex.has_value())
        {
            return nullptr;
        }

        return Chicane::ReflectionTypeRegistry::getInstance().find(inInfo.typeIndex.value());
    }

    static void pushAttributeField(AttributeGroup::List& ioGroups, AttributeField inField)
    {
        if (inField.group.isEmpty())
        {
            inField.group = "Properties";
        }

        AttributeGroup* group = nullptr;
        for (AttributeGroup& candidate : ioGroups)
        {
            if (!candidate.label.equals(inField.group))
            {
                continue;
            }

            group = &candidate;

            break;
        }

        if (!group)
        {
            ioGroups.push_back({});
            group        = &ioGroups.back();
            group->label = inField.group;
        }

        group->fields.push_back(inField);
    }

    static void collectAttributeFields(
        AttributeGroup::List&              ioGroups,
        Chicane::Object&                   inItem,
        const Chicane::ReflectionTypeInfo& inRoot,
        const Chicane::ReflectionTypeInfo& inType,
        const Chicane::String&             inPrefix,
        const Chicane::String&             inFallbackGroup,
        CoordinateSpace                    inSpace
    )
    {
        for (const Chicane::ReflectionFieldInfo& info : inType.fields)
        {
            if (info.names.empty() || info.bIsIterable)
            {
                continue;
            }

            const Chicane::String name = info.names.front();
            if (name.equals("angles", "right", "forward", "up"))
            {
                continue;
            }

            if (!inPrefix.isEmpty() && isTransformAttribute(name))
            {
                continue;
            }

            const Chicane::String                  path     = inPrefix.isEmpty() ? name : inPrefix + "." + name;
            const Chicane::ReflectionFieldAccessor accessor = inRoot.resolve(path);
            if (!accessor.isValid())
            {
                continue;
            }

            Chicane::String group = info.group;
            if (group.isEmpty())
            {
                group = inFallbackGroup;
            }
            if (group.isEmpty())
            {
                group = typeGroupLabel(inRoot.group);
            }

            if (info.bIsPointer || !isLeafAttribute(accessor, name))
            {
                const Chicane::ReflectionTypeInfo* nested = nestedAttributeType(info);
                if (nested && nested != &inRoot && nested != &inType)
                {
                    collectAttributeFields(ioGroups, inItem, inRoot, *nested, path, group, inSpace);
                }

                continue;
            }

            AttributeField field;
            field.name        = path;
            field.label       = name;
            field.group       = group;
            field.description = info.description;
            field.type        = AttributeFieldType::Text;

            if (const Chicane::ReflectionEnumInfo* enumeration = findEnum(accessor.typeName))
            {
                field.type = AttributeFieldType::Enum;
                field.text = accessor.toString(&inItem);

                for (const Chicane::ReflectionEnumeratorInfo& enumerator : enumeration->enumerators)
                {
                    const Chicane::String option = typeTail(enumerator.name);
                    field.options.push_back(option);
                    if (field.text.equals(enumerator.name, option))
                    {
                        field.text = option;
                    }
                    else if (enumerator.value == 0 && field.text.isEmpty())
                    {
                        field.text = option;
                    }
                }
            }
            else if (accessor.isType<bool>())
            {
                field.type        = AttributeFieldType::Bool;
                const bool* value = accessor.getValue<bool>(&inItem);
                field.bIsChecked  = value && *value;
            }
            else if (name.equals("color"))
            {
                field.type = AttributeFieldType::Color;
                if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem))
                {
                    field.vector = *value;
                    field.text   = formatVec3(*value);
                }
            }
            else if (accessor.isType<Chicane::Vec3>() || accessor.isType<Chicane::Rotator>())
            {
                field.type = AttributeFieldType::Vec3;
                if (isTransformAttribute(name))
                {
                    assignTransformAttribute(inItem, field, inSpace);
                }
                else if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem))
                {
                    field.vector = *value;
                    field.text   = formatVec3(field.vector);
                }
                else if (const Chicane::Rotator* rotator = accessor.getValue<Chicane::Rotator>(&inItem))
                {
                    field.vector = rotator->getAngles();
                    field.text   = formatVec3(field.vector);
                }
                else
                {
                    field.text = formatVec3(field.vector);
                }
            }
            else if (accessor.isType<float>())
            {
                field.type = AttributeFieldType::Float;
                field.text = accessor.toString(&inItem);
            }
            else if (accessor.isType<Chicane::String>() || accessor.isType<Chicane::FileSystem::Path>() ||
                     accessor.isType<int>())
            {
                field.type = AttributeFieldType::Text;
                if (accessor.isType<Chicane::FileSystem::Path>())
                {
                    const Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(&inItem);
                    field.text                            = path ? path->toString() : Chicane::String::empty();
                    field.type                            = AttributeFieldType::Asset;
                    field.kind                            = name;
                }
                else
                {
                    field.text = accessor.toString(&inItem);
                }
            }
            else
            {
                continue;
            }

            pushAttributeField(ioGroups, field);
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

        collectAttributeFields(attributeGroups, *selectedItem, *type, *type, {}, {}, m_coordinateSpace);
        insertCoordinateSpaceField(attributeGroups, m_coordinateSpace);
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
            syncAttributeField(*selectedItem, *type, field, m_coordinateSpace);
        }

        for (AttributeGroup& group : attributeGroups)
        {
            for (AttributeField& field : group.fields)
            {
                syncAttributeField(*selectedItem, *type, field, m_coordinateSpace);
            }
        }
    }

    bool HomeView::hasSelectedItem() const
    {
        return selectedItem != nullptr;
    }

    CoordinateSpace HomeView::getCoordinateSpace() const
    {
        return m_coordinateSpace;
    }
}
