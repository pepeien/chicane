#pragma once

#include <vector>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Mesh.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command/Poly.hpp"
#include "Chicane/Renderer/Draw/Sky/Data.hpp"
#include "Chicane/Renderer/Light.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DCommand
        {
        public:
            using List = std::vector<DrawPoly3DCommand>;

        public:
            void clear()
            {
                camera = {};
                sky    = {};

                lights.clear();
                meshes.clear();
                polys.clear();
            }

        public:
            // View
            View                        camera = {};
            Light::List                 lights = {};

            // Sky
            Renderer::DrawSkyData       sky = {};

            // Mesh
            DrawPoly3DCommandMesh::List meshes = {};

            // Immediate
            DrawPoly3DCommandPoly::List polys = {};
        };
    }
}
