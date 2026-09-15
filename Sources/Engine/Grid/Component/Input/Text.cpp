#include "Chicane/Grid/Component/Input/Text.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <glm/gtc/matrix_inverse.hpp>

#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Text/Event.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    namespace Grid
    {
        bool hasModifier(Input::KeyboardButtonModifier inValue, Input::KeyboardButtonModifier inFlag)
        {
            return (static_cast<std::uint16_t>(inValue) & static_cast<std::uint16_t>(inFlag)) != 0;
        }

        String characterFromKey(const Input::KeyboardEvent& inEvent)
        {
            if (hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Ctrl) ||
                hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Alt) ||
                hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Gui))
            {
                return String::empty();
            }

            if (inEvent.key < 32 || inEvent.key > 126)
            {
                return String::empty();
            }

            char character = static_cast<char>(inEvent.key);
            if (character >= 'a' && character <= 'z')
            {
                const bool bHasShift = hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Shift);
                const bool bHasCaps  = hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Caps);
                if (bHasShift != bHasCaps)
                {
                    character = static_cast<char>(character - 32);
                }
            }

            return String(character);
        }

        void appendCodepoint(String& outValue, char32_t inCode)
        {
            if (inCode <= 0x7F)
            {
                outValue += static_cast<char>(inCode);

                return;
            }

            if (inCode <= 0x7FF)
            {
                outValue += static_cast<char>(0xC0 | (inCode >> 6));
                outValue += static_cast<char>(0x80 | (inCode & 0x3F));

                return;
            }

            if (inCode <= 0xFFFF)
            {
                outValue += static_cast<char>(0xE0 | (inCode >> 12));
                outValue += static_cast<char>(0x80 | ((inCode >> 6) & 0x3F));
                outValue += static_cast<char>(0x80 | (inCode & 0x3F));

                return;
            }

            outValue += static_cast<char>(0xF0 | (inCode >> 18));
            outValue += static_cast<char>(0x80 | ((inCode >> 12) & 0x3F));
            outValue += static_cast<char>(0x80 | ((inCode >> 6) & 0x3F));
            outValue += static_cast<char>(0x80 | (inCode & 0x3F));
        }

        String fromCodepoints(const std::vector<char32_t>& inCodes)
        {
            String result;

            for (const char32_t code : inCodes)
            {
                appendCodepoint(result, code);
            }

            return result;
        }

        InputText::InputText(const pugi::xml_node& inNode)
            : Text(inNode),
              value(String::empty()),
              caret(Vec2::Zero()),
              selection(Vec2::Zero()),
              selectionWidth(0.0f),
              caretColor("#252525FF"),
              m_bIsEdited(false),
              m_bIsSelecting(false),
              m_pendingText(String::empty()),
              m_caret(0),
              m_anchor(0)
        {
            load("Assets/Engine/UI/Components/Input/Text.grid", "Assets/Engine/UI/Components/Input/Text.decal");
            hoistChrome();
        }

        bool InputText::isFocusable() const
        {
            return true;
        }

        bool InputText::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowFocusLost)
            {
                setTextInputActive(false);

                return true;
            }

            if (inEvent.type == WindowEventType::WindowFocusGained)
            {
                if (isFocused())
                {
                    setTextInputActive(true);
                }

                return true;
            }

            if (Scrollable::onEvent(inEvent))
            {
                return true;
            }

            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (!m_bIsSelecting)
                {
                    return false;
                }

                m_bIsSelecting = false;

                return true;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left || !containsPoint(event.location))
                {
                    return false;
                }

                return beginSelect(event.location, false);
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!m_bIsSelecting || !inEvent.data)
                {
                    return false;
                }

                const Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);

                return updateSelect(event.location);
            }

            if (!isFocused())
            {
                return false;
            }

            if (inEvent.type == WindowEventType::TextInput)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::TextEvent event = *static_cast<Input::TextEvent*>(inEvent.data);
                if (event.text.isEmpty() || event.text.equals(m_pendingText))
                {
                    m_pendingText = String::empty();

                    return true;
                }

                insert(event.text);
                m_pendingText = String::empty();

                return true;
            }

            if (inEvent.type != WindowEventType::KeyDown)
            {
                return inEvent.type == WindowEventType::KeyUp;
            }

            if (!inEvent.data)
            {
                return true;
            }

            const Input::KeyboardEvent event  = *static_cast<Input::KeyboardEvent*>(inEvent.data);
            const bool                 bShift = hasModifier(event.modifier, Input::KeyboardButtonModifier::Shift);
            const bool                 bCtrl  = hasModifier(event.modifier, Input::KeyboardButtonModifier::Ctrl) ||
                               hasModifier(event.modifier, Input::KeyboardButtonModifier::Gui);

            if (bCtrl && event.button == Input::KeyboardButton::A)
            {
                selectAll();

                return true;
            }

            if (event.button == Input::KeyboardButton::Backspace)
            {
                erase();
                m_pendingText = String::empty();

                return true;
            }

            if (event.button == Input::KeyboardButton::Delete)
            {
                eraseForward();
                m_pendingText = String::empty();

                return true;
            }

            if (event.button == Input::KeyboardButton::Left)
            {
                return moveCaret(-1, bShift);
            }

            if (event.button == Input::KeyboardButton::Right)
            {
                return moveCaret(1, bShift);
            }

            if (event.button == Input::KeyboardButton::Home)
            {
                setCaret(0, bShift);

                return true;
            }

            if (event.button == Input::KeyboardButton::End)
            {
                setCaret(codepointCount(), bShift);

                return true;
            }

            const String character = characterFromKey(event);
            if (character.isEmpty())
            {
                return true;
            }

            insert(character);
            m_pendingText = character;

            return true;
        }

        void InputText::onRefresh()
        {
            refreshValue();
            setText(value);

            Text::onRefresh();
            refreshChrome();
        }

        void InputText::onFocus()
        {
            if (!m_bIsSelecting)
            {
                selectAll();
            }

            setTextInputActive(true);
            refreshStyleSubtree();
        }

        void InputText::onBlur()
        {
            m_pendingText  = String::empty();
            m_bIsSelecting = false;
            collapseSelection();
            setTextInputActive(false);
            refreshStyleSubtree();
        }

        void InputText::refreshSize()
        {
            m_style.width.refresh();
            m_style.height.refresh();

            Component::refreshSize();
        }

        void InputText::refreshPosition()
        {
            Text::refreshPosition();

            refreshChrome();
        }

        bool InputText::hasSelection() const
        {
            return m_caret != m_anchor;
        }

        void InputText::refreshValue()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (raw.isEmpty() || isFocused())
            {
                return;
            }

            if (!isReference(raw) && m_bIsEdited)
            {
                return;
            }

            value    = parseText(raw);
            m_caret  = codepointCount();
            m_anchor = m_caret;
        }

        void InputText::refreshChrome()
        {
            const std::size_t count = codepointCount();
            m_caret                 = std::min(m_caret, count);
            m_anchor                = std::min(m_anchor, count);

            const Color::Rgba color     = m_style.foregroundColor.get();
            const String      nextColor = String::sprint(
                "#%02X%02X%02X%02X",
                static_cast<unsigned>(color.r),
                static_cast<unsigned>(color.g),
                static_cast<unsigned>(color.b),
                static_cast<unsigned>(color.a)
            );
            const std::size_t start = std::min(m_caret, m_anchor);
            const std::size_t end   = std::max(m_caret, m_anchor);
            const Vec2        nextCaret(getInsertionX(m_caret), 0.0f);
            Vec2              nextSelection(nextCaret.x, 0.0f);
            float             nextWidth = 0.0f;

            if (isFocused() && start != end)
            {
                nextSelection.x = getInsertionX(start);
                nextWidth       = std::max(0.0f, getInsertionX(end) - nextSelection.x);
            }

            if (std::fabs(caret.x - nextCaret.x) < 0.01f && std::fabs(caret.y - nextCaret.y) < 0.01f &&
                std::fabs(selection.x - nextSelection.x) < 0.01f && std::fabs(selection.y - nextSelection.y) < 0.01f &&
                std::fabs(selectionWidth - nextWidth) < 0.01f && caretColor.equals(nextColor))
            {
                return;
            }

            caret          = nextCaret;
            selection      = nextSelection;
            selectionWidth = nextWidth;
            caretColor     = nextColor;

            for (Component* child : getChildren())
            {
                if (!child)
                {
                    continue;
                }

                child->markStyleDirty();
                child->markLayoutDirty();
            }
        }

        void InputText::hoistChrome()
        {
            if (getChildren().size() != 1 || !getChildren().front())
            {
                return;
            }

            Component*                    chrome = getChildren().front();
            const std::vector<Component*> nested = chrome->getChildren();
            for (Component* node : nested)
            {
                chrome->releaseChild(node);
                addChild(node);
            }

            removeChild(chrome);
        }

        void InputText::insert(const String& inText)
        {
            if (inText.isEmpty() || inText.equals("\n", "\r", "\t"))
            {
                return;
            }

            if (hasSelection())
            {
                deleteSelection();
            }

            std::vector<char32_t> codes    = value.toUnicode();
            const auto            incoming = inText.toUnicode();
            const std::size_t     at       = std::min(m_caret, codes.size());

            codes.insert(codes.begin() + static_cast<std::ptrdiff_t>(at), incoming.begin(), incoming.end());
            value       = fromCodepoints(codes);
            m_caret     = at + incoming.size();
            m_anchor    = m_caret;
            m_bIsEdited = true;
            setText(value);

            commit();
            emitInput();
        }

        void InputText::erase()
        {
            if (hasSelection())
            {
                deleteSelection();
                m_bIsEdited = true;
                setText(value);
                commit();
                emitInput();

                return;
            }

            if (m_caret == 0 || value.isEmpty())
            {
                return;
            }

            std::vector<char32_t> codes = value.toUnicode();
            if (codes.empty())
            {
                return;
            }

            const std::size_t at = std::min(m_caret, codes.size());
            codes.erase(codes.begin() + static_cast<std::ptrdiff_t>(at - 1));
            value       = fromCodepoints(codes);
            m_caret     = at - 1;
            m_anchor    = m_caret;
            m_bIsEdited = true;
            setText(value);

            commit();
            emitInput();
        }

        void InputText::eraseForward()
        {
            if (hasSelection())
            {
                deleteSelection();
                m_bIsEdited = true;
                setText(value);
                commit();
                emitInput();

                return;
            }

            std::vector<char32_t> codes = value.toUnicode();
            if (m_caret >= codes.size())
            {
                return;
            }

            codes.erase(codes.begin() + static_cast<std::ptrdiff_t>(m_caret));
            value       = fromCodepoints(codes);
            m_anchor    = m_caret;
            m_bIsEdited = true;
            setText(value);

            commit();
            emitInput();
        }

        void InputText::commit()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!isReference(raw) || !hasParent())
            {
                return;
            }

            const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            if (id.isEmpty())
            {
                return;
            }

            for (Component* node = getParent(); node != nullptr; node = node->getParent())
            {
                const ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (accessor.isType<String>())
                    {
                        if (String* target = accessor.getValue<String>(instance))
                        {
                            *target = value;
                        }

                        return;
                    }

                    if (accessor.isType<float>())
                    {
                        const String trimmed = value.trim();
                        if (trimmed.isEmpty())
                        {
                            return;
                        }

                        char*       end    = nullptr;
                        const float parsed = std::strtof(trimmed.toChar(), &end);
                        if (!end || end == trimmed.toChar() || *end != '\0')
                        {
                            return;
                        }

                        if (float* target = accessor.getValue<float>(instance))
                        {
                            *target = parsed;
                        }

                        return;
                    }
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void InputText::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void InputText::setTextInputActive(bool inValue)
        {
            Window* window = Window::getCurrent();
            if (!window)
            {
                return;
            }

            if (inValue)
            {
                window->startTextInput();

                return;
            }

            window->stopTextInput();
        }

        void InputText::setCaret(std::size_t inIndex, bool inShouldSelect)
        {
            m_caret = std::min(inIndex, codepointCount());
            if (!inShouldSelect)
            {
                m_anchor = m_caret;
            }

            refreshChrome();
        }

        bool InputText::moveCaret(int inDelta, bool inShouldSelect)
        {
            if (!inShouldSelect && hasSelection() && (inDelta < 0 || inDelta > 0))
            {
                const std::size_t edge = inDelta < 0 ? std::min(m_caret, m_anchor) : std::max(m_caret, m_anchor);
                setCaret(edge, false);

                return true;
            }

            const int next = std::max(0, static_cast<int>(m_caret) + inDelta);
            setCaret(static_cast<std::size_t>(next), inShouldSelect);

            return true;
        }

        std::size_t InputText::codepointCount() const
        {
            return value.toUnicode().size();
        }

        Vec2 InputText::toContentPoint(const Vec2& inLocation) const
        {
            const Mat3 inverse  = glm::inverse(static_cast<glm::mat3>(getPaintMatrix()));
            const Vec3 mapped   = inverse * Vec3(inLocation.x, inLocation.y, 1.0f);
            const Vec2 position = getDrawPosition();

            return Vec2(mapped.x - position.x - m_style.insetLeft(), mapped.y - position.y - m_style.insetTop());
        }

        std::size_t InputText::hitIndex(const Vec2& inLocation) const
        {
            return getInsertionIndexAt(toContentPoint(inLocation).x);
        }

        void InputText::selectAll()
        {
            m_anchor = 0;
            m_caret  = codepointCount();

            refreshChrome();
        }

        void InputText::collapseSelection()
        {
            m_anchor = m_caret;

            refreshChrome();
        }

        void InputText::deleteSelection()
        {
            if (!hasSelection())
            {
                return;
            }

            std::vector<char32_t> codes = value.toUnicode();
            const std::size_t     start = std::min(m_caret, m_anchor);
            const std::size_t     end   = std::max(m_caret, m_anchor);
            if (start >= codes.size())
            {
                m_caret  = codes.size();
                m_anchor = m_caret;

                return;
            }

            const std::size_t stop = std::min(end, codes.size());
            codes.erase(
                codes.begin() + static_cast<std::ptrdiff_t>(start),
                codes.begin() + static_cast<std::ptrdiff_t>(stop)
            );
            value    = fromCodepoints(codes);
            m_caret  = start;
            m_anchor = start;
        }

        bool InputText::beginSelect(const Vec2& inLocation, bool inShouldExtend)
        {
            m_bIsSelecting = true;
            setCaret(hitIndex(inLocation), inShouldExtend);

            return true;
        }

        bool InputText::updateSelect(const Vec2& inLocation)
        {
            setCaret(hitIndex(inLocation), true);

            return true;
        }
    }
}
