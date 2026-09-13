#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawSkyResource : public DrawResource<int, int>
        {
        public:
            bool isEmpty() const;

            Draw::Id findId(const Draw::Reference& inValue) const;

            const DrawSky& getDraw() const;

            Draw::Id add(const DrawSky& inData);

            void reset();

        public:
            float exposure = 1.0f;

        private:
            DrawSky m_draw = {};
        };
    }
}