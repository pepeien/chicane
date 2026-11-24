#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

#include "Chicane/Grid/Component/LayoutMetrics.hpp"
#include "Chicane/Grid/Component/Loading.hpp"
#include "Chicane/Grid/Component/Projection.hpp"
#include "Chicane/Grid/Component/Scope.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/StyleMatch.hpp"
#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Component/Script.hpp"

namespace Chicane
{
    namespace Grid
    {
        bool Component::canPlayAnimation() const
        {
            return isDisplayable();
        }

        bool Component::isDrawable() const
        {
            return isDisplayable() && isVisible() && isSolid() && hasPrimitive() && !isCulled();
        }

        bool Component::isFocusable() const
        {
            return getAttribute(FOCUSABLE_ATTRIBUTE_NAME).equals("true", "1");
        }

        bool Component::onEvent(const WindowEvent&)
        {
            return false;
        }

        bool Component::isVisible() const
        {
            const bool bIsBackgroundImageVisible    = !style.background.image.getRaw().isEmpty();
            const bool bIsBackgroundColorVisible    = style.background.color.get().a > 0.0f;
            const bool bIsBackgroundGradientVisible = StyleGradient::sIsActive(style.background.gradients);
            const bool bIsBackdropVisible           = style.backdrop.blur.get() > 0.0f;
            const bool bIsBorderVisible             = style.border.isVisible();

            return (bIsBackgroundImageVisible || bIsBackgroundColorVisible || bIsBackgroundGradientVisible ||
                    bIsBackdropVisible || bIsBorderVisible) &&
                   getOpacity() > 0.0f;
        }

        String Component::getClassName() const
        {
            return classList.toString();
        }

        void Component::setClassName(const String& inValue)
        {
            classList = inValue;
        }

        void Component::addClassName(const String& inValue)
        {
            classList.add(inValue);
        }

        bool Component::isDisplayable() const
        {
            if (!isRoot())
            {
                if (!m_parent || !m_parent->isDisplayable())
                {
                    return false;
                }
            }

            return !style.isDisplay(StyleDisplay::None) && !style.isDisplay(StyleDisplay::Hidden);
        }

        bool Component::isSolid() const
        {
            return m_size.x > 0.0f && m_size.y > 0.0f;
        }

        bool Component::isHovered() const
        {
            return hasStatus(ComponentStatus::Hovered);
        }

        bool Component::isFocused() const
        {
            return hasStatus(ComponentStatus::Focused);
        }

        bool Component::isDragging() const
        {
            return hasStatus(ComponentStatus::Dragging);
        }

        bool Component::isCulled() const
        {
            return hasStatus(ComponentStatus::Culled);
        }

        bool Component::isSelected() const
        {
            return hasStatus(ComponentStatus::Selected);
        }

        ComponentStatus Component::getStatus() const
        {
            return m_status;
        }

        bool Component::hasStatus(ComponentStatus inStatus) const
        {
            return has(m_status, inStatus);
        }

        bool Component::hasFlag(ComponentDirty inFlag) const
        {
            return has(m_flags, inFlag);
        }

        void Component::setFlag(ComponentDirty inFlag, bool inEnabled)
        {
            const bool bHasEnabled = static_cast<bool>(inEnabled);

            if (bHasEnabled)
            {
                m_flags |= inFlag;
            }

            if (!bHasEnabled)
            {
                m_flags &= ~inFlag;
            }
        }

        void Component::hover()
        {
            onHover();

            getMethod(getAttribute(ON_HOVER_ATTRIBUTE_NAME)).invoke();
        }

        void Component::leave()
        {
            onLeave();

            getMethod(getAttribute(ON_LEAVE_ATTRIBUTE_NAME)).invoke();
        }

        void Component::click()
        {
            onClick();

            getMethod(getAttribute(ON_CLICK_ATTRIBUTE_NAME)).invoke();
        }

        void Component::focus()
        {
            onFocus();

            getMethod(getAttribute(ON_FOCUS_ATTRIBUTE_NAME)).invoke();
        }

        void Component::blur()
        {
            onBlur();

            getMethod(getAttribute(ON_BLUR_ATTRIBUTE_NAME)).invoke();
        }

        void Component::drag()
        {
            onDrag();

            getMethod(getAttribute(ON_DRAG_ATTRIBUTE_NAME)).invoke();
        }

        void Component::endDrag()
        {
            onDragEnd();

            getMethod(getAttribute(ON_DRAG_END_ATTRIBUTE_NAME)).invoke();
        }

        void Component::setHovered(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isHovered() == inValue)
            {
                return;
            }

            const bool bHasValue = static_cast<bool>(inValue);

            if (bHasValue)
            {
                m_status |= ComponentStatus::Hovered;
            }

            if (!bHasValue)
            {
                m_status &= ~ComponentStatus::Hovered;
            }

            const bool bInvalidateSubtree = static_cast<bool>(bShouldInvalidateSubtree);

            if (bInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }

            if (!bInvalidateSubtree)
            {
                markStyleDirty();
            }

            if (inValue)
            {
                hover();

                return;
            }

            leave();
        }

        void Component::setFocused(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isFocused() == inValue)
            {
                return;
            }

            const bool bHasValue = static_cast<bool>(inValue);

            if (bHasValue)
            {
                m_status |= ComponentStatus::Focused;
            }

            if (!bHasValue)
            {
                m_status &= ~ComponentStatus::Focused;
            }

            const bool bInvalidateSubtree = static_cast<bool>(bShouldInvalidateSubtree);

            if (bInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }

            if (!bInvalidateSubtree)
            {
                markStyleDirty();
            }

            if (inValue)
            {
                focus();

                return;
            }

            blur();
        }

        void Component::setDragging(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isDragging() == inValue)
            {
                return;
            }

            const bool bHasValue = static_cast<bool>(inValue);

            if (bHasValue)
            {
                m_status |= ComponentStatus::Dragging;
            }

            if (!bHasValue)
            {
                m_status &= ~ComponentStatus::Dragging;
            }

            const bool bInvalidateSubtree = static_cast<bool>(bShouldInvalidateSubtree);

            if (bInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }

            if (!bInvalidateSubtree)
            {
                markStyleDirty();
            }

            if (inValue)
            {
                drag();

                return;
            }

            endDrag();
        }

        void Component::setSelected(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isSelected() == inValue)
            {
                return;
            }

            const bool bHasValue = static_cast<bool>(inValue);

            if (bHasValue)
            {
                m_status |= ComponentStatus::Selected;
            }

            if (!bHasValue)
            {
                m_status &= ~ComponentStatus::Selected;
            }

            const bool bInvalidateSubtree = static_cast<bool>(bShouldInvalidateSubtree);

            if (bInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }

            if (!bInvalidateSubtree)
            {
                markStyleDirty();
            }
        }

        void Component::setCulled(bool inValue)
        {
            if (isCulled() == inValue)
            {
                return;
            }

            const bool bHasValue = static_cast<bool>(inValue);

            if (bHasValue)
            {
                m_status |= ComponentStatus::Culled;
            }

            if (!bHasValue)
            {
                m_status &= ~ComponentStatus::Culled;
            }
        }

        ClassList& Component::getClassList()
        {
            return classList;
        }

        const ClassList& Component::getClassList() const
        {
            return classList;
        }

        void Component::setClassList(const ClassList& inValue)
        {
            classList = inValue;
        }

        void Component::onClassListChanged()
        {
            const auto found = m_attributes.find(CLASS_ATTRIBUTE_NAME);
            if (found == m_attributes.end() || !isReference(found->second))
            {
                m_attributes[CLASS_ATTRIBUTE_NAME] = classList.toString();
            }

            markStyleDirtySubtree();
            markLayoutDirtySubtree();
        }
    }
}
