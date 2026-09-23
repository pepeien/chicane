#include "Editor/UI/Component/Asset/Manager.reflected.hpp"

#include <Chicane/Box/Asset.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Effect.hpp>
#include <Chicane/Box/Font.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Box/Model.hpp>
#include <Chicane/Box/Sky.hpp>
#include <Chicane/Box/Sound.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>

#include "Editor/Application.hpp"
#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Prop.hpp"
#include "Editor/Viewer/Scene.hpp"

namespace Editor
{
    template <typename T>
    static Chicane::FileSystem::Path bakeSource(
        const Chicane::FileSystem::Path& inSource,
        const Chicane::String&           inExtension
    )
    {
        const Chicane::FileSystem::Path output = inSource.withExtension(inExtension);

        T asset(output);
        asset.setId(output.stem().toString());
        asset.setData(inSource);
        asset.saveXML();

        return output;
    }

    static bool hasRawExtension(Chicane::Box::AssetType inType, const Chicane::String& inExtension)
    {
        for (const Chicane::FileSystem::Path& extension : Chicane::Box::getTypeRawExtensions(inType))
        {
            if (inExtension.equals(extension.toString()))
            {
                return true;
            }
        }

        return false;
    }

    AssetManager::AssetManager(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          bHasAsset(false),
          bIsAssetEmpty(true),
          bIsMeshAsset(false),
          assetPath(Chicane::String::empty()),
          assetId(Chicane::String::empty()),
          assetSource(Chicane::String::empty()),
          assetType(Chicane::String::empty()),
          selectedFolderPath(Chicane::String::empty()),
          selectedAssetName(Chicane::String::empty()),
          m_viewerAsset(Chicane::String::empty())
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Asset/Manager.grid", "Assets/Editor/UI/Components/Asset/Manager.decal");

        Prop::bind(this, SELECTED_FOLDER_ATTRIBUTE, selectedFolderPath);
        Prop::bind(this, SELECTED_ASSET_ATTRIBUTE, selectedAssetName);
    }

    void AssetManager::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, SELECTED_FOLDER_ATTRIBUTE, selectedFolderPath);
        Prop::copy(this, SELECTED_ASSET_ATTRIBUTE, selectedAssetName);

        refreshFromExplorer();
    }

    void AssetManager::onCreateTexture()
    {
        createAsset(Chicane::Box::AssetType::Texture, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Texture));
    }

    void AssetManager::onCreateMesh()
    {
        createAsset(Chicane::Box::AssetType::Mesh, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Mesh));
    }

    void AssetManager::onCreateSky()
    {
        createAsset(Chicane::Box::AssetType::Sky, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sky));
    }

    void AssetManager::onCreateModel()
    {
        createAsset(Chicane::Box::AssetType::Model, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Model));
    }

    void AssetManager::onCreateSound()
    {
        createAsset(Chicane::Box::AssetType::Sound, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sound));
    }

    void AssetManager::onCreateEffect()
    {
        createAsset(Chicane::Box::AssetType::Effect, Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Effect));
    }

    void AssetManager::onSave()
    {
        const Chicane::FileSystem::Path path = selectedAssetPath();
        if (path.isEmpty() || !Chicane::Box::isFileAsset(path))
        {
            return;
        }

        Chicane::Box::Asset asset(path);
        if (!assetId.isEmpty())
        {
            asset.setId(assetId);
        }

        if (!assetSource.isEmpty())
        {
            asset.setPayload(assetSource);
        }

        asset.saveXML();

        if (bIsMeshAsset)
        {
            if (std::shared_ptr<ViewerScene> viewer = Application::getInstance().getViewerScene())
            {
                viewer->commitGroups();
            }
        }

        refreshFromExplorer();
    }

    void AssetManager::onImport()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.title          = "Import source";
        dialog.addFilter("Images", Chicane::Box::getTypeRawExtensions(Chicane::Box::AssetType::Texture));
        dialog.addFilter("Models", Chicane::Box::getTypeRawExtensions(Chicane::Box::AssetType::Model));
        dialog.addFilter("Audio", Chicane::Box::getTypeRawExtensions(Chicane::Box::AssetType::Sound));
        dialog.addFilter("Fonts", Chicane::Box::getTypeRawExtensions(Chicane::Box::AssetType::Font));
        dialog.addFilter("Textures", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Texture)});
        dialog.addFilter("Meshes", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Mesh)});
        dialog.addFilter("Model Assets", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Model)});
        dialog.addFilter("Skies", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sky)});
        dialog.addFilter("Sounds", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sound)});
        dialog.addFilter("Font Assets", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Font)});
        dialog.addFilter("Effects", {Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Effect)});

        dialog.open(
            [this](const Chicane::FileSystem::Item::List& inFiles)
            {
                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    if (item.type != Chicane::FileSystem::ItemType::File)
                    {
                        continue;
                    }

                    if (Chicane::Box::isFileAsset(item.path))
                    {
                        assetPath = item.path.toString();
                        refreshFromExplorer();

                        return;
                    }

                    const Chicane::String     extension = item.path.extension().toString().toLower();
                    Chicane::FileSystem::Path output;

                    if (hasRawExtension(Chicane::Box::AssetType::Model, extension))
                    {
                        output = bakeSource<Chicane::Box::Model>(
                            item.path,
                            Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Model)
                        );
                    }
                    else if (hasRawExtension(Chicane::Box::AssetType::Sound, extension))
                    {
                        output = bakeSource<Chicane::Box::Sound>(
                            item.path,
                            Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Sound)
                        );
                    }
                    else if (hasRawExtension(Chicane::Box::AssetType::Font, extension))
                    {
                        output = bakeSource<Chicane::Box::Font>(
                            item.path,
                            Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Font)
                        );
                    }
                    else
                    {
                        output = bakeSource<Chicane::Box::Texture>(
                            item.path,
                            Chicane::Box::getTypeExtension(Chicane::Box::AssetType::Texture)
                        );
                    }

                    assetPath = output.toString();
                    refreshFromExplorer();

                    return;
                }
            }
        );
    }

    void AssetManager::createAsset(Chicane::Box::AssetType inType, const Chicane::String& inExtension)
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.title          = "Create " + Chicane::toString(inType);
        dialog.addFilter(Chicane::toString(inType) + "s", {inExtension});

        dialog.open(
            [this, inType, inExtension](const Chicane::FileSystem::Item::List& inFiles)
            {
                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    Chicane::FileSystem::Path path = item.path;
                    if (!path.extension().toString().equals(inExtension))
                    {
                        path = path.withExtension(inExtension);
                    }

                    switch (inType)
                    {
                    case Chicane::Box::AssetType::Texture: {
                        Chicane::Box::Texture asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    case Chicane::Box::AssetType::Mesh: {
                        Chicane::Box::Mesh asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    case Chicane::Box::AssetType::Sky: {
                        Chicane::Box::Sky asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    case Chicane::Box::AssetType::Model: {
                        Chicane::Box::Model asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    case Chicane::Box::AssetType::Sound: {
                        Chicane::Box::Sound asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    case Chicane::Box::AssetType::Effect: {
                        Chicane::Box::Effect asset(path);
                        asset.setId(path.stem().toString());
                        asset.saveXML();

                        break;
                    }

                    default:
                        break;
                    }

                    assetPath = path.toString();
                    refreshFromExplorer();

                    return;
                }
            }
        );
    }

    void AssetManager::refreshFromExplorer()
    {
        const Chicane::FileSystem::Path path = selectedAssetPath();
        if (path.isEmpty() || !Chicane::Box::isFileAsset(path) || !Chicane::FileSystem::exists(path))
        {
            if (assetPath.isEmpty())
            {
                bHasAsset     = false;
                bIsAssetEmpty = true;
                bIsMeshAsset  = false;
                assetId       = Chicane::String::empty();
                assetSource   = Chicane::String::empty();
                assetType     = Chicane::String::empty();
                syncViewer();
            }

            return;
        }

        if (!assetPath.equals(path.toString()))
        {
            assetPath = path.toString();
        }

        Chicane::Box::Asset asset(path);
        bHasAsset     = true;
        bIsAssetEmpty = false;
        bIsMeshAsset  = asset.getType() == Chicane::Box::AssetType::Mesh;
        assetId       = asset.getId();
        assetSource   = asset.getPayload();
        assetType     = Chicane::toString(asset.getType());

        syncViewer();
    }

    void AssetManager::syncViewer()
    {
        std::shared_ptr<ViewerScene> viewer = Application::getInstance().getViewerScene();
        if (!viewer)
        {
            return;
        }

        if (!bIsMeshAsset)
        {
            if (!m_viewerAsset.isEmpty())
            {
                viewer->clearAsset();
                m_viewerAsset = Chicane::String::empty();
            }

            return;
        }

        if (m_viewerAsset.equals(assetPath))
        {
            return;
        }

        m_viewerAsset = assetPath;
        viewer->setAsset(Chicane::FileSystem::Path(assetPath));
    }

    Chicane::FileSystem::Path AssetManager::selectedAssetPath() const
    {
        if (!assetPath.isEmpty())
        {
            return Chicane::FileSystem::Path(assetPath);
        }

        if (selectedFolderPath.isEmpty() || selectedAssetName.isEmpty())
        {
            return {};
        }

        return Chicane::FileSystem::Path(selectedFolderPath) / selectedAssetName;
    }
}
