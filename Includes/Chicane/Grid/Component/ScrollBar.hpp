#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID ScrollBar : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "ScrollBar";

        public:
            ScrollBar();

        public:
            bool isDrawable() const override;
            float getDepth() const override;
            Vec2 getDrawPosition() const override;

        public:
            void configure(const Bounds2D& inBox, const Color::Rgba& inColor, float inDepthBias);
            void hide();

        private:
            float m_depthBias;
        };
    }
}
