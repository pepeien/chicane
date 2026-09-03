#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Button.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class ExplorerItem : public Chicane::Grid::Button
    {
    public:
        CH_CONSTRUCTOR()
        ExplorerItem(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    private:
        void refreshState();

    public:
        CH_FIELD()
        bool isFolder;

        CH_FIELD()
        bool isFile;

        CH_FIELD()
        Chicane::String typeClass;

        CH_FIELD()
        Chicane::String selectionState;

        CH_FIELD()
        Chicane::String itemName;

        CH_FIELD()
        Chicane::String itemPath;
    };
}
