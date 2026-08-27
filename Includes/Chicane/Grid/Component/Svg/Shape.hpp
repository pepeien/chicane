#pragma once

#include "Chicane/Core/Color.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Primitive.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID SvgShape : public Component
        {
        public:
            static constexpr inline const char* TAG_ID = "SvgShape";

        public:
            SvgShape();

        public:
            bool isDrawable() const override;

        protected:
            void refreshStyleRuleset() override;

        public:
            void configure(const Primitive& inPrimitive, const Color::Rgba& inColor);
            void sync(const Component* inParent, float inScale);
            void clear();
        };
    }
}
