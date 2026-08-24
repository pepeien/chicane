#pragma once

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Explorer : public Chicane::Grid::Component
    {
    public:
        CH_CONSTRUCTOR()
        Explorer(const pugi::xml_node& inNode);

    public:
        CH_FUNCTION()
        std::vector<Chicane::String> getFolderLocations() const;

        CH_FUNCTION()
        void onOutlinerSwitch(Chicane::String inFolderName);

    public:
        CH_FIELD()
        Chicane::FileSystem::Item explorerFolder;
    };
}
