#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/Dock/Side.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Dock;
        class DockHandle;
        struct DockRegion;

        CH_TYPE(Manual)
        class CHICANE_GRID DockPanel : public Scrollable
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "DockPanel";

            // Properties
            static constexpr inline const char* SIDE_ATTRIBUTE_NAME         = "side";
            static constexpr inline const char* SIZE_ATTRIBUTE_NAME         = "size";
            static constexpr inline const char* MIN_SIZE_ATTRIBUTE_NAME     = "min-size";
            static constexpr inline const char* MAX_SIZE_ATTRIBUTE_NAME     = "max-size";
            static constexpr inline const char* IS_RESIZABLE_ATTRIBUTE_NAME = "isResizable";
            static constexpr inline const char* IS_GRABBABLE_ATTRIBUTE_NAME = "isGrabbable";
            static constexpr inline const char* HANDLE_ATTRIBUTE_NAME       = "handle";

            static constexpr inline const char* SIDE_TYPE_FILL   = "fill";
            static constexpr inline const char* SIDE_TYPE_CENTER = "center";
            static constexpr inline const char* SIDE_TYPE_FLOAT  = "float";
            static constexpr inline const char* SIDE_TYPE_LEFT   = "left";
            static constexpr inline const char* SIDE_TYPE_RIGHT  = "right";
            static constexpr inline const char* SIDE_TYPE_TOP    = "top";
            static constexpr inline const char* SIDE_TYPE_BOTTOM = "bottom";

            static constexpr inline const char* DEFAULT_SIZE     = "25%";
            static constexpr inline const char* DEFAULT_MIN_SIZE = "4em";

            static DockPanel* findFrom(Component* inComponent);

        public:
            CH_CONSTRUCTOR()
            DockPanel(const pugi::xml_node& inNode);

            ~DockPanel() override;

        public:
            std::vector<Component*> getChildrenFlat() const override;

        protected:
            void onRefresh() override;
            void refreshSize() override;
            void refreshPosition() override;

        public:
            DockSide getSide() const;
            void setSide(DockSide inSide);
            bool isFill() const;
            bool isFloating() const;
            bool isResizable() const;
            bool isGrabbable() const;
            void setGrabbable(bool inValue);
            bool hasAssignedHandle() const;
            bool isAssignedHandle(const Component* inComponent) const;

            const String& getHandleId() const;
            Component* findAssignedHandle() const;

            const String& getSizeValue() const;
            const String& getMinSizeValue() const;
            const String& getMaxSizeValue() const;

            bool hasExtent() const;
            float getExtent() const;
            void setExtent(float inValue);
            void clearExtent();

            const Vec2& getFloatPosition() const;
            void setFloatPosition(const Vec2& inValue);
            const Vec2& getFloatSize() const;
            void setFloatSize(const Vec2& inValue);

            DockHandle* getHandle() const;

        private:
            friend class Dock;

            void refreshAttributes();
            void refreshHandle();
            const DockRegion* region() const;
            DockSide parseSide(const String& inValue) const;
            String toSideString(DockSide inSide) const;
            bool parseFlag(const String& inValue, bool inFallback) const;

        private:
            DockSide    m_side;
            String      m_size;
            String      m_minSize;
            String      m_maxSize;
            String      m_handleId;
            bool        m_bResizable;
            bool        m_bGrabbable;
            float       m_extent;
            Vec2        m_floatPosition;
            Vec2        m_floatSize;
            DockHandle* m_handle;
        };
    }
}
