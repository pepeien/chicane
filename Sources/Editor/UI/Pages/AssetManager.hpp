#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Dock.hpp>

namespace Editor
{
    class HomeView;

    namespace Page
    {
        CH_TYPE(Type = (Manual), Alias = (Editor::Page::AssetManager))
        class AssetManager : public Chicane::Grid::Dock
        {
        public:
            CH_CONSTRUCTOR()
            AssetManager(const Chicane::XmlNode& inNode);

        public:
            CH_FUNCTION()
            void onExplorerFolder(Chicane::String inPath);

            CH_FUNCTION()
            void onExplorerAsset(Chicane::String inName);

        private:
            HomeView* home() const;
        };
    }
}
