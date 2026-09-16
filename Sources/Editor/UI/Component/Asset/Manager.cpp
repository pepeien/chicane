#include "Editor/UI/Component/Asset/Manager.reflected.hpp"

#include <Chicane/Box/Asset.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Effect.hpp>
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
        createAsset(Chicane::Box::AssetType::Texture, Chicane::Box::Texture::EXTENSION);
    }

    void AssetManager::onCreateMesh()
    {
        createAsset(Chicane::Box::AssetType::Mesh, Chicane::Box::Mesh::EXTENSION);
    }

    void AssetManager::onCreateSky()
    {
        createAsset(Chicane::Box::AssetType::Sky, Chicane::Box::Sky::EXTENSION);
    }

    void AssetManager::onCreateModel()
    {
        createAsset(Chicane::Box::AssetType::Model, Chicane::Box::Model::EXTENSION);
    }

    void AssetManager::onCreateSound()
    {
        createAsset(Chicane::Box::AssetType::Sound, Chicane::Box::Sound::EXTENSION);
    }

    void AssetManager::onCreateEffect()
    {
        createAsset(Chicane::Box::AssetType::Effect, Chicane::Box::Effect::EXTENSION);
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
        dialog.addFilter("Images", {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".hdr"});
        dialog.addFilter("Textures", {Chicane::Box::Texture::EXTENSION});
        dialog.addFilter("Meshes", {Chicane::Box::Mesh::EXTENSION});
        dialog.addFilter("Models", {Chicane::Box::Model::EXTENSION});
        dialog.addFilter("Skies", {Chicane::Box::Sky::EXTENSION});

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

                    Chicane::FileSystem::Path output = item.path.withExtension(Chicane::Box::Texture::EXTENSION);
                    Chicane::Box::Texture     texture(output);
                    texture.setId(output.stem().toString());
                    texture.setData(item.path);
                    texture.saveXML();

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
        dialog.title          = "Create " + Chicane::Box::toString(inType);
        dialog.addFilter(Chicane::Box::toString(inType) + "s", {inExtension});

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
        assetType     = Chicane::Box::toString(asset.getType());

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
