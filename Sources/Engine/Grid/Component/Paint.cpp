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
        bool Component::escapesOverflow() const
        {
            return style.escape.get();
        }

        void Component::setEscapesOverflow(bool inValue)
        {
            if (style.escape.get() == inValue)
            {
                return;
            }

            style.escape.set(inValue);

            markPaintDirty();
        }

        void Component::markStyleDirty()
        {
            setFlag(ComponentDirty::Style);
        }

        void Component::markLayoutDirty()
        {
            setFlag(ComponentDirty::Layout);
            markPaintDirty();
        }

        void Component::markStyleDirtySubtree()
        {
            markStyleDirty();

            for (Component* child : m_children)
            {
                child->markStyleDirtySubtree();
            }
        }

        void Component::markLayoutDirtySubtree()
        {
            markLayoutDirty();

            for (Component* child : m_children)
            {
                child->markLayoutDirtySubtree();
            }
        }

        void Component::markPaintDirty()
        {
            if (hasFlag(ComponentDirty::Paint))
            {
                return;
            }

            setFlag(ComponentDirty::Paint);
            publishPaintDirty();
        }

        void Component::markPaintDirtySubtree()
        {
            const bool bWasDirty = hasFlag(ComponentDirty::Paint);
            setFlag(ComponentDirty::Paint);

            if (!bWasDirty)
            {
                publishPaintDirty();
            }

            if (!m_paintChildren.empty())
            {
                m_bDescendantPaintDirty = true;
            }

            for (Component* child : m_paintChildren)
            {
                if (child)
                {
                    child->sealPaintDirty();
                }
            }
        }

        bool Component::consumeCommandsDirty()
        {
            const bool bDirty = m_bCommandsDirty;
            m_bCommandsDirty  = false;

            return bDirty;
        }

        void Component::publishPaintDirty()
        {
            Component* root        = m_root ? m_root : this;
            root->m_bCommandsDirty = true;

            for (Component* ancestor = (!isRoot() && m_parent && m_parent != this) ? m_parent : nullptr; ancestor;
                 ancestor = (ancestor->isRoot() || ancestor->m_parent == ancestor) ? nullptr : ancestor->m_parent)
            {
                if (ancestor->m_bDescendantPaintDirty)
                {
                    break;
                }

                ancestor->m_bDescendantPaintDirty = true;
            }
        }

        void Component::sealPaintDirty()
        {
            setFlag(ComponentDirty::Paint);

            if (!m_paintChildren.empty())
            {
                m_bDescendantPaintDirty = true;
            }

            for (Component* child : m_paintChildren)
            {
                if (child)
                {
                    child->sealPaintDirty();
                }
            }
        }

        void Component::paint(const ComponentPaintContext& inContext)
        {
            const bool bSelf    = hasFlag(ComponentDirty::Paint);
            const bool bDescend = m_bDescendantPaintDirty;

            if (!bSelf && !bDescend)
            {
                return;
            }

            m_bDescendantPaintDirty = false;

            if (style.isDisplay(StyleDisplay::None))
            {
                setFlag(ComponentDirty::Paint, false);

                return;
            }

            if (bSelf)
            {
                setFlag(ComponentDirty::Paint, false);

                m_draw.position = getTranslation() - inContext.scroll;
                m_draw.opacity  = inContext.opacity * style.opacity.get();
                m_draw.depth    = isRoot() ? 0.0f : inContext.depth + style.zIndex.get() + 0.1f;

                const float blur  = style.filter.blur.get();
                m_draw.filterBlur = blur > 0.0f
                                        ? std::sqrt((inContext.filterBlur * inContext.filterBlur) + (blur * blur))
                                        : inContext.filterBlur;

                const StyleTransform xform = style.getTransform();
                m_draw.matrix = xform.isIdentity() ? inContext.world : inContext.world * computeLocalPaintMatrix(xform);

                Bounds2D box;
                box.set(
                    m_draw.position.y,
                    m_draw.position.x,
                    m_draw.position.y + m_size.y,
                    m_draw.position.x + m_size.x
                );
                box.transform(m_draw.matrix);
                m_draw.bounds = box;

                const bool bEscapes = escapesOverflow();
                m_draw.clip         = bEscapes ? Bounds2D::sUnconstrained() : inContext.clip;

                std::vector<const Component*> roundedAncestors;
                if (!bEscapes && inContext.roundedAncestors)
                {
                    roundedAncestors = *inContext.roundedAncestors;
                }

                paintRadius(roundedAncestors);
                paintRoundClips(roundedAncestors);
                setCulled(isCulledByAncestor());
            }

            if (!bDescend)
            {
                return;
            }

            const bool bEscapes = escapesOverflow();

            std::vector<const Component*> roundedAncestors;
            if (!bEscapes && inContext.roundedAncestors)
            {
                roundedAncestors = *inContext.roundedAncestors;
            }

            ComponentPaintContext context;
            context.world            = m_draw.matrix;
            context.clip             = m_draw.clip;
            context.scroll           = inContext.scroll + getScrollOffset();
            context.opacity          = m_draw.opacity;
            context.filterBlur       = m_draw.filterBlur;
            context.depth            = m_draw.depth;
            context.roundedAncestors = &roundedAncestors;

            if (!bEscapes && style.isClippingOverflow())
            {
                context.clip = context.clip.intersect(m_draw.bounds);
            }

            if (style.isClippingOverflow() && !style.radius.isZero())
            {
                roundedAncestors.push_back(this);
            }

            ComponentPaintContext pinned = context;
            pinned.scroll                = inContext.scroll;

            const Scrollable* scroller = dynamic_cast<const Scrollable*>(this);

            for (Component* child : m_paintChildren)
            {
                if (!child || child == this)
                {
                    continue;
                }

                if (!child->hasFlag(ComponentDirty::Paint) && !child->m_bDescendantPaintDirty)
                {
                    continue;
                }

                const bool bPinned = scroller && scroller->isScrollPinned(child);
                child->paint(bPinned ? pinned : context);
            }
        }

        void Component::paintRadius(const std::vector<const Component*>& inRoundedAncestors)
        {
            m_draw.radiusX = style.radius.horizontal();
            m_draw.radiusY = style.radius.vertical();

            for (auto it = inRoundedAncestors.rbegin(); it != inRoundedAncestors.rend(); it++)
            {
                const Component* ancestor = *it;
                const Style&     style    = ancestor->getStyle();

                const Bounds2D box     = ancestor->getDrawBounds();
                const Vec4     radiusX = style.radius.horizontal();
                const Vec4     radiusY = style.radius.vertical();
                const float    insetL  = style.padding.left.get() + style.border.paintedLeft();
                const float    insetR  = style.padding.right.get() + style.border.paintedRight();
                const float    insetT  = style.padding.top.get() + style.border.paintedTop();
                const float    insetB  = style.padding.bottom.get() + style.border.paintedBottom();
                const float eps = std::max(2.5f, std::max(std::max(insetL, insetR), std::max(insetT, insetB)) + 1.0f);

                const auto flush = [eps](float inLeft, float inRight) { return std::abs(inLeft - inRight) <= eps; };

                const bool flushLeft =
                    flush(m_draw.bounds.left, box.left) || flush(m_draw.bounds.left, box.left + insetL);
                const bool flushRight =
                    flush(m_draw.bounds.right, box.right) || flush(m_draw.bounds.right, box.right - insetR);
                const bool flushTop = flush(m_draw.bounds.top, box.top) || flush(m_draw.bounds.top, box.top + insetT);
                const bool flushBottom =
                    flush(m_draw.bounds.bottom, box.bottom) || flush(m_draw.bounds.bottom, box.bottom - insetB);

                if (flushTop && flushLeft)
                {
                    m_draw.radiusX.x = std::max(m_draw.radiusX.x, radiusX.x);
                    m_draw.radiusY.x = std::max(m_draw.radiusY.x, radiusY.x);
                }

                if (flushTop && flushRight)
                {
                    m_draw.radiusX.y = std::max(m_draw.radiusX.y, radiusX.y);
                    m_draw.radiusY.y = std::max(m_draw.radiusY.y, radiusY.y);
                }

                if (flushBottom && flushRight)
                {
                    m_draw.radiusX.z = std::max(m_draw.radiusX.z, radiusX.z);
                    m_draw.radiusY.z = std::max(m_draw.radiusY.z, radiusY.z);
                }

                if (flushBottom && flushLeft)
                {
                    m_draw.radiusX.w = std::max(m_draw.radiusX.w, radiusX.w);
                    m_draw.radiusY.w = std::max(m_draw.radiusY.w, radiusY.w);
                }
            }
        }

        void Component::paintRoundClips(const std::vector<const Component*>& inRoundedAncestors)
        {
            m_draw.innerClip        = Vec4::sSentinel();
            m_draw.innerClipRadiusX = Vec4::sZero();
            m_draw.innerClipRadiusY = Vec4::sZero();
            m_draw.outerClip        = Vec4::sSentinel();
            m_draw.outerClipRadiusX = Vec4::sZero();
            m_draw.outerClipRadiusY = Vec4::sZero();

            std::uint32_t filled = 0;

            for (auto it = inRoundedAncestors.rbegin(); it != inRoundedAncestors.rend() && filled < 2; it++)
            {
                const Component* ancestor = *it;
                const Bounds2D   box      = ancestor->getDrawBounds();
                const Vec4       radiusX  = ancestor->getStyle().radius.horizontal();
                const Vec4       radiusY  = ancestor->getStyle().radius.vertical();
                const Vec4       clip     = Vec4(box.left, box.top, box.right, box.bottom);

                const bool bFilledZero = static_cast<bool>(filled == 0);

                if (bFilledZero)
                {
                    m_draw.innerClip        = clip;
                    m_draw.innerClipRadiusX = radiusX;
                    m_draw.innerClipRadiusY = radiusY;
                }

                if (!bFilledZero)
                {
                    m_draw.outerClip        = clip;
                    m_draw.outerClipRadiusX = radiusX;
                    m_draw.outerClipRadiusY = radiusY;
                }

                filled++;
            }
        }

        Component* Component::getHitAt(const Vec2& inLocation) const
        {
            Component* hit = nullptr;

            std::vector<const Component*>        stack;
            std::unordered_set<const Component*> visited;
            stack.push_back(this);
            visited.insert(this);

            auto consider = [&](Component* candidate)
            {
                if (!candidate || !candidate->isDisplayable() || candidate->isCulled())
                {
                    return;
                }

                if (!candidate->getDrawBounds().contains(inLocation))
                {
                    return;
                }

                if (!candidate->containsPoint(inLocation))
                {
                    return;
                }

                if (!hit || candidate->getDepth() >= hit->getDepth())
                {
                    hit = candidate;
                }
            };

            while (!stack.empty())
            {
                const Component* node = stack.back();
                stack.pop_back();

                if (node != this && node->isCulled())
                {
                    continue;
                }

                const std::vector<Component*> children = node->m_paintChildren;
                for (auto it = children.rbegin(); it != children.rend(); it++)
                {
                    Component* child = *it;
                    if (!child || !child->isDisplayable() || !visited.insert(child).second)
                    {
                        continue;
                    }

                    stack.push_back(child);
                }

                if (node != this)
                {
                    consider(const_cast<Component*>(node));
                }
            }

            return hit;
        }

        bool Component::containsPoint(const Vec2& inLocation) const
        {
            if (!isDisplayable())
            {
                return false;
            }

            if (!getDrawBounds().contains(inLocation))
            {
                return false;
            }

            Vec2        local  = inLocation;
            const Mat3& matrix = getPaintMatrix();

            const bool bIdentity = std::fabs(matrix[0][0] - 1.0f) <= 0.0001f &&
                                   std::fabs(matrix[1][1] - 1.0f) <= 0.0001f &&
                                   std::fabs(matrix[2][2] - 1.0f) <= 0.0001f && std::fabs(matrix[0][1]) <= 0.0001f &&
                                   std::fabs(matrix[0][2]) <= 0.0001f && std::fabs(matrix[1][0]) <= 0.0001f &&
                                   std::fabs(matrix[1][2]) <= 0.0001f && std::fabs(matrix[2][0]) <= 0.0001f &&
                                   std::fabs(matrix[2][1]) <= 0.0001f;

            if (!bIdentity)
            {
                const Mat3 inverse = matrix.inverse();
                const Vec3 mapped  = inverse * Vec3(inLocation.x, inLocation.y, 1.0f);

                local.x = mapped.x;
                local.y = mapped.y;
            }

            const Vec2 position = getDrawPosition();
            Bounds2D   box;
            box.left   = position.x;
            box.top    = position.y;
            box.right  = position.x + m_size.x;
            box.bottom = position.y + m_size.y;

            if (!box.containsRounded(local, style.radius.horizontal(), style.radius.vertical()))
            {
                return false;
            }

            if (!getOverflowClip().contains(inLocation))
            {
                return false;
            }

            if (escapesOverflow())
            {
                return true;
            }

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                if (ancestor->getStyle().isClippingOverflow() && !ancestor->getDrawBounds().containsRounded(
                                                                     inLocation,
                                                                     ancestor->getStyle().radius.horizontal(),
                                                                     ancestor->getStyle().radius.vertical()
                                                                 ))
                {
                    return false;
                }

                if (ancestor->escapesOverflow() || ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return true;
        }

        Vec2 Component::getDrawPosition() const
        {
            const Scrollable* scroller = hasParent() ? dynamic_cast<const Scrollable*>(m_parent) : nullptr;
            if (!scroller || !scroller->isScrollPinned(this))
            {
                return m_draw.position;
            }

            Vec2 result = getPosition();

            const Component* ancestor = hasParent() ? m_parent->getParent() : nullptr;
            while (ancestor && ancestor != this)
            {
                const Vec2 scroll = ancestor->getScrollOffset();
                result.x -= scroll.x;
                result.y -= scroll.y;

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return result;
        }

        Vec2 Component::getTransformPivot() const
        {
            return getDrawPosition() + getStyle().getTransformOrigin(getBorderSize());
        }

        const Mat3& Component::getPaintMatrix() const
        {
            return m_draw.matrix;
        }

        Mat3 Component::computeLocalPaintMatrix(const StyleTransform& inTransform) const
        {
            const Vec2  origin  = getTransformPivot();
            const float radians = inTransform.rotation * Math::DEG_TO_RAD;
            const float cosine  = std::cos(radians);
            const float sine    = std::sin(radians);

            const Mat3 toOrigin(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -origin.x, -origin.y, 1.0f);
            const Mat3 fromOrigin(
                1.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                0.0f,
                origin.x + inTransform.translation.x,
                origin.y + inTransform.translation.y,
                1.0f
            );
            const Mat3 linear(
                cosine * inTransform.scale.x,
                sine * inTransform.scale.x,
                0.0f,
                -sine * inTransform.scale.y,
                cosine * inTransform.scale.y,
                0.0f,
                0.0f,
                0.0f,
                1.0f
            );

            return fromOrigin * linear * toOrigin;
        }

        const Bounds2D& Component::getBounds() const
        {
            return Transformable2D::getBounds();
        }

        const Bounds2D& Component::getDrawBounds() const
        {
            return m_draw.bounds;
        }

        const Bounds2D& Component::getOverflowClip() const
        {
            return m_draw.clip;
        }

        void Component::getOverflowRoundClips(
            Vec4& outFirst,
            Vec4& outFirstRadiusX,
            Vec4& outFirstRadiusY,
            Vec4& outSecond,
            Vec4& outSecondRadiusX,
            Vec4& outSecondRadiusY
        ) const
        {
            outFirst         = m_draw.innerClip;
            outFirstRadiusX  = m_draw.innerClipRadiusX;
            outFirstRadiusY  = m_draw.innerClipRadiusY;
            outSecond        = m_draw.outerClip;
            outSecondRadiusX = m_draw.outerClipRadiusX;
            outSecondRadiusY = m_draw.outerClipRadiusY;
        }

        void Component::getPaintRadius(Vec4& outRadiusX, Vec4& outRadiusY) const
        {
            outRadiusX = m_draw.radiusX;
            outRadiusY = m_draw.radiusY;
        }

        bool Component::hasPrimitive() const
        {
            return !m_primitive.isEmpty();
        }

        const Primitive& Component::getPrimitive() const
        {
            return m_primitive;
        }

        void Component::clearPrimitive()
        {
            m_primitive.clear();
        }

        void Component::setPrimitive(const Primitive& inPrimitive)
        {
            m_primitive = inPrimitive;
            m_primitive.seal();
        }

        bool Component::isCulledByAncestor() const
        {
            if (m_size.x <= 0.0f || m_size.y <= 0.0f)
            {
                return false;
            }

            const Bounds2D clip = getOverflowClip();
            if (clip.isEmpty())
            {
                return true;
            }

            Bounds2D    draw = getDrawBounds();
            const float pad  = std::max(m_size.x, m_size.y) + 64.0f;
            draw.left -= pad;
            draw.top -= pad;
            draw.right += pad;
            draw.bottom += pad;

            return !draw.overlaps(clip);
        }

        void Component::rebuildPaintChildren()
        {
            m_paintChildren.clear();
            m_paintChildren.reserve(m_children.size() + m_peripherals.size());

            auto append = [this](Component* node)
            {
                if (!node || node == this)
                {
                    return;
                }

                for (const Component* ancestor = this; ancestor != nullptr;
                     ancestor                  = ancestor->isRoot() ? nullptr : ancestor->m_parent)
                {
                    if (ancestor == node)
                    {
                        return;
                    }
                }

                for (Component* existing : m_paintChildren)
                {
                    if (existing == node)
                    {
                        return;
                    }
                }

                m_paintChildren.push_back(node);
            };

            for (Component* child : m_children)
            {
                append(child);
            }

            for (Component* peripheral : m_peripherals)
            {
                append(peripheral);
            }
        }
    }
}
