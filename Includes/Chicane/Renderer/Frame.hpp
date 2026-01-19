#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data/Data2D.hpp"
#include "Chicane/Renderer/Draw/Data/Data3D.hpp"
#include "Chicane/Renderer/Draw/Draw2D.hpp"
#include "Chicane/Renderer/Draw/Draw3D.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Frame
        {
        public:
            const View& getCamera() const;
            void useCamera(const View& inData);

            const View::List& getLights() const;
            void addLights(const View::List& inData);
            void addLight(const View& inData);

            const Draw2D::List& getDraws2D() const;
            const Vertex::List& getVertices2D() const;
            const Vertex::Indices& getIndices2D() const;
            void addDraw(const DrawData2D& inData);

            const Draw3D::List& getDraws3D() const;
            const Vertex::List& getVertices3D() const;
            const Vertex::Indices& getIndices3D() const;
            void addDraw(const DrawData3D& inData);

            void reset();

        protected:
            void resetCamera();
            void resetLights();

            Draw::Id findDraw(const DrawData2D& inData) const;
            void resetDraw2D();

            Draw::Id findDraw(const DrawData3D& inData) const;
            void resetDraw3D();

        protected:
            View            m_camera     = {};
            View::List      m_lights     = {};

            Draw2D::List    m_draws2D    = {};
            Vertex::List    m_vertices2D = {};
            Vertex::Indices m_indices2D  = {};

            Draw3D::List    m_draws3D    = {};
            Vertex::List    m_vertices3D = {};
            Vertex::Indices m_indices3D  = {};
        };
    }
}