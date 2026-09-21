#include "Editor/UI/Component/ViewportSettings.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Runtime/Application.hpp>

#include "Editor/UI/Prop.hpp"
#include "Editor/Viewport/Overlay.hpp"

namespace Editor
{
    static Chicane::String formatFloat(float inValue)
    {
        return Chicane::String::sprint("%.3f", inValue);
    }

    static Chicane::String formatUint(float inValue)
    {
        return Chicane::String::sprint("%u", static_cast<unsigned int>(std::max(inValue, 1.0f)));
    }

    ViewportSettings::ViewportSettings(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          isVisible(false),
          bShowFill(true),
          bShowLit(true),
          bShowHdr(false),
          bGridEnabled(true),
          bGridAxisX(true),
          bGridAxisY(true),
          bGridAxisZ(true),
          axisXState("on"),
          axisYState("on"),
          axisZState("on"),
          gridColor(Chicane::Vec3::Zero()),
          gridScale(0.5f),
          gridDivisions(10.0f),
          gridScaleText(Chicane::String::empty()),
          gridDivisionsText(Chicane::String::empty()),
          bShowWireframe(false),
          bShowBounds(false),
          geometryColor(Chicane::Vec4::Zero()),
          bShowOutline(true),
          outlinerColor(Chicane::Vec3::Zero()),
          bShowCollider(false),
          physicsColor(Chicane::Vec4::Zero()),
          bShowBones(false),
          boneColor(Chicane::Vec3::Zero()),
          bShowMotionPaths(false)
    {
        const ViewportOverlay& overlay = ViewportOverlay::get();
        bGridEnabled    = overlay.bGridEnabled;
        bGridAxisX      = overlay.bGridAxisX;
        bGridAxisY      = overlay.bGridAxisY;
        bGridAxisZ      = overlay.bGridAxisZ;
        gridColor       = overlay.gridColor;
        gridScale       = overlay.gridScale;
        gridDivisions   = overlay.gridDivisions;
        geometryColor   = overlay.geometryColor;
        outlinerColor   = overlay.outlinerColor;
        physicsColor    = overlay.physicsColor;
        boneColor       = overlay.boneColor;

        load(
            "Assets/Editor/UI/Components/ViewportSettings.grid",
            "Assets/Editor/UI/Components/ViewportSettings.decal"
        );

        Prop::bind(this, IS_OPEN_ATTRIBUTE, isVisible);
        refreshFieldText();
        refreshAxisState();
    }

    bool ViewportSettings::escapesOverflow() const
    {
        return true;
    }

    bool ViewportSettings::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (!isVisible || inEvent.type != Chicane::WindowEventType::MouseButtonUp || !inEvent.data)
        {
            return false;
        }

        const Chicane::Input::MouseButtonEvent event =
            *static_cast<const Chicane::Input::MouseButtonEvent*>(inEvent.data);
        if (event.button != Chicane::Input::MouseButton::Left)
        {
            return false;
        }

        Chicane::Grid::Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
        for (Chicane::Grid::Component* node = hit; node != nullptr; node = node->getParent())
        {
            if (node == this || node->getId().equals(ANCHOR_ID))
            {
                return false;
            }

            if (node->isRoot())
            {
                break;
            }
        }

        close();

        return false;
    }

    void ViewportSettings::onTick(float inDeltaTime)
    {
        Prop::copy(this, IS_OPEN_ATTRIBUTE, isVisible);
        Chicane::Grid::Container::onTick(inDeltaTime);
        syncFlags();
        applyOverlay();
        refreshAxisState();
    }

    void ViewportSettings::refreshPosition()
    {
        Chicane::Grid::Container::refreshPosition();

        Chicane::Grid::Component* anchor = findAnchor();
        if (!anchor)
        {
            return;
        }

        const Chicane::Vec2 current = getPosition();
        const Chicane::Vec2 target(anchor->getPosition().x, anchor->getPosition().y + anchor->getSize().y + 6.0f);

        addPosition(target.x - current.x, target.y - current.y);
    }

    Chicane::Grid::Component* ViewportSettings::findAnchor() const
    {
        for (Chicane::Grid::Component* origin = getParent(); origin != nullptr; origin = origin->getParent())
        {
            if (origin->getId().equals(ANCHOR_ID))
            {
                return origin;
            }

            for (Chicane::Grid::Component* child : origin->getChildren())
            {
                if (child && child->getId().equals(ANCHOR_ID))
                {
                    return child;
                }
            }

            if (origin->isRoot())
            {
                break;
            }
        }

        return nullptr;
    }

    void ViewportSettings::close()
    {
        isVisible = false;
        Prop::invoke(this, ON_CLOSE_ATTRIBUTE);
    }

    void ViewportSettings::onFillInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Fill, bShowFill);
    }

    void ViewportSettings::onLitInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Light, bShowLit);
    }

    void ViewportSettings::onHdrInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::HDR, bShowHdr);
    }

    void ViewportSettings::onGridInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onAxisX()
    {
        bGridAxisX = !bGridAxisX;
        refreshAxisState();
        applyOverlay();
    }

    void ViewportSettings::onAxisY()
    {
        bGridAxisY = !bGridAxisY;
        refreshAxisState();
        applyOverlay();
    }

    void ViewportSettings::onAxisZ()
    {
        bGridAxisZ = !bGridAxisZ;
        refreshAxisState();
        applyOverlay();
    }

    void ViewportSettings::onGridScaleInput()
    {
        gridScale = std::max(gridScale, 0.001f);
        applyOverlay();
    }

    void ViewportSettings::onGridDivisionsInput()
    {
        gridDivisions = std::max(gridDivisions, 1.0f);
        applyOverlay();
    }

    void ViewportSettings::onGridColorInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onWireframeInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Wireframe, bShowWireframe);
    }

    void ViewportSettings::onBoundsInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Bounds, bShowBounds);
    }

    void ViewportSettings::onGeometryColorInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onOutlineInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Outline, bShowOutline);
    }

    void ViewportSettings::onOutlinerColorInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onColliderInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Colliders, bShowCollider);
    }

    void ViewportSettings::onPhysicsColorInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onBonesInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Skeletons, bShowBones);
    }

    void ViewportSettings::onBonesColorInput()
    {
        applyOverlay();
    }

    void ViewportSettings::onMotionPathsInput()
    {
        setFeature(Chicane::Renderer::RendererFeature::Traces, bShowMotionPaths);
    }

    Chicane::Renderer::Instance* ViewportSettings::getRenderer() const
    {
        return Chicane::Application::getInstance().getRenderer();
    }

    void ViewportSettings::syncFlags()
    {
        const Chicane::Renderer::Instance* renderer = getRenderer();

        bShowFill        = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Fill);
        bShowLit         = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Light);
        bShowHdr         = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::HDR);
        bShowWireframe   = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Wireframe);
        bShowBounds      = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Bounds);
        bShowOutline     = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Outline);
        bShowCollider    = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Colliders);
        bShowBones       = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Skeletons);
        bShowMotionPaths = renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Traces);
    }

    void ViewportSettings::applyOverlay()
    {
        ViewportOverlay& overlay = ViewportOverlay::get();
        overlay.bGridEnabled     = bGridEnabled;
        overlay.bGridAxisX       = bGridAxisX;
        overlay.bGridAxisY       = bGridAxisY;
        overlay.bGridAxisZ       = bGridAxisZ;
        overlay.gridColor        = gridColor;
        overlay.gridScale        = std::max(gridScale, 0.001f);
        overlay.gridDivisions    = std::max(gridDivisions, 1.0f);
        overlay.geometryColor    = geometryColor;
        overlay.outlinerColor    = outlinerColor;
        overlay.physicsColor     = physicsColor;
        overlay.boneColor        = boneColor;
    }

    void ViewportSettings::setFeature(Chicane::Renderer::RendererFeature inFeature, bool inEnabled)
    {
        Chicane::Renderer::Instance* renderer = getRenderer();
        if (!renderer)
        {
            return;
        }

        if (inEnabled)
        {
            renderer->enableFeature(inFeature);
        }
        else
        {
            renderer->disableFeature(inFeature);
        }
    }

    void ViewportSettings::refreshFieldText()
    {
        gridScaleText         = formatFloat(gridScale);
        gridDivisionsText     = formatUint(gridDivisions);
    }

    void ViewportSettings::refreshAxisState()
    {
        axisXState = bGridAxisX ? "on" : "off";
        axisYState = bGridAxisY ? "on" : "off";
        axisZState = bGridAxisZ ? "on" : "off";
    }
}
