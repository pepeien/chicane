#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Sky/Instance.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Frame
        {
        public:
            void reset();
            void setup(const DrawPolyResource::Map& inResources);
            void setup(const DrawSky& inResource);

            const View& getCamera() const;
            void useCamera(const View& inData);

            const View::List& getLights() const;
            void addLight(const View::List& inData);
            void addLight(const View& inData);

            bool hasDraws(DrawPolyType inType, DrawPolyMode inMode) const;
            DrawPoly::List getDraws(DrawPolyType inType, DrawPolyMode inMode) const;

            const DrawPoly2DInstance::List getInstances2D() const;
            void draw(Draw::Id inId, const DrawPoly2DInstance& inInstance);

            const DrawPoly3DInstance::List getInstances3D() const;
            void draw(Draw::Id inId, const DrawPoly3DInstance& inInstance);

            const DrawSkyInstance& getSkyInstance() const;

        protected:
            void resetCamera();
            void resetLights();

            void refresh2DDraws();
            void reset2DDraws();

            void refresh3DDraws();
            void reset3DDraws();

        protected:
            // View
            View                    m_camera = {};
            View::List              m_lights = {};

            // Poly
            DrawPoly::Map           m_polys = {};

            // Instances
            DrawPoly2DInstance::Map m_2DInstances = {};
            DrawPoly3DInstance::Map m_3DInstances = {};
            DrawSkyInstance         m_skyInstance = {};
        };
    }
}