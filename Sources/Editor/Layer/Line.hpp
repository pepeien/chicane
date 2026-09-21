#pragma once

#include <Chicane/Core/Math/Vec/Vec4.hpp>
#include <Chicane/Renderer/Layer/Scene/Line.hpp>

#include "Editor/Viewport/Overlay.hpp"

namespace Editor
{
    class LLine : public Chicane::Renderer::LSceneLine
    {
    protected:
        Chicane::Vec4 getWireframeColor() const override { return ViewportOverlay::getInstance().geometryColor; }

        Chicane::Vec4 getOutlineColor() const override
        {
            return Chicane::Vec4(ViewportOverlay::getInstance().outlinerColor, 1.0f);
        }
    };
}
