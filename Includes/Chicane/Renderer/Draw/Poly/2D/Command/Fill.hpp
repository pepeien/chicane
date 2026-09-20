#pragma once

#include <memory>
#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly2DCommandFill
        {
        public:
            using List = std::vector<DrawPoly2DCommandFill>;

        public:
            // Geometry
            Draw::Reference                          polygon  = Draw::InvalidReference;
            std::shared_ptr<const Vertex::List>      vertices = nullptr;
            std::shared_ptr<const Vertex::Indices>   indices  = nullptr;

            // Texture
            Draw::Reference                          texture     = Draw::InvalidReference;
            bool                                     bHasTexture = false;

            // Glyph
            Draw::Reference                          glyph        = Draw::InvalidReference;
            std::shared_ptr<const std::vector<Vec2>> glyphOutline = nullptr;
            Vec2                                     glyphMin     = Vec2::Zero();
            Vec2                                     glyphMax     = Vec2::Zero();

            // Instance
            DrawPoly2DInstance                       instance = {};
        };
    }
}
