#pragma once

#include <functional>

#include <Chicane/Core/FileSystem/Item.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Button.hpp>

#include "Editor/UI/Component/Explorer/Item/Kind.hpp"

namespace Editor
{
    class Explorer;

    CH_TYPE(Type = (Manual), Alias = (Editor::ExplorerItem))
    class ExplorerItem : public Chicane::Grid::Button
    {
    public:
        // Tag
        static constexpr inline const char* TAG_ID = "Editor::ExplorerItem";

    public:
        CH_CONSTRUCTOR()
        ExplorerItem(const Chicane::XmlNode& inNode);

    public:
        bool onEvent(const Chicane::WindowEvent& inEvent) override;
        bool escapesOverflow() const override;
        float getDepth() const override;

    protected:
        void onTick(float inDeltaTime) override;
        void onDrag() override;
        void onDragEnd() override;
        void refreshPosition() override;

    public:
        void bind(
            const Chicane::FileSystem::Item* inItem,
            int                              inIndex,
            const Chicane::Vec2&             inSlot,
            bool                             inShouldRestyle = false
        );
        void unbind();
        int boundIndex() const;

        void showGhost(
            const ExplorerItem& inSource, Chicane::FileSystem::Item& ioStorage, const Chicane::Vec2& inPointer
        );
        void moveGhost(const Chicane::Vec2& inPointer);
        void hideGhost();
        bool isGhost() const;

    private:
        void refreshState();
        bool isDroppableAsset() const;
        Explorer* findExplorer() const;
        void applyGhostShown();
        void applyGhostHidden();
        void restyleChildren();

    public:
        CH_FIELD()
        ExplorerItemKind kind;
        CH_FIELD()
        Chicane::String typeClass;
        CH_FIELD()
        Chicane::String dragClass;
        CH_FIELD()
        Chicane::String ghostClass;

        CH_FIELD()
        Chicane::String selectionState;

        CH_FIELD()
        Chicane::String itemName;
        CH_FIELD()
        Chicane::String itemPath;

        CH_FIELD()
        std::function<void()> onClick;

    private:
        const Chicane::FileSystem::Item* m_item;
        int                              m_boundIndex;
        Chicane::Vec2                    m_slot;
        Chicane::Vec2                    m_pointer;
        Chicane::Vec2                    m_grab;
        Chicane::Vec2                    m_ghostSize;
        bool                             m_bIsGhost;
    };
}
