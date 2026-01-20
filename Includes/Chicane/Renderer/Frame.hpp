#pragma once

#include <unordered_map>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Thread/Pool.hpp"
#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Bundle.hpp"
#include "Chicane/Renderer/Draw/2D/Data.hpp"
#include "Chicane/Renderer/Draw/3D/Data.hpp"
#include "Chicane/Renderer/Draw/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/3D/Instance.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Frame
        {
        public:
            // Lifecycle
            void reset();

            const View& getCamera() const;
            void useCamera(const View& inData);

            const View::List& getLights() const;
            void addLights(const View::List& inData);
            void addLight(const View& inData);

            const DrawBundle<Draw2DInstance>::DrawList& getDraws2D() const;
            const Vertex::List& getVertices2D() const;
            const Vertex::Indices& getIndices2D() const;
            const DrawBundle<Draw2DInstance>::DrawInstances getInstances2D() const;
            void addDraw(const DrawData2D& inData);

            const DrawBundle<Draw3DInstance>::DrawList& getDraws3D() const;
            const Vertex::List& getVertices3D() const;
            const Vertex::Indices& getIndices3D() const;
            const DrawBundle<Draw3DInstance>::DrawInstances getInstances3D() const;
            void addDraw(const DrawData3D& inData);

        protected:
            void resetCamera();
            void resetLights();

            void resetDraw2D();
            void resetDraw3D();

        protected:
            View                       m_camera  = {};
            View::List                 m_lights  = {};

            DrawBundle<Draw2DInstance> m_draws2D = {};
            DrawBundle<Draw3DInstance> m_draws3D = {};
        };
    }
}