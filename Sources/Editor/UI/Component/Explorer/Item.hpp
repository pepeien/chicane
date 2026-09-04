#pragma once

#include <Chicane/Core/FileSystem/Item.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
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
        // Tag
        static constexpr inline const char* TAG_ID = "ExplorerItem";

    public:
        CH_CONSTRUCTOR()
        ExplorerItem(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;
        void refreshPosition() override;

    public:
        void bind(const Chicane::FileSystem::Item* inItem, int inIndex, const Chicane::Vec2& inSlot);
        void unbind();
        int  boundIndex() const;

    private:
        void refreshState();

    public:
        CH_FIELD()
        bool isFolder;
        CH_FIELD()
        bool isFile;
        CH_FIELD()
        bool hasPreview;
        CH_FIELD()
        bool showFileGlyph;
        CH_FIELD()
        Chicane::String typeClass;

        CH_FIELD()
        Chicane::String selectionState;

        CH_FIELD()
        Chicane::String itemName;
        CH_FIELD()
        Chicane::String itemPath;

    private:
        const Chicane::FileSystem::Item* m_item;
        int                              m_boundIndex;
        Chicane::Vec2                    m_slot;
    };
}
