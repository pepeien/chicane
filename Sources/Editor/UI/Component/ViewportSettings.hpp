#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Math/Vec/Vec4.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Renderer/Feature.hpp>
#include <Chicane/Renderer/Instance.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::ViewportSettings))
    class ViewportSettings : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* IS_OPEN_ATTRIBUTE  = "isOpen";
        static constexpr inline const char* ON_CLOSE_ATTRIBUTE = "onClose";
        static constexpr inline const char* ANCHOR_ID          = "toolbarSettings";

    public:
        CH_CONSTRUCTOR()
        ViewportSettings(const Chicane::XmlNode& inNode);

    public:
        bool escapesOverflow() const override;
        bool onEvent(const Chicane::WindowEvent& inEvent) override;

    protected:
        void onTick(float inDeltaTime) override;
        void refreshPosition() override;

    public:
        CH_FUNCTION()
        void close();

        CH_FUNCTION()
        void onFillInput();

        CH_FUNCTION()
        void onLitInput();

        CH_FUNCTION()
        void onHdrInput();

        CH_FUNCTION()
        void onGridInput();

        CH_FUNCTION()
        void onAxisX();

        CH_FUNCTION()
        void onAxisY();

        CH_FUNCTION()
        void onAxisZ();

        CH_FUNCTION()
        void onGridScaleInput();

        CH_FUNCTION()
        void onGridDivisionsInput();

        CH_FUNCTION()
        void onGridColorInput();

        CH_FUNCTION()
        void onWireframeInput();

        CH_FUNCTION()
        void onBoundsInput();

        CH_FUNCTION()
        void onGeometryColorInput();

        CH_FUNCTION()
        void onOutlineInput();

        CH_FUNCTION()
        void onOutlinerColorInput();

        CH_FUNCTION()
        void onColliderInput();

        CH_FUNCTION()
        void onPhysicsColorInput();

        CH_FUNCTION()
        void onBonesInput();

        CH_FUNCTION()
        void onBonesColorInput();

        CH_FUNCTION()
        void onTracerInput();

        CH_FUNCTION()
        void onTracerColorInput();

    private:
        Chicane::Renderer::Instance* getRenderer() const;
        void syncFlags();
        void applyOverlay();
        void setFeature(Chicane::Renderer::RendererFeature inFeature, bool inEnabled);
        void refreshFieldText();
        void refreshAxisState();
        Chicane::Grid::Component* findAnchor() const;

    public:
        CH_FIELD()
        bool isVisible;

        CH_FIELD()
        bool bShowFill;
        CH_FIELD()
        bool bShowLit;
        CH_FIELD()
        bool bShowHdr;

        CH_FIELD()
        bool bGridEnabled;
        CH_FIELD()
        bool bGridAxisX;
        CH_FIELD()
        bool bGridAxisY;
        CH_FIELD()
        bool bGridAxisZ;
        CH_FIELD()
        Chicane::String axisXState;
        CH_FIELD()
        Chicane::String axisYState;
        CH_FIELD()
        Chicane::String axisZState;
        CH_FIELD()
        Chicane::Vec3 gridColor;
        CH_FIELD()
        float gridScale;
        CH_FIELD()
        float gridDivisions;
        CH_FIELD()
        Chicane::String gridScaleText;
        CH_FIELD()
        Chicane::String gridDivisionsText;

        CH_FIELD()
        bool bShowWireframe;
        CH_FIELD()
        bool bShowBounds;
        CH_FIELD()
        Chicane::Vec4 geometryColor;
        CH_FIELD()
        bool bShowOutline;
        CH_FIELD()
        Chicane::Vec3 outlinerColor;

        CH_FIELD()
        bool bShowCollider;
        CH_FIELD()
        Chicane::Vec4 physicsColor;

        CH_FIELD()
        bool bShowBones;
        CH_FIELD()
        Chicane::Vec3 boneColor;
        CH_FIELD()
        bool bShowTracer;
        CH_FIELD()
        Chicane::Vec4 tracerColor;
    };
}
