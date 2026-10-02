#include "Editor/UI/Pages/Viewport.reflected.hpp"

#include <cstdlib>
#include <exception>

#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Color.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection/Enum/Registry.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Core/Reflection/Type/Info.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/View.hpp>
#include <Chicane/Runtime/Scene/Trace/Request.hpp>
#include <Chicane/Runtime/Track.hpp>

#include "Editor/Application.hpp"
#include "Editor/Component/Gizmo.hpp"
#include "Editor/Scene.hpp"
#include "Editor/UI/View/Home.hpp"

namespace Editor
{
    namespace Page
    {
        static std::shared_ptr<Scene> workspaceScene(const HomeView* inHome)
        {
            if (!inHome)
            {
                return nullptr;
            }

            if (inHome->bIsAssetsWorkspace)
            {
                return Application::sInstance().getViewerScene();
            }

            return Application::sInstance().getHomeScene();
        }

        Viewport::Viewport(const Chicane::XmlNode& inNode)
            : Chicane::Grid::Dock(inNode),
              translateState(HomeView::STATE_ACTIVE),
              rotateState(HomeView::STATE_IDLE),
              scaleState(HomeView::STATE_IDLE),
              bIsViewPreviewOpen(false),
              attributeFields({}),
              attributeGroups({}),
              bIsItemSelected(false),
              m_previewSource(nullptr),
              m_attributeItem(nullptr),
              m_coordinateSpace(CoordinateSpace::Absolute),
              m_attributesType(nullptr),
              m_cursor(Chicane::Vec2::sZero()),
              m_modifiers(Chicane::Input::KeyboardButtonModifier::None),
              m_bLeft(false)
        {
            m_tag = Chicane::Grid::Dock::TAG_ID;
            markStyleDirty();
            bindCursor();
        }

        HomeView* Viewport::home() const
        {
            return dynamic_cast<HomeView*>(getRoot());
        }

        Chicane::Object* Viewport::subject() const
        {
            HomeView* view = home();

            return view ? view->selectedItem : nullptr;
        }

        void Viewport::bindCursor()
        {
            Chicane::Controller* controller = Chicane::Instance::sInstance().getController();
            if (!controller)
            {
                return;
            }

            controller->bindEvent([this](const Chicane::Input::MouseMotionEvent& inEvent) { m_cursor = inEvent.location; });

            const auto bindModifier =
                [this, controller](Chicane::Input::KeyboardButton inButton, Chicane::Input::KeyboardButtonModifier inFlag)
            {
                controller->bindEvent(
                    inButton,
                    Chicane::Input::Status::Pressed,
                    [this, inFlag]()
                    {
                        const std::uint16_t bits = static_cast<std::uint16_t>(m_modifiers);
                        const std::uint16_t mask = static_cast<std::uint16_t>(inFlag);
                        m_modifiers              = static_cast<Chicane::Input::KeyboardButtonModifier>(bits | mask);
                    }
                );
                controller->bindEvent(
                    inButton,
                    Chicane::Input::Status::Released,
                    [this, inFlag]()
                    {
                        const std::uint16_t bits = static_cast<std::uint16_t>(m_modifiers);
                        const std::uint16_t mask = static_cast<std::uint16_t>(inFlag);
                        m_modifiers = static_cast<Chicane::Input::KeyboardButtonModifier>(bits & static_cast<std::uint16_t>(~mask));
                    }
                );
            };

            bindModifier(Chicane::Input::KeyboardButton::LAlt, Chicane::Input::KeyboardButtonModifier::LeftAlt);
            bindModifier(Chicane::Input::KeyboardButton::RAlt, Chicane::Input::KeyboardButtonModifier::RightAlt);

            controller->bindEvent(
                Chicane::Input::MouseButton::Left,
                Chicane::Input::Status::Pressed,
                [this]() { m_bLeft = true; }
            );
            controller->bindEvent(
                Chicane::Input::MouseButton::Left,
                Chicane::Input::Status::Released,
                [this]() { m_bLeft = false; }
            );
        }

        void Viewport::refreshAttributes()
        {
            Chicane::Object*                   item = subject();
            const Chicane::ReflectionTypeInfo* type = selectedAttributeType();
            bIsItemSelected                         = item != nullptr;

            if (item != m_attributeItem || type != m_attributesType)
            {
                m_attributeItem = item;
                rebuildAttributes();

                return;
            }

            syncAttributeValues();
        }

        const Chicane::Vec2& Viewport::cursor() const
        {
            return m_cursor;
        }

        Viewport* findViewport(Chicane::Grid::Component* inComponent)
        {
            if (!inComponent)
            {
                return nullptr;
            }

            if (Viewport* page = dynamic_cast<Viewport*>(inComponent))
            {
                return page;
            }

            for (Chicane::Grid::Component* child : inComponent->getChildren())
            {
                if (Viewport* page = findViewport(child))
                {
                    return page;
                }
            }

            return nullptr;
        }

        void Viewport::onTick(float inDeltaTime)
        {
            (void)inDeltaTime;

            HomeView*        view = home();
            Chicane::Object* item = view ? view->selectedItem : nullptr;
            if (item != m_previewSource)
            {
                syncViewPreview(item);
            }

            refreshAttributes();

            std::shared_ptr<Scene> scene = workspaceScene(view);
            if (!scene)
            {
                return;
            }

            Gizmo* gizmo = scene->getGizmo();
            if (!gizmo)
            {
                return;
            }

            translateState = gizmo->getType() == GizmoType::Translation ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
            rotateState    = gizmo->getType() == GizmoType::Rotation ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
            scaleState     = gizmo->getType() == GizmoType::Scale ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
        }

        bool Viewport::isViewportAt(const Chicane::Vec2& inLocation) const
        {
            HomeView* view = home();
            if (!view)
            {
                return false;
            }

            Chicane::Grid::Component* hit = view->getHitAt(inLocation);

            for (Chicane::Grid::Component* node = hit; node != nullptr; node = node->getParent())
            {
                if (node->getTag().equals(Chicane::Grid::Viewport::TAG_ID))
                {
                    return !node->getAttribute(Chicane::Grid::Component::ON_CLICK_ATTRIBUTE_NAME).isEmpty();
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            return false;
        }

        void Viewport::pickAt(const Chicane::Vec2& inLocation)
        {
            HomeView* view = home();
            if (!view || !view->bIsViewportWorkspace || !isViewportAt(inLocation))
            {
                return;
            }

            const std::uint16_t modifiers = static_cast<std::uint16_t>(m_modifiers);
            const std::uint16_t alt       = static_cast<std::uint16_t>(Chicane::Input::KeyboardButtonModifier::Alt);
            if ((modifiers & alt) != 0)
            {
                return;
            }

            Chicane::Window* window = Chicane::Instance::sInstance().getWindow();
            if (!window || window->isFocused() || window->isTextInputActive())
            {
                return;
            }

            std::shared_ptr<Scene> scene = workspaceScene(view);
            if (!scene)
            {
                return;
            }

            const std::vector<Chicane::CCamera*> cameras = scene->getActiveComponents<Chicane::CCamera>();
            if (cameras.empty())
            {
                return;
            }

            Chicane::SceneTraceRequest trace;
            if (!scene
                     ->trace(trace, inLocation, Chicane::Instance::sInstance().getScreenViewportRect(), cameras.back()))
            {
                return;
            }

            if (Gizmo* gizmo = scene->getGizmo())
            {
                if (gizmo->isDragging() || gizmo->hitsHandle(trace))
                {
                    return;
                }
            }

            view->onItemSelection(scene->pickObject(trace));
        }

        void Viewport::onViewportHover()
        {}

        void Viewport::onViewportClick()
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            if (!m_bLeft)
            {
                return;
            }

            pickAt(cursor());
        }

        void Viewport::onViewportFocus()
        {}

        void Viewport::onViewportBlur()
        {}

        void Viewport::applyGizmo(const Chicane::String& inKind)
        {
            std::shared_ptr<Scene> scene = workspaceScene(home());
            if (!scene)
            {
                return;
            }

            const bool bTranslate = inKind.equals("translate");
            const bool bRotate    = !bTranslate && inKind.equals("rotate");
            const bool bScale     = !bTranslate && !bRotate && inKind.equals("scale");

            if (bTranslate)
            {
                scene->setGizmoType(GizmoType::Translation);
            }

            if (bRotate)
            {
                scene->setGizmoType(GizmoType::Rotation);
            }

            if (bScale)
            {
                scene->setGizmoType(GizmoType::Scale);
            }

            translateState = bTranslate ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
            rotateState    = bRotate ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
            scaleState     = bScale ? HomeView::STATE_ACTIVE : HomeView::STATE_IDLE;
        }

        void Viewport::onGizmoTranslate()
        {
            applyGizmo("translate");
        }

        void Viewport::onGizmoRotate()
        {
            applyGizmo("rotate");
        }

        void Viewport::onGizmoScale()
        {
            applyGizmo("scale");
        }

        void Viewport::selectFolder(const Chicane::String& inPath)
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            view->selectedFolderPath = inPath;
            view->selectedAssetName  = Chicane::String::sEmpty();
        }

        void Viewport::selectAsset(const Chicane::String& inName)
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            view->selectedAssetName = inName;
        }

        void Viewport::onExplorerFolder(Chicane::String inPath)
        {
            selectFolder(inPath);
        }

        void Viewport::onExplorerAsset(Chicane::String inName)
        {
            selectAsset(inName);
        }

        void Viewport::onSpawnActor()
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            std::shared_ptr<Scene> scene = Application::sInstance().getHomeScene();
            if (!scene)
            {
                return;
            }

            Chicane::Actor* actor = scene->createActor<Chicane::Actor>();
            actor->setOrigin(Chicane::ObjectOrigin::Instance);
            view->onItemSelection(actor);
        }

        void Viewport::onSpawnMesh()
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            std::shared_ptr<Scene> scene = Application::sInstance().getHomeScene();
            if (!scene)
            {
                return;
            }

            view->onItemSelection(scene->spawnMeshActor(Chicane::Box::Mesh::DEFAULT_SOURCE));
        }

        void Viewport::onSpawn(Chicane::String inTypeName)
        {
            HomeView* view = home();
            if (!view || inTypeName.isEmpty())
            {
                return;
            }

            std::shared_ptr<Scene> scene = Application::sInstance().getHomeScene();
            if (!scene)
            {
                return;
            }

            try
            {
                Chicane::Actor* actor = scene->createActorFromTag(inTypeName);
                actor->setOrigin(Chicane::ObjectOrigin::Instance);
                view->onItemSelection(actor);

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

            Chicane::Object* parent = view->selectedItem;
            if (Chicane::Component* selectedComponent = dynamic_cast<Chicane::Component*>(parent))
            {
                parent = selectedComponent->getParent();
            }

            if (!parent || parent->isTransient())
            {
                parent = scene->createActor<Chicane::Actor>();
                parent->setOrigin(Chicane::ObjectOrigin::Instance);
            }

            component->setOrigin(Chicane::ObjectOrigin::Instance);
            component->attachTo(parent);
            component->activate();
            view->onItemSelection(component);
        }

        void Viewport::onViewPreviewClose()
        {
            bIsViewPreviewOpen = false;
            Chicane::Instance::sInstance().setViewTarget("View", nullptr);
        }

        void Viewport::syncViewPreview(Chicane::Object* inItem)
        {
            m_previewSource = inItem;

            Chicane::CView* view = dynamic_cast<Chicane::CView*>(inItem);
            if (!view && inItem)
            {
                for (Chicane::Object* child : inItem->getAttachments())
                {
                    view = dynamic_cast<Chicane::CView*>(child);
                    if (view && !view->isTransient())
                    {
                        break;
                    }

                    view = nullptr;
                }
            }

            if (view && view->isTransient())
            {
                view = nullptr;
            }

            bIsViewPreviewOpen = view != nullptr;
            Chicane::Instance::sInstance().setViewTarget("View", view);
        }

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
        Chicane::ReflectionEnumRegistry& registry = Chicane::ReflectionEnumRegistry::sInstance();
        if (const Chicane::ReflectionEnumInfo* found = registry.find(inTypeName))
        {
            return found;
        }

        return registry.find(typeTail(inTypeName));
    }

    static Chicane::String formatVec3(const Chicane::Vec3& inValue)
    {
        return Chicane::String::sSprint("%g,%g,%g", inValue.x, inValue.y, inValue.z);
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
        const bool bIsRelative             = inSpace == CoordinateSpace::Relative;
        const bool bNameMatchesTranslation = static_cast<bool>(ioField.name.equals("translation"));

        if (bNameMatchesTranslation)
        {
            ioField.vector = bIsRelative ? inItem.getRelativeTranslation() : inItem.getTranslation();
        }

        const bool bNameMatchesRotation = !bNameMatchesTranslation && (ioField.name.equals("rotation"));

        if (bNameMatchesRotation)
        {
            ioField.vector =
                bIsRelative ? inItem.getRelativeRotation().getAngles() : inItem.getAbsoluteRotation().getAngles();
        }

        const bool bNameMatchesScale =
            !bNameMatchesTranslation && !bNameMatchesRotation && (ioField.name.equals("scale"));

        if (bNameMatchesScale)
        {
            ioField.vector = bIsRelative ? inItem.getRelativeScale() : inItem.getAbsoluteScale();
        }

        ioField.text = formatVec3(ioField.vector);
    }

    static void applyTransformAttribute(Chicane::Object& inItem, const AttributeField& inField, CoordinateSpace inSpace)
    {
        const bool bIsRelative             = inSpace == CoordinateSpace::Relative;
        const bool bNameMatchesTranslation = static_cast<bool>(inField.name.equals("translation"));

        if (bNameMatchesTranslation)
        {
            const bool bRelative = static_cast<bool>(bIsRelative);

            if (bRelative)
            {
                inItem.setRelativeTranslation(inField.vector);
            }

            if (!bRelative)
            {
                inItem.setTranslation(inField.vector);
            }
        }

        const bool bNameMatchesRotation = !bNameMatchesTranslation && (inField.name.equals("rotation"));

        if (bNameMatchesRotation)
        {
            const bool bRelative = static_cast<bool>(bIsRelative);

            if (bRelative)
            {
                inItem.setRelativeRotation(inField.vector);
            }

            if (!bRelative)
            {
                inItem.setAbsoluteRotation(inField.vector);
            }
        }

        const bool bNameMatchesScale =
            !bNameMatchesTranslation && !bNameMatchesRotation && (inField.name.equals("scale"));

        if (bNameMatchesScale)
        {
            const bool bRelative = static_cast<bool>(bIsRelative);

            if (bRelative)
            {
                inItem.setRelativeScale(inField.vector);
            }

            if (!bRelative)
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

        Chicane::String value     = inField.text;
        const bool      bTypeBool = static_cast<bool>(inField.type == AttributeFieldType::Bool);

        if (bTypeBool)
        {
            value = inField.bIsChecked ? "true" : "false";
        }

        const bool bTypeVec3OrTypeColor =
            !bTypeBool && (inField.type == AttributeFieldType::Vec3 || inField.type == AttributeFieldType::Color);

        if (bTypeVec3OrTypeColor)
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

            {
                const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem);

                const Chicane::Rotator* rotator = accessor.getValue<Chicane::Rotator>(&inItem);

                const bool bHasValue = static_cast<bool>(value);

                if (bHasValue)
                {
                    ioField.vector = *value;
                    ioField.text   = formatVec3(*value);
                }

                const bool bHasRotator = !bHasValue && (rotator);

                if (bHasRotator)
                {
                    ioField.vector = rotator->getAngles();
                    ioField.text   = formatVec3(ioField.vector);
                }
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
            ioField.text                          = path ? path->toString() : Chicane::String::sEmpty();

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

        return Chicane::ReflectionTypeRegistry::sInstance().find(inInfo.typeIndex.value());
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
            if (info.getNames().empty() || info.bIsIterable)
            {
                continue;
            }

            const Chicane::String name = info.getName();
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

            Chicane::String group        = info.getGroup();
            const bool      bIsTypeGroup = !group.isEmpty() && group.equals(inType.getGroup());
            const bool      bFallbackGroupEmptyAndEmpty =
                static_cast<bool>(!inFallbackGroup.isEmpty() && (group.isEmpty() || bIsTypeGroup));

            if (bFallbackGroupEmptyAndEmpty)
            {
                group = inFallbackGroup;
            }

            const bool bGroupEmpty = !bFallbackGroupEmptyAndEmpty && (group.isEmpty());

            if (bGroupEmpty)
            {
                group = inType.getGroup();
                if (group.isEmpty())
                {
                    group = typeGroupLabel(inRoot.getGroup());
                }
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
            field.description = info.getDescription();
            field.type        = AttributeFieldType::Text;

            {
                const Chicane::ReflectionEnumInfo* enumeration = findEnum(accessor.typeName);

                const bool bHasEnumeration = static_cast<bool>(enumeration);

                if (bHasEnumeration)
                {
                    field.type = AttributeFieldType::Enum;
                    field.text = accessor.toString(&inItem);

                    for (const Chicane::ReflectionEnumeratorInfo& enumerator : enumeration->enumerators)
                    {
                        const Chicane::String option = typeTail(enumerator.name);
                        field.options.push_back(option);
                        const bool bTextMatchesOption = static_cast<bool>(field.text.equals(enumerator.name, option));

                        if (bTextMatchesOption)
                        {
                            field.text = option;
                        }

                        const bool bEnumeratorZeroAndTextEmpty =
                            !bTextMatchesOption && (enumerator.value == 0 && field.text.isEmpty());

                        if (bEnumeratorZeroAndTextEmpty)
                        {
                            field.text = option;
                        }
                    }
                }

                const bool bBool = !bHasEnumeration && (accessor.isType<bool>());

                if (bBool)
                {
                    field.type        = AttributeFieldType::Bool;
                    const bool* value = accessor.getValue<bool>(&inItem);
                    field.bIsChecked  = value && *value;
                }

                const bool bNameMatchesColor = !bHasEnumeration && !bBool && (name.equals("color"));

                if (bNameMatchesColor)
                {
                    field.type = AttributeFieldType::Color;
                    if (const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem))
                    {
                        field.vector = *value;
                        field.text   = formatVec3(*value);
                    }
                }

                const bool bVec3OrRotator = !bHasEnumeration && !bBool && !bNameMatchesColor &&
                                            (accessor.isType<Chicane::Vec3>() || accessor.isType<Chicane::Rotator>());

                if (bVec3OrRotator)
                {
                    field.type = AttributeFieldType::Vec3;
                    {
                        const Chicane::Vec3* value = accessor.getValue<Chicane::Vec3>(&inItem);

                        const Chicane::Rotator* rotator = accessor.getValue<Chicane::Rotator>(&inItem);

                        const bool bTransformAttribute = static_cast<bool>(isTransformAttribute(name));

                        if (bTransformAttribute)
                        {
                            assignTransformAttribute(inItem, field, inSpace);
                        }

                        const bool bHasValue = !bTransformAttribute && (value);

                        if (bHasValue)
                        {
                            field.vector = *value;
                            field.text   = formatVec3(field.vector);
                        }

                        const bool bHasRotator = !bTransformAttribute && !bHasValue && (rotator);

                        if (bHasRotator)
                        {
                            field.vector = rotator->getAngles();
                            field.text   = formatVec3(field.vector);
                        }

                        if (!bTransformAttribute && !bHasValue && !bHasRotator)
                        {
                            field.text = formatVec3(field.vector);
                        }
                    }
                }

                const bool bFloat =
                    !bHasEnumeration && !bBool && !bNameMatchesColor && !bVec3OrRotator && (accessor.isType<float>());

                if (bFloat)
                {
                    field.type = AttributeFieldType::Float;
                    field.text = accessor.toString(&inItem);
                }

                const bool bStringOrPath = !bHasEnumeration && !bBool && !bNameMatchesColor && !bVec3OrRotator &&
                                           !bFloat &&
                                           (accessor.isType<Chicane::String>() ||
                                            accessor.isType<Chicane::FileSystem::Path>() || accessor.isType<int>());

                if (bStringOrPath)
                {
                    field.type       = AttributeFieldType::Text;
                    const bool bPath = static_cast<bool>(accessor.isType<Chicane::FileSystem::Path>());

                    if (bPath)
                    {
                        const Chicane::FileSystem::Path* path = accessor.getValue<Chicane::FileSystem::Path>(&inItem);
                        field.text                            = path ? path->toString() : Chicane::String::sEmpty();
                        field.type                            = AttributeFieldType::Asset;
                        field.kind                            = name;
                    }

                    if (!bPath)
                    {
                        field.text = accessor.toString(&inItem);
                    }
                }

                if (!bHasEnumeration && !bBool && !bNameMatchesColor && !bVec3OrRotator && !bFloat && !bStringOrPath)
                {
                    continue;
                }
            }

            pushAttributeField(ioGroups, field);
        }
    }
    void Viewport::onAttributeCommit(Chicane::String inName, Chicane::String inValue)
    {
        if (!subject() || inName.isEmpty())
        {
            return;
        }

        auto apply = [&](AttributeField& field) -> bool
        {
            if (!field.name.equals(inName))
            {
                return false;
            }

            const bool bTypeBool = static_cast<bool>(field.type == AttributeFieldType::Bool);

            if (bTypeBool)
            {
                field.bIsChecked = inValue.toBool() || inValue.equals("true", "1", "yes", "checked");
            }

            const bool bTypeVec3 = !bTypeBool && (field.type == AttributeFieldType::Vec3);

            if (bTypeVec3)
            {
                field.vector = parseVec3(inValue, field.vector);
                field.text   = formatVec3(field.vector);
            }

            const bool bTypeColor = !bTypeBool && !bTypeVec3 && (field.type == AttributeFieldType::Color);

            if (bTypeColor)
            {
                const Chicane::String color = inValue.trim();
                const bool            bColorStartsTextOrColorStartsRgb =
                    static_cast<bool>(color.startsWith("#") || color.startsWith("rgb"));

                if (bColorStartsTextOrColorStartsRgb)
                {
                    const Chicane::Color::Rgba rgba = Chicane::Color::toRgba(color);
                    field.vector                    = Chicane::Vec3(
                        static_cast<float>(rgba.r) / 255.0f,
                        static_cast<float>(rgba.g) / 255.0f,
                        static_cast<float>(rgba.b) / 255.0f
                    );
                }

                if (!bColorStartsTextOrColorStartsRgb)
                {
                    field.vector = parseVec3(inValue, field.vector);
                }

                field.text = formatVec3(field.vector);
            }

            const bool bTypeFloat =
                !bTypeBool && !bTypeVec3 && !bTypeColor && (field.type == AttributeFieldType::Float);

            if (bTypeFloat)
            {
                field.text = inValue;
                if (!isCompleteFloat(inValue))
                {
                    return true;
                }
            }

            if (!bTypeBool && !bTypeVec3 && !bTypeColor && !bTypeFloat)
            {
                field.text = inValue;
            }

            commitAttributeField(*subject(), field, m_coordinateSpace);

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
                        assignTransformAttribute(*subject(), field, m_coordinateSpace);
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


    void Viewport::rebuildAttributes()
    {
        attributeFields.clear();
        attributeGroups.clear();
        m_attributesType = selectedAttributeType();

        if (!subject() || !m_attributesType)
        {
            return;
        }

        collectAttributeFields(
            attributeGroups,
            *subject(),
            *m_attributesType,
            *m_attributesType,
            {},
            {},
            m_coordinateSpace
        );
        insertCoordinateSpaceField(attributeGroups, m_coordinateSpace);
    }

    const Chicane::ReflectionTypeInfo* Viewport::selectedAttributeType() const
    {
        if (!subject())
        {
            return nullptr;
        }

        return Chicane::ReflectionTypeRegistry::sInstance().find(typeid(*subject()));
    }

    void Viewport::syncAttributeValues()
    {
        if (!subject())
        {
            return;
        }

        const Chicane::ReflectionTypeInfo* type =
            Chicane::ReflectionTypeRegistry::sInstance().find(typeid(*subject()));
        if (!type)
        {
            return;
        }

        for (AttributeField& field : attributeFields)
        {
            syncAttributeField(*subject(), *type, field, m_coordinateSpace);
        }

        for (AttributeGroup& group : attributeGroups)
        {
            for (AttributeField& field : group.fields)
            {
                syncAttributeField(*subject(), *type, field, m_coordinateSpace);
            }
        }
    }

    CoordinateSpace Viewport::getCoordinateSpace() const
    {
        return m_coordinateSpace;
    }
    }
}
