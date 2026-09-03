#include "Chicane/Grid/Component/Dock/Panel.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Grid/Component/Dock.hpp"
#include "Chicane/Grid/Component/Dock/Handle.hpp"
#include "Chicane/Grid/Component/Dock/Region.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace
        {
            const Color::Rgba kHandleColor(0, 0, 0, 70);
        }

        DockPanel::DockPanel(const pugi::xml_node& inNode)
            : Scrollable(inNode),
              m_side(DockSide::Fill),
              m_size(String::empty()),
              m_minSize(String::empty()),
              m_maxSize(String::empty()),
              m_handleId(String::empty()),
              m_bResizable(true),
              m_bGrabbable(true),
              m_extent(-1.0f),
              m_floatPosition(Vec2::Zero()),
              m_floatSize(Vec2::Zero()),
              m_handle(new DockHandle())
        {
            refreshAttributes();

            m_handle->setParent(this);
        }

        DockPanel::~DockPanel()
        {
            delete m_handle;
            m_handle = nullptr;
        }

        DockSide DockPanel::getSide() const
        {
            return m_side;
        }

        void DockPanel::setSide(DockSide inSide)
        {
            m_side                            = inSide;
            m_attributes[SIDE_ATTRIBUTE_NAME] = toSideString(inSide);
        }

        bool DockPanel::isFill() const
        {
            return m_side == DockSide::Fill;
        }

        bool DockPanel::isFloating() const
        {
            return m_side == DockSide::Float;
        }

        bool DockPanel::isResizable() const
        {
            return m_bResizable && !isFill() && !isFloating();
        }

        bool DockPanel::isGrabbable() const
        {
            return m_bGrabbable;
        }

        void DockPanel::setGrabbable(bool inValue)
        {
            m_bGrabbable                              = inValue;
            m_attributes[IS_GRABBABLE_ATTRIBUTE_NAME] = inValue ? "true" : "false";

            refreshHandle();
        }

        const String& DockPanel::getHandleId() const
        {
            return m_handleId;
        }

        Component* DockPanel::findAssignedHandle() const
        {
            if (m_handleId.isEmpty())
            {
                return nullptr;
            }

            if (getId().equals(m_handleId))
            {
                return const_cast<DockPanel*>(this);
            }

            for (Component* child : Component::getChildrenFlat())
            {
                if (child && child->getId().equals(m_handleId))
                {
                    return child;
                }
            }

            return nullptr;
        }

        const String& DockPanel::getSizeValue() const
        {
            return m_size;
        }

        const String& DockPanel::getMinSizeValue() const
        {
            return m_minSize;
        }

        const String& DockPanel::getMaxSizeValue() const
        {
            return m_maxSize;
        }

        bool DockPanel::hasExtent() const
        {
            return m_extent >= 0.0f;
        }

        float DockPanel::getExtent() const
        {
            return m_extent;
        }

        void DockPanel::setExtent(float inValue)
        {
            m_extent = std::max(0.0f, inValue);
        }

        void DockPanel::clearExtent()
        {
            m_extent = -1.0f;
        }

        const Vec2& DockPanel::getFloatPosition() const
        {
            return m_floatPosition;
        }

        void DockPanel::setFloatPosition(const Vec2& inValue)
        {
            m_floatPosition = inValue;
        }

        const Vec2& DockPanel::getFloatSize() const
        {
            return m_floatSize;
        }

        void DockPanel::setFloatSize(const Vec2& inValue)
        {
            m_floatSize.x = std::max(0.0f, inValue.x);
            m_floatSize.y = std::max(0.0f, inValue.y);
        }

        DockHandle* DockPanel::getHandle() const
        {
            return m_handle;
        }

        bool DockPanel::hasAssignedHandle() const
        {
            return findAssignedHandle() != nullptr;
        }

        bool DockPanel::isAssignedHandle(const Component* inComponent) const
        {
            if (!inComponent || m_handleId.isEmpty())
            {
                return false;
            }

            return inComponent->getId().equals(m_handleId);
        }

        DockPanel* DockPanel::findFrom(Component* inComponent)
        {
            Component* node = inComponent;
            while (node)
            {
                if (node->getTag().equals(TAG_ID))
                {
                    return static_cast<DockPanel*>(node);
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return nullptr;
        }

        std::vector<Component*> DockPanel::getChildrenFlat() const
        {
            std::vector<Component*> result = Component::getChildrenFlat();

            if (m_handle)
            {
                result.push_back(m_handle);
            }

            return result;
        }

        void DockPanel::onRefresh()
        {
            refreshAttributes();
        }

        void DockPanel::refreshSize()
        {
            if (const DockRegion* slot = region())
            {
                setSize(
                    std::max(0.0f, slot->box.right - slot->box.left),
                    std::max(0.0f, slot->box.bottom - slot->box.top)
                );

                return;
            }

            Component::refreshSize();
        }

        void DockPanel::refreshPosition()
        {
            const DockRegion* slot = region();
            if (!slot || !hasParent())
            {
                Component::refreshPosition();
                refreshHandle();

                return;
            }

            const Style& parentStyle = m_parent->getStyle();
            setPosition(
                m_parent->getPosition().x + parentStyle.insetLeft() + slot->box.left,
                m_parent->getPosition().y + parentStyle.insetTop() + slot->box.top
            );
            addCursor(m_style.insetLeft(), m_style.insetTop());

            m_style.zIndex.set(isFloating() ? 100.0f : 0.0f);

            refreshHandle();
        }

        const DockRegion* DockPanel::region() const
        {
            const Dock* dock =
                static_cast<const Dock*>(hasParent() && m_parent->getTag().equals(Dock::TAG_ID) ? m_parent : nullptr);

            return dock ? dock->findRegion(this) : nullptr;
        }

        void DockPanel::refreshHandle()
        {
            if (!m_handle)
            {
                return;
            }

            m_handle->setRoot(m_root);
            m_handle->setParent(this);
            m_handle->setStyleFile(m_styleFile);

            const Vec2& size = getSize();
            if (!isDisplayable() || !isGrabbable() || size.x <= 0.0f || size.y <= 0.0f || hasAssignedHandle())
            {
                m_handle->hide();

                return;
            }

            Bounds2D box;
            box.set(0.0f, 0.0f, std::min(Dock::HANDLE_THICKNESS, size.y), size.x);
            m_handle->configure(box, kHandleColor);
        }

        void DockPanel::refreshAttributes()
        {
            m_side       = parseSide(parseText(getAttribute(SIDE_ATTRIBUTE_NAME)).trim());
            m_size       = parseText(getAttribute(SIZE_ATTRIBUTE_NAME)).trim();
            m_minSize    = parseText(getAttribute(MIN_SIZE_ATTRIBUTE_NAME)).trim();
            m_maxSize    = parseText(getAttribute(MAX_SIZE_ATTRIBUTE_NAME)).trim();
            m_handleId    = parseText(getAttribute(HANDLE_ATTRIBUTE_NAME)).trim();
            m_bResizable  = parseFlag(getAttribute(IS_RESIZABLE_ATTRIBUTE_NAME), !isFill() && !isFloating());
            m_bGrabbable  = parseFlag(getAttribute(IS_GRABBABLE_ATTRIBUTE_NAME), true);
        }

        DockSide DockPanel::parseSide(const String& inValue) const
        {
            const String value = inValue.toLower();

            if (value.equals(SIDE_TYPE_LEFT))
            {
                return DockSide::Left;
            }

            if (value.equals(SIDE_TYPE_RIGHT))
            {
                return DockSide::Right;
            }

            if (value.equals(SIDE_TYPE_TOP))
            {
                return DockSide::Top;
            }

            if (value.equals(SIDE_TYPE_BOTTOM))
            {
                return DockSide::Bottom;
            }

            if (value.equals(SIDE_TYPE_FLOAT))
            {
                return DockSide::Float;
            }

            return DockSide::Fill;
        }

        String DockPanel::toSideString(DockSide inSide) const
        {
            switch (inSide)
            {
            case DockSide::Left:
                return SIDE_TYPE_LEFT;

            case DockSide::Right:
                return SIDE_TYPE_RIGHT;

            case DockSide::Top:
                return SIDE_TYPE_TOP;

            case DockSide::Bottom:
                return SIDE_TYPE_BOTTOM;

            case DockSide::Float:
                return SIDE_TYPE_FLOAT;

            default:
                return SIDE_TYPE_FILL;
            }
        }

        bool DockPanel::parseFlag(const String& inValue, bool inFallback) const
        {
            const String value = parseText(inValue).trim().toLower();
            if (value.isEmpty())
            {
                return inFallback;
            }

            if (value.equals("true", "1", "yes"))
            {
                return true;
            }

            if (value.equals("false", "0", "no"))
            {
                return false;
            }

            return inFallback;
        }
    }
}
