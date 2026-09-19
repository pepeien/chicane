import Target;
import Poly2D;

[vk::binding(0, 0)] StructuredBuffer<PolyInstance2D> gComponents;

struct VSOut
{
    nointerpolation int textureIndex : TEXCOORD0;
    nointerpolation int glyphIndex : TEXCOORD1;
    nointerpolation int instanceIndex : TEXCOORD2;
    nointerpolation float glyphDilation : TEXCOORD3;
    nointerpolation float4 clip : TEXCOORD4;
    nointerpolation float4 view : TEXCOORD5;
    nointerpolation float4 box : TEXCOORD6;
    nointerpolation float4 linear : TEXCOORD7;
    nointerpolation float4 radiusX : TEXCOORD8;
    nointerpolation float4 radiusY : TEXCOORD9;
    nointerpolation float4 innerClip : TEXCOORD10;
    nointerpolation float4 innerClipRadiusX : TEXCOORD11;
    nointerpolation float4 innerClipRadiusY : TEXCOORD12;
    nointerpolation float4 outerClip : TEXCOORD13;
    nointerpolation float4 outerClipRadiusX : TEXCOORD14;
    nointerpolation float4 outerClipRadiusY : TEXCOORD15;
    nointerpolation float4 borderWidth : TEXCOORD16;
    nointerpolation float4 borderColorTop : TEXCOORD17;
    nointerpolation float4 borderColorRight : TEXCOORD18;
    nointerpolation float4 borderColorBottom : TEXCOORD19;
    nointerpolation float4 borderColorLeft : TEXCOORD20;
    float2 uv : TEXCOORD21;
    float3 normal : TEXCOORD22;
    float4 color : COLOR;
    float4 position : SV_Position;
}

[shader("vertex")]
VSOut main(
    [vk::location(0)] float3 inPosition : POSITION,
    [vk::location(2)] float2 inUV : TEXCOORD0,
    [vk::location(3)] float3 inNormal : NORMAL,
    uint instanceId : SV_InstanceID
)
{
    PolyInstance2D component = gComponents[instanceId];

    float2 originalScale = component.scale;
    float2 blurredScale  = get2DBlurredScale(component);
    component.scale      = blurredScale;

    float2 vertexPosition = get2DVertexPosition(component, inPosition.xy, uiVertexMultiplier());
    float3 screenPosition = get2DScreenPosition(component, uiScreenMultiplier());

    VSOut output;
    output.textureIndex     = component.textureIndex;
    output.glyphIndex       = component.glyphIndex;
    output.instanceIndex    = int(instanceId);
    output.glyphDilation    = component.glyphDilation;
    output.clip             = component.clip;
    output.view             = float4(component.view, component.filterBlur, component.backdropBlur);
    output.box              = float4(component.position.xy, component.position.xy + component.size);
    output.linear           = float4(component.transformX, component.transformY);
    output.radiusX          = component.radiusX;
    output.radiusY          = component.radiusY;
    output.innerClip        = component.innerClip;
    output.innerClipRadiusX = component.innerClipRadiusX;
    output.innerClipRadiusY = component.innerClipRadiusY;
    output.outerClip        = component.outerClip;
    output.outerClipRadiusX = component.outerClipRadiusX;
    output.outerClipRadiusY = component.outerClipRadiusY;
    output.borderWidth      = component.borderWidth;
    output.borderColorTop   = normalize2DColor(component.borderColorTop);
    output.borderColorRight = normalize2DColor(component.borderColorRight);
    output.borderColorBottom = normalize2DColor(component.borderColorBottom);
    output.borderColorLeft  = normalize2DColor(component.borderColorLeft);
    output.uv               = inNormal.x >= 0.5 ? inUV : get2DFilterUV(inUV, originalScale, blurredScale);
    output.normal           = inNormal;
    output.color            = normalize2DColor(component.color);
    output.position         = float4(vertexPosition + screenPosition.xy, screenPosition.z, 1.0);

    return output;
}
