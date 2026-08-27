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
            static constexpr inline const char* TAG_ID = "Svg";

            // Attributes
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME   = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME  = "height";
            static constexpr inline const char* VIEWBOX_ATTRIBUTE_NAME = "viewBox";

        public:
            CH_CONSTRUCTOR()
            Svg(const pugi::xml_node& inNode);

            ~Svg() override;

        public:
            void refresh() override;
            std::vector<Component*> getChildrenFlat() const override;

        protected:
            void onRefresh() override;
            void refreshSize() override;

        private:
            void applySizeAttributes();
            void rebuildShapes();
            void syncShapes();
            SvgShape* acquireShape(std::size_t inIndex);

        private:
            Vec2                   m_intrinsic;
            SvgViewBox             m_viewBox;
            String                 m_signature;
            std::vector<SvgShape*> m_shapes;
        };
    }
}
