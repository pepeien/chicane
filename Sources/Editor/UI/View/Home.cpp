#include "Editor/UI/View/Home.reflected.hpp"

#include <cstdlib>
#include <exception>

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Color.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Input/Keyboard/Button.hpp>
#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Status.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection/Enum/Registry.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Core/Reflection/Type/Info.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene/Trace/Request.hpp>
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
#include "Editor/UI/Component/Console.hpp"
#include "Editor/UI/Component/Explorer.hpp"
#include "Editor/UI/Component/Header.hpp"
#include "Editor/UI/Component/Outliner.hpp"
#include "Editor/UI/Component/Toolbar.hpp"
#include "Editor/UI/Pages/Viewport.hpp"
#include "Editor/UI/Component/Telemetry.hpp"

namespace Editor
{
    static constexpr inline const char* OUTLINER_EXPAND_LEAF      = "leaf";
    static constexpr inline const char* OUTLINER_EXPAND_COLLAPSED = "collapsed";
    static constexpr inline const char* OUTLINER_EXPAND_EXPANDED  = "expanded";
    static constexpr inline const char* OUTLINER_SELECTED         = "selected";

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


    static std::shared_ptr<Scene> editorScene()
    {
        return Application::sInstance().getHomeScene();
    }

    static std::shared_ptr<Scene> workspaceScene(bool bIsAssetsWorkspace)
    {
        if (bIsAssetsWorkspace)
        {
            return Application::sInstance().getViewerScene();
        }

        return Application::sInstance().getHomeScene();
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

        if (const Chicane::Component* component = dynamic_cast<const Chicane::Component*>(inObject))
        {
            if (dynamic_cast<const Chicane::Component*>(component->getParent()))
            {
                return "Image";
            }
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

    static Page::Viewport* findViewportPage(Chicane::Grid::Component* inComponent)
    {
        if (!inComponent)
        {
            return nullptr;
        }

        if (Page::Viewport* page = dynamic_cast<Page::Viewport*>(inComponent))
        {
            return page;
        }

        for (Chicane::Grid::Component* child : inComponent->getChildren())
        {
            if (Page::Viewport* page = findViewportPage(child))
            {
                return page;
            }
        }

        return nullptr;
    }

    static void syncViewportPreview(HomeView* inHome, Chicane::Object* inItem)
    {
        if (Page::Viewport* page = findViewportPage(inHome))
        {
            page->syncViewPreview(inItem);
        }
    }

    static bool hasOutlinerChildren(const Chicane::Object* inObject)
    {
        if (!inObject)
        {
            return false;
        }

        for (Chicane::Object* attachment : inObject->getAttachments())
        {
            if (attachment && !attachment->isTransient())
            {
                return true;
            }
        }

        return false;
    }


    HomeView::HomeView()
        : Chicane::Grid::View(),
          outlinerNodes({}),
          selectedItem(nullptr),
          theme("dark"),
          workspace(WORKSPACE_VIEWPORT),
          bIsViewportWorkspace(true),
          bIsAssetsWorkspace(false),
          viewportTabState(STATE_ACTIVE),
          assetsTabState(STATE_IDLE),
          selectedFolderPath(Chicane::String::sEmpty()),
          selectedAssetName(Chicane::String::sEmpty()),
          m_activeWorkspace(WORKSPACE_VIEWPORT),
          m_expandedOutlinerItems({}),
          m_editingOutlinerItem(nullptr),
          m_outlinerEditId(Chicane::String::sEmpty()),
          m_bOutlinerDirty(false),
          m_bIsListening(nullptr),
          m_pawnSubscription({})
    {
        import <AssetManager>();
        import <Attributes>();
        import <Console>();
        import <Explorer>();
        import <Header>();
        import <Outliner>();
        import <Telemetry>();
        import <Toolbar>();

        addRoute({WORKSPACE_VIEWPORT, "Assets/Editor/UI/Pages/Viewport/Index.grid"});
        addRoute({WORKSPACE_ASSETS, "Assets/Editor/UI/Pages/AssetManager/Index.grid"});
        navigate(WORKSPACE_VIEWPORT);

        load("Assets/Editor/UI/Views/Home/Index.grid", "Assets/Editor/UI/Views/Home/Index.decal");

        bindScene();
        bindController();
    }

    HomeView::~HomeView()
    {
        unbindController();
    }

    void HomeView::tick(float inDeltaTime)
    {
        flushPendingRebuilds();

        Chicane::Grid::View::tick(inDeltaTime);
    }

    void HomeView::onTick(float inDeltaTime)
    {
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

        bind(Application::sInstance().getHomeScene());
        bind(Application::sInstance().getViewerScene());
    }

    void HomeView::bindController()
    {
        unbindController();

        Chicane::Controller* controller = Chicane::Instance::sInstance().getController();
        if (!controller)
        {
            return;
        }

        m_bIsListening                  = std::make_shared<bool>(true);
        std::shared_ptr<bool> listening = m_bIsListening;

        m_pawnSubscription = controller->watchAttachment(
            [this, listening](Chicane::APawn* inPawn)
            {
                if (!listening || !*listening || !inPawn)
                {
                    return;
                }

                bindInput(Chicane::Instance::sInstance().getController());
            }
        );
    }

    void HomeView::unbindController()
    {
        if (m_bIsListening)
        {
            *m_bIsListening = false;
        }

        m_pawnSubscription.complete();
    }

    void HomeView::bindInput(Chicane::Controller* inController)
    {
        if (!inController)
        {
            return;
        }

        std::shared_ptr<bool> listening = m_bIsListening;

        inController->bindEvent(
            Chicane::Input::KeyboardButton::Delete,
            Chicane::Input::Status::Pressed,
            [this, listening]()
            {
                if (!listening || !*listening)
                {
                    return;
                }

                Chicane::Window* window = Chicane::Instance::sInstance().getWindow();
                if (window && window->isTextInputActive())
                {
                    return;
                }

                if (m_editingOutlinerItem)
                {
                    return;
                }

                onItemDelete();
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

    bool HomeView::hasSelectedItem() const
    {
        return selectedItem != nullptr;
    }

    void HomeView::onItemSelection(Chicane::Object* inItem)
    {
        if (m_editingOutlinerItem && m_editingOutlinerItem != inItem)
        {
            commitOutlinerEdit(false);
        }

        syncViewportPreview(this, inItem);

        if (selectedItem == inItem)
        {
            return;
        }

        selectedItem = inItem;

        if (std::shared_ptr<Scene> scene = workspaceScene(bIsAssetsWorkspace))
        {
            scene->setSelection(selectedItem);
        }

        const std::size_t expanded = m_expandedOutlinerItems.size();
        expandOutlinerAncestors(selectedItem);
        const bool bSizeDiffers = static_cast<bool>(m_expandedOutlinerItems.size() != expanded);

        if (bSizeDiffers)
        {
            requestOutlinerRebuild();
        }

        if (!bSizeDiffers)
        {
            syncOutlinerSelection();
        }
    }

    void HomeView::onItemToggle(Chicane::Object* inItem)
    {
        if (!inItem || !hasOutlinerChildren(inItem))
        {
            return;
        }

        if (m_expandedOutlinerItems.erase(inItem) == 0)
        {
            m_expandedOutlinerItems.insert(inItem);
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
        if (!selectedItem || selectedItem->isTransient() || selectedItem->isNative())
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
        workspace            = inValue;
        bIsViewportWorkspace = workspace.equals(WORKSPACE_VIEWPORT);
        bIsAssetsWorkspace   = workspace.equals(WORKSPACE_ASSETS);
        viewportTabState     = bIsViewportWorkspace ? STATE_ACTIVE : STATE_IDLE;
        assetsTabState       = bIsAssetsWorkspace ? STATE_ACTIVE : STATE_IDLE;
        navigate(inValue);

        if (m_activeWorkspace.equals(inValue))
        {
            return;
        }

        m_activeWorkspace = inValue;

        if (std::shared_ptr<Scene> home = Application::sInstance().getHomeScene())
        {
            home->setSelection(nullptr);
        }

        if (std::shared_ptr<ViewerScene> viewer = Application::sInstance().getViewerScene())
        {
            viewer->setSelection(nullptr);
        }

        selectedItem = nullptr;
        syncViewportPreview(this, nullptr);

        const bool bAssetsWorkspace = static_cast<bool>(bIsAssetsWorkspace);

        if (bAssetsWorkspace)
        {
            Application::sInstance().activateViewerScene();
        }

        if (!bAssetsWorkspace)
        {
            Application::sInstance().activateHomeScene();
        }

        requestOutlinerRebuild();
    }

    void HomeView::onTrackNew()
    {
        if (std::shared_ptr<Scene> scene = editorScene())
        {
            scene->open({});

            onItemSelection(nullptr);

            Application::sInstance().possess(scene);
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

                    Application::sInstance().possess(scene);

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
            m_expandedOutlinerItems.clear();
            m_editingOutlinerItem = nullptr;
            m_outlinerEditId      = Chicane::String::sEmpty();

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

        for (auto it = m_expandedOutlinerItems.begin(); it != m_expandedOutlinerItems.end();)
        {
            if (live.find(*it) == live.end())
            {
                it = m_expandedOutlinerItems.erase(it);

                continue;
            }

            it++;
        }

        if (selectedItem && live.find(selectedItem) == live.end())
        {
            selectedItem = nullptr;
            syncViewportPreview(this, nullptr);

            scene->setSelection(nullptr);
        }

        if (m_editingOutlinerItem && live.find(m_editingOutlinerItem) == live.end())
        {
            m_editingOutlinerItem = nullptr;
            m_outlinerEditId      = Chicane::String::sEmpty();
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
        const bool bIsExpanded  = m_expandedOutlinerItems.find(inObject) != m_expandedOutlinerItems.end();

        if (inIsVisible)
        {
            OutlinerNode node;
            node.item          = inObject;
            node.label         = inObject == m_editingOutlinerItem ? m_outlinerEditId : inObject->getId();
            node.icon          = outlinerIcon(inObject);
            node.indent        = Chicane::String::sSprint("%.2fem", static_cast<float>(inDepth) * 0.85f);
            node.expandState   = !bHasChildren ? OUTLINER_EXPAND_LEAF
                                 : bIsExpanded ? OUTLINER_EXPAND_EXPANDED
                                               : OUTLINER_EXPAND_COLLAPSED;
            node.bHasChildren  = bHasChildren;
            node.bIsLeaf       = !bHasChildren;
            node.bIsEditing    = inObject == m_editingOutlinerItem;
            node.bShowLabel    = !node.bIsEditing;
            node.selectedState = inObject == selectedItem ? OUTLINER_SELECTED : STATE_IDLE;
            outlinerNodes.push_back(node);
        }

        for (Chicane::Object* attachment : inObject->getAttachments())
        {
            if (!attachment || attachment->isTransient())
            {
                continue;
            }

            appendOutlinerNode(attachment, inDepth + 1, inIsVisible && bIsExpanded, outLive);
        }
    }

    void HomeView::expandOutlinerAncestors(Chicane::Object* inItem)
    {
        Chicane::Object* current = inItem;
        while (current)
        {
            if (hasOutlinerChildren(current))
            {
                m_expandedOutlinerItems.insert(current);
            }

            current = current->getParent();
        }
    }

    void HomeView::syncOutlinerSelection()
    {
        bool bFound = selectedItem == nullptr;
        for (OutlinerNode& node : outlinerNodes)
        {
            const bool            bIsSelected = node.item == selectedItem;
            const Chicane::String next        = bIsSelected ? OUTLINER_SELECTED : STATE_IDLE;
            if (bIsSelected)
            {
                bFound = true;
            }

            if (!node.selectedState.equals(next))
            {
                node.selectedState = next;
            }
        }

        if (selectedItem && !bFound)
        {
            requestOutlinerRebuild();
            return;
        }

        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (Outliner* outliner = dynamic_cast<Outliner*>(child))
            {
                outliner->syncRowSelection();
            }
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
        m_outlinerEditId         = Chicane::String::sEmpty();

        if (!id.isEmpty() && !id.equals(item->getId()))
        {
            std::shared_ptr<Scene> scene     = workspaceScene(bIsAssetsWorkspace);
            const bool             bHasScene = static_cast<bool>(scene);

            if (bHasScene)
            {
                Chicane::Object* existing = scene->getObject(id);
                if (!existing || existing == item)
                {
                    item->setId(id);
                }
            }

            if (!bHasScene)
            {
                item->setId(id);
            }
        }

        if (bShouldRebuild)
        {
            requestOutlinerRebuild();
        }
    }

    void HomeView::requestOutlinerRebuild()
    {
        m_bOutlinerDirty.store(true, std::memory_order_release);
    }

    void HomeView::flushPendingRebuilds()
    {
        if (m_bOutlinerDirty.exchange(false, std::memory_order_acq_rel))
        {
            rebuildOutliner();
        }
    }

}
