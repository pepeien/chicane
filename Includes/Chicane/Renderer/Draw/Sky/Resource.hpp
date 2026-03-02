#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawSkyResource : DrawResource<int, int>
        {
        public:
            bool isEmpty() const;

            Draw::Id findId(const Draw::Reference& inValue) const;

            const DrawSky& getDraw() const;

            Draw::Id add(const DrawSky& inData);

            void reset();

        private:
            DrawSky m_draw = {};
        };
    }
}