#pragma once

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::AssetManager))
    class AssetManager : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* SELECTED_FOLDER_ATTRIBUTE = "selectedFolderPath";
        static constexpr inline const char* SELECTED_ASSET_ATTRIBUTE  = "selectedAssetName";

    public:
        CH_CONSTRUCTOR()
        AssetManager(const Chicane::XmlNode& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onCreateTexture();

        CH_FUNCTION()
        void onCreateMesh();

        CH_FUNCTION()
        void onCreateSky();

        CH_FUNCTION()
        void onCreateModel();

        CH_FUNCTION()
        void onCreateSound();

        CH_FUNCTION()
        void onCreateEffect();

        CH_FUNCTION()
        void onSave();

        CH_FUNCTION()
        void onImport();

    private:
        void createAsset(Chicane::Box::AssetType inType, const Chicane::String& inExtension);
        void refreshFromExplorer();
        void syncViewer();
        Chicane::FileSystem::Path selectedAssetPath() const;

    public:
        CH_FIELD()
        bool bHasAsset;
        CH_FIELD()
        bool bIsAssetEmpty;
        CH_FIELD()
        bool bIsMeshAsset;
        CH_FIELD()
        Chicane::String assetPath;
        CH_FIELD()
        Chicane::String assetId;
        CH_FIELD()
        Chicane::String assetSource;
        CH_FIELD()
        Chicane::String assetType;

        CH_FIELD()
        Chicane::String selectedFolderPath;
        CH_FIELD()
        Chicane::String selectedAssetName;

    private:
        Chicane::String m_viewerAsset;
    };
}
