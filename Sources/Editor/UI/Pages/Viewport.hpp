#pragma once

#include <Chicane/Core/Input/Keyboard/Button/Modifier.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Dock.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/UI/Component/Attributes/CoordinateSpace.hpp"
#include "Editor/UI/Component/Attributes/Field.hpp"
#include "Editor/UI/Component/Attributes/Group.hpp"

namespace Editor
{
    class HomeView;

    namespace Page
    {
        CH_TYPE(Type = (Manual), Alias = (Editor::Page::Viewport))
        class Viewport : public Chicane::Grid::Dock
        {
        public:
            CH_CONSTRUCTOR()
            Viewport(const Chicane::XmlNode& inNode);

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void onViewportHover();

            CH_FUNCTION()
            void onViewportClick();

            CH_FUNCTION()
            void onViewportFocus();

            CH_FUNCTION()
            void onViewportBlur();

            CH_FUNCTION()
            void onGizmoTranslate();

            CH_FUNCTION()
            void onGizmoRotate();

            CH_FUNCTION()
            void onGizmoScale();

            CH_FUNCTION()
            void onSpawnActor();

            CH_FUNCTION()
            void onSpawnMesh();

            CH_FUNCTION()
            void onSpawn(Chicane::String inTypeName);

            CH_FUNCTION()
            void onExplorerFolder(Chicane::String inPath);

            CH_FUNCTION()
            void onExplorerAsset(Chicane::String inName);

            CH_FUNCTION()
            void onViewPreviewClose();

            CH_FUNCTION()
            void onAttributeCommit(Chicane::String inName, Chicane::String inValue);

            void syncViewPreview(Chicane::Object* inItem);
            CoordinateSpace getCoordinateSpace() const;
            const Chicane::Vec2& cursor() const;

        public:
            CH_FIELD()
            Chicane::String translateState;

            CH_FIELD()
            Chicane::String rotateState;

            CH_FIELD()
            Chicane::String scaleState;

            CH_FIELD()
            bool bIsViewPreviewOpen;

            CH_FIELD()
            AttributeField::List attributeFields;

            CH_FIELD()
            AttributeGroup::List attributeGroups;

            CH_FIELD()
            bool bIsItemSelected;

        private:
            HomeView* home() const;
            Chicane::Object* subject() const;

            bool isViewportAt(const Chicane::Vec2& inLocation) const;
            void pickAt(const Chicane::Vec2& inLocation);
            void selectFolder(const Chicane::String& inPath);
            void selectAsset(const Chicane::String& inName);
            void applyGizmo(const Chicane::String& inKind);
            void bindCursor();
            void refreshAttributes();
            void rebuildAttributes();
            void syncAttributeValues();
            const Chicane::ReflectionTypeInfo* selectedAttributeType() const;

            Chicane::Object*                   m_previewSource;
            Chicane::Object*                   m_attributeItem;
            CoordinateSpace                    m_coordinateSpace;
            const Chicane::ReflectionTypeInfo* m_attributesType;
            Chicane::Vec2                      m_cursor;
            Chicane::Input::KeyboardButtonModifier m_modifiers;
            bool                               m_bLeft;
        };

        Viewport* findViewport(Chicane::Grid::Component* inComponent);
    }
}
