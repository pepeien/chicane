#pragma once

#include <unordered_map>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Thread/Pool.hpp"
#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/3D/Instance.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"
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

            // Properties
            const View& getCamera() const;
            void useCamera(const View& inData);

            const View::List& getLights() const;
            void addLights(const View::List& inData);
            void addLight(const View& inData);

            const Draw2DInstance::List getInstances2D() const;
            const std::uint32_t getInstance2DStart(Draw::Id inId) const;
            const std::uint32_t getInstance2DCount(Draw::Id inId) const;
            void use(Draw::Id inId, const Draw2DInstance& inInstance);

            const Draw3DInstance::List getInstances3D() const;
            const std::uint32_t getInstance3DStart(Draw::Id inId) const;
            const std::uint32_t getInstance3DCount(Draw::Id inId) const;
            void use(Draw::Id inId, const Draw3DInstance& inInstance);

        protected:
            void resetCamera();
            void resetLights();

            void resetDraw2D();
            void resetDraw3D();

        protected:
            View                m_camera  = {};
            View::List          m_lights  = {};

            Draw2DInstance::Map m_draws2D = {};
            Draw3DInstance::Map m_draws3D = {};
        };
    }
}