#include "Editor/UI/Pages/AssetManager.reflected.hpp"

#include "Editor/UI/View/Home.hpp"

namespace Editor
{
    namespace Page
    {
        AssetManager::AssetManager(const Chicane::XmlNode& inNode)
            : Chicane::Grid::Dock(inNode)
        {
            m_tag = Chicane::Grid::Dock::TAG_ID;
            markStyleDirty();
        }

        HomeView* AssetManager::home() const
        {
            return dynamic_cast<HomeView*>(getRoot());
        }

        void AssetManager::onExplorerFolder(Chicane::String inPath)
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            view->selectedFolderPath = inPath;
            view->selectedAssetName  = Chicane::String::sEmpty();
        }

        void AssetManager::onExplorerAsset(Chicane::String inName)
        {
            HomeView* view = home();
            if (!view)
            {
                return;
            }

            view->selectedAssetName = inName;
        }
    }
}
