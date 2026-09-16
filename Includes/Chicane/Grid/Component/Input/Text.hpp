#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Text.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Input::Text))
        class CHICANE_GRID InputText : public Text
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Input::Text";

            // Attributes
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

        public:
            CH_CONSTRUCTOR()
            InputText(const XmlNode& inNode);

        public:
            bool isFocusable() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onRefresh() override;
            void onFocus() override;
            void onBlur() override;
            void refreshSize() override;
            void refreshPosition() override;

        private:
            bool hasSelection() const;

            void refreshValue();

            void refreshChrome();
            void hoistChrome();

            void insert(const String& inText);
            void erase();
            void eraseForward();
            void commit();
            void emitInput();

            void setTextInputActive(bool inValue);

            void setCaret(std::size_t inIndex, bool inShouldSelect);
            bool moveCaret(int inDelta, bool inShouldSelect);

            std::size_t codepointCount() const;

            Vec2 toContentPoint(const Vec2& inLocation) const;

            std::size_t hitIndex(const Vec2& inLocation) const;

            void selectAll();
            void collapseSelection();
            void deleteSelection();
            bool beginSelect(const Vec2& inLocation, bool inShouldExtend);
            bool updateSelect(const Vec2& inLocation);

        public:
            CH_FIELD()
            String value;

            CH_FIELD()
            Vec2 caret;

            CH_FIELD()
            Vec2 selection;

            CH_FIELD()
            float selectionWidth;

            CH_FIELD()
            String caretColor;

        private:
            bool        m_bIsEdited;
            bool        m_bIsSelecting;
            String      m_pendingText;
            std::size_t m_caret;
            std::size_t m_anchor;
        };
    }
}
