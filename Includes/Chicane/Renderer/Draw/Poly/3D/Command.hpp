#pragma once

#include <vector>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Mesh.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"
#include "Chicane/Renderer/Draw/Sky/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DCommand
        {
        public:
            using List = std::vector<DrawPoly3DCommand>;

        public:
            DrawPoly3DCommandType       type = DrawPoly3DCommandType::Undefined;

            // View
            View                        camera = {};
            View                        light  = {};

            // Sky
            Renderer::DrawSkyData       sky = {};

            // Mesh
            DrawPoly3DCommandMesh::List meshes = {};
        };
    }
}