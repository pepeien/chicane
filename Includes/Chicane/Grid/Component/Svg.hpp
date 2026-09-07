#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/Svg/Shape.hpp"
#include "Chicane/Grid/Component/Svg/ViewBox.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Svg : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID           = "Svg";
            static constexpr inline const char* SVG_TAG          = "svg";
            static constexpr inline const char* GROUP_TAG        = "g";
            static constexpr inline const char* GROUP_ALIAS      = "group";
            static constexpr inline const char* ANCHOR_TAG       = "a";
            static constexpr inline const char* PATH_TAG         = "path";
            static constexpr inline const char* CIRCLE_TAG       = "circle";
            static constexpr inline const char* ELLIPSE_TAG      = "ellipse";
            static constexpr inline const char* RECT_TAG         = "rect";
            static constexpr inline const char* LINE_TAG         = "line";
            static constexpr inline const char* POLYLINE_TAG     = "polyline";
            static constexpr inline const char* POLYGON_TAG      = "polygon";
            static constexpr inline const char* NAMESPACE_PREFIX = "svg";

            // Attributes
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME      = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME     = "height";
            static constexpr inline const char* VIEWBOX_ATTRIBUTE_NAME    = "viewBox";
            static constexpr inline const char* D_ATTRIBUTE_NAME          = "d";
            static constexpr inline const char* CX_ATTRIBUTE_NAME         = "cx";
            static constexpr inline const char* CY_ATTRIBUTE_NAME         = "cy";
            static constexpr inline const char* R_ATTRIBUTE_NAME          = "r";
            static constexpr inline const char* RX_ATTRIBUTE_NAME         = "rx";
            static constexpr inline const char* RY_ATTRIBUTE_NAME         = "ry";
            static constexpr inline const char* X_ATTRIBUTE_NAME          = "x";
            static constexpr inline const char* Y_ATTRIBUTE_NAME          = "y";
            static constexpr inline const char* X1_ATTRIBUTE_NAME         = "x1";
            static constexpr inline const char* Y1_ATTRIBUTE_NAME         = "y1";
            static constexpr inline const char* X2_ATTRIBUTE_NAME         = "x2";
            static constexpr inline const char* Y2_ATTRIBUTE_NAME         = "y2";
            static constexpr inline const char* POINTS_ATTRIBUTE_NAME     = "points";
            static constexpr inline const char* DISPLAY_ATTRIBUTE_NAME    = "display";
            static constexpr inline const char* VISIBILITY_ATTRIBUTE_NAME = "visibility";
            static constexpr inline const char* DISPLAY_NONE              = "none";
            static constexpr inline const char* VISIBILITY_HIDDEN         = "hidden";

            static constexpr const char*        TRANSFORM_MATRIX    = "matrix";
            static constexpr const char*        TRANSFORM_TRANSLATE = "translate";
            static constexpr const char*        TRANSFORM_SCALE     = "scale";
            static constexpr const char*        TRANSFORM_ROTATE    = "rotate";
            static constexpr const char*        TRANSFORM_SKEW_X    = "skewx";
            static constexpr const char*        TRANSFORM_SKEW_Y    = "skewy";

            // Values
            static constexpr inline float       UNSYNCED_SCALE          = -1.0f;
            static constexpr inline int         NAMESPACE_PREFIX_LENGTH = 3;

            static constexpr int                BEZIER_SEGMENTS           = 8;
            static constexpr int                MATRIX_PARAM_COUNT        = 6;
            static constexpr int                ROTATE_ORIGIN_PARAM_COUNT = 3;
            static constexpr int                MIN_CONTOUR_POINTS        = 2;
            static constexpr int                MIN_CLOSED_POINTS         = 3;
            static constexpr float              MIN_LENGTH                = 1.0e-5f;
            static constexpr float              KAPPA                     = 0.5522847498f;
            static constexpr float              HALF                      = 0.5f;
            static constexpr float              PI                        = static_cast<float>(M_PI);
            static constexpr float              TWO_PI                    = PI * 2.0f;
            static constexpr float              HALF_PI                   = PI * HALF;
            static constexpr float              DEG_TO_RAD                = PI / 180.0f;
            static constexpr float              CHANNEL_MAX               = 255.0f;
            static constexpr float              CONTROL_REFLECT           = 2.0f;
            static constexpr float              ARC_CUBIC_OFFSET          = 4.0f;
            static constexpr float              ARC_CUBIC_SCALE           = 3.0f;
            static constexpr float              HOMOGENEOUS               = 1.0f;

        public:
            CH_CONSTRUCTOR()
            Svg(const pugi::xml_node& inNode);

            ~Svg() override;

        public:
            void refresh() override;
            std::vector<Component*> getChildrenFlat() const override;
            void invalidateDrawCacheSubtree() override;

        protected:
            void onRefresh() override;
            void refreshSize() override;
            void refreshPosition() override;

        private:
            void applySizeAttributes();
            void rebuildShapes();
            void syncShapes();
            SvgShape* acquireShape(std::size_t inIndex);

        private:
            Vec2                   m_intrinsic;
            SvgViewBox             m_viewBox;
            String                 m_signature;
            Vec2                   m_syncedSize;
            Vec2                   m_syncedPosition;
            float                  m_syncedScale;
            std::vector<SvgShape*> m_shapes;
        };
    }
}
