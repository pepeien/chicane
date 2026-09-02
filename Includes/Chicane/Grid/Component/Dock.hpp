#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Size/Direction.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/Dock/Drag.hpp"
#include "Chicane/Grid/Component/Dock/Panel.hpp"
#include "Chicane/Grid/Component/Dock/Region.hpp"
#include "Chicane/Grid/Component/Dock/Resize.hpp"
#include "Chicane/Grid/Component/Dock/Side.hpp"

namespace Chicane
{
    namespace Grid
    {
        class DockDrop;
        class DockSplitter;

        CH_TYPE(Manual)
        class CHICANE_GRID Dock : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Dock";

            // Properties
            static constexpr inline float       SPLITTER_THICKNESS = 2.0f;
            static constexpr inline float       HANDLE_THICKNESS   = 10.0f;
            static constexpr inline float       DRAG_THRESHOLD     = 6.0f;
            static constexpr inline float       DROP_BAND_MIN      = 36.0f;
            static constexpr inline float       DROP_BAND_RATIO    = 0.22f;

        public:
            CH_CONSTRUCTOR()
            Dock(const pugi::xml_node& inNode);

            ~Dock() override;

        public:
            bool onEvent(const WindowEvent& inEvent) override;
            void tick(float inDeltaTime) override;
            std::vector<Component*> getChildrenFlat() const override;

            const DockRegion* findRegion(const DockPanel* inPanel) const;

        protected:
            void refreshSize() override;

        private:
            DockPanel* asPanel(Component* inComponent) const;
            void collectPanels(
                std::vector<DockPanel*>& outEdges, std::vector<DockPanel*>& outFills, std::vector<DockPanel*>& outFloats
            ) const;

            void layout();
            void claim(DockPanel* inPanel, Bounds2D& outRemaining, const Vec2& inContent);
            void assignFill(const std::vector<DockPanel*>& inPanels, const Bounds2D& inRemaining);
            void assignFloat(DockPanel* inPanel);
            void refreshSplitters();
            void refreshDrop();
            void clearSplitters();

            bool hitSplitter(const Vec2& inLocation, DockPanel*& outPanel) const;
            bool hitHandle(const Vec2& inLocation, DockPanel*& outPanel) const;
            Bounds2D getSplitterBounds(const DockRegion& inRegion) const;
            Bounds2D getContentBounds() const;
            Bounds2D getDropPreview(DockSide inSide) const;

            void beginResize(DockPanel* inPanel, const Vec2& inLocation);
            void updateResize(const Vec2& inLocation);
            void endResize();
            void applyResizeCursor(DockSide inSide);

            void beginDrag(DockPanel* inPanel, const Vec2& inLocation);
            void updateDrag(const Vec2& inLocation);
            void endDrag();
            void applyDragCursor();
            void clearCursor();
            void raise(DockPanel* inPanel);
            DockSide pickDropSide(const Vec2& inLocation) const;

            float resolveExtent(DockPanel* inPanel, const Bounds2D& inRemaining, const Vec2& inContent) const;
            float resolveLimit(
                const String& inValue, SizeDirection inDirection, const Vec2& inContent, float inFallback
            ) const;
            float parseExtent(const String& inValue, SizeDirection inDirection, const Vec2& inContent) const;
            SizeDirection axisOf(DockSide inSide) const;
            bool isHorizontal(DockSide inSide) const;

        private:
            std::unordered_map<const DockPanel*, DockRegion> m_regions;
            std::vector<DockSplitter*>                       m_splitters;
            DockDrop*                                        m_drop;
            DockResize                                       m_resize;
            DockDrag                                         m_drag;
        };
    }
}
