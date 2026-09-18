#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/UI/Component/Outliner/Node.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Outliner))
    class Outliner : public Chicane::Grid::Container
    {
    public:
        // Attributes
        static constexpr inline const char* NODES_ATTRIBUTE             = "outlinerNodes";
        static constexpr inline const char* ON_ITEM_SELECTION_ATTRIBUTE = "onItemSelection";
        static constexpr inline const char* ON_ITEM_TOGGLE_ATTRIBUTE    = "onItemToggle";
        static constexpr inline const char* ON_ITEM_EDIT_ATTRIBUTE      = "onItemEdit";
        static constexpr inline const char* ON_ITEM_ID_INPUT_ATTRIBUTE  = "onItemIdInput";
        static constexpr inline const char* ON_ITEM_ID_COMMIT_ATTRIBUTE = "onItemIdCommit";

    public:
        CH_CONSTRUCTOR()
        Outliner(const Chicane::XmlNode& inNode);

    public:
        bool onEvent(const Chicane::WindowEvent& inEvent) override;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onItemSelection(Chicane::Object* inItem);

        CH_FUNCTION()
        void onItemToggle(Chicane::Object* inItem);

        CH_FUNCTION()
        void onItemIdInput(Chicane::Object* inItem, Chicane::String inValue);

    private:
        bool isItemBodyAt(const Chicane::Vec2& inLocation) const;
        Chicane::Grid::Component* findRenameInput() const;
        bool hasEditingNode() const;
        void beginRename();
        void commitRename();
        void focusRenameInput();

    public:
        CH_FIELD()
        OutlinerNode::List outlinerNodes;

    private:
        bool m_bShouldFocusRename;
        bool m_bRenameFocused;
    };
}
