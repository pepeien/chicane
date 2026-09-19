import Constant;
import Target;
import Poly2D;
import Glyph;
import Rounded;
import Filter;
import Texture;
import Tonemap;

[vk::binding(0, 0)] StructuredBuffer<PolyInstance2D> gComponents;
[vk::binding(1, 0)] StructuredBuffer<float> gOutlines;
[vk::binding(2, 0)] Sampler2D gBackdrop;
[vk::binding(3, 0)] Sampler2D gScreenTarget;

struct ScreenPush
{
    int4 data;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<ScreenPush> screen;

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
}

float2 getUITextureUV(int textureIndex, float2 uv)
{
    float2 result = saturate(uv);

#ifdef CHICANE_VULKAN
    if (textureIndex == screen.data.x)
    {
        result.y = 1.0 - result.y;
    }
#endif

    return result;
}

float3 sampleUITexture(int textureIndex, float2 uv)
{
    float2 texUV = getUITextureUV(textureIndex, uv);
    if (textureIndex == screen.data.x)
    {
        float3 color = gScreenTarget.Sample(texUV).rgb;
        return screen.data.y != 0 ? acesTonemap(color) : saturate(color);
    }

    return sampleTextureSlot(textureIndex, texUV).rgb;
}

[shader("fragment")]
float4 main(VSOut inData, float4 fragCoord : SV_Position) : SV_Target
{
    float4 color = inData.color;

    if (inData.glyphIndex < 0 && inData.textureIndex < 0 && inData.normal.x < 0.5)
    {
        PolyInstance2D component = gComponents[inData.instanceIndex];

        if (component.gradientType > 0)
        {
            color = samplePoly2DGradient(component, inData.uv);
        }
    }

    int offset       = max(inData.glyphIndex, 0);
    float2 minBounds = getGlyphBoundsMin(gOutlines, offset) - inData.glyphDilation;
    float2 maxBounds = getGlyphBoundsMax(gOutlines, offset) + inData.glyphDilation;
    float2 position  = lerp(minBounds, maxBounds, inData.uv);
    float2 pixel     = fwidth(position) * (1.0 + max(inData.view.z, 0.0));

    if (inData.glyphIndex >= 0)
    {
        color.a *= getGlyphCoverage(gOutlines, inData.glyphIndex, position, pixel);
    }
    else if (inData.glyphIndex < 0 && inData.normal.x >= 0.5)
    {
        float fw       = max(fwidth(inData.uv.x), 1e-5);
        float coverage = 1.0;

        if (inData.normal.x >= 1.5)
        {
            coverage = saturate(0.5 + ((1.0 - inData.uv.x) / fw));
        }
        else
        {
            coverage = saturate(0.5 + (inData.uv.x / fw)) * saturate(0.5 + ((1.0 - inData.uv.x) / fw));
        }

        color.a *= coverage;
    }
    else if (inData.textureIndex >= 0)
    {
        float3 rgb = sampleUITexture(inData.textureIndex, inData.uv);

        if (inData.view.z >= 0.5)
        {
            float2 texel = float2(inData.view.z, inData.view.z) / max(inData.box.zw - inData.box.xy, float2(1.0, 1.0));
            float3 acc   = float3(0.0, 0.0, 0.0);
            float wsum   = 0.0;

            for (int y = -1; y <= 1; y++)
            {
                for (int x = -1; x <= 1; x++)
                {
                    float2 tap = inData.uv + (float2(float(x), float(y)) * texel);
                    float w    = exp(-0.5 * float((x * x) + (y * y)) / 2.0);

                    acc  += sampleUITexture(inData.textureIndex, tap) * w;
                    wsum += w;
                }
            }

            rgb = acc / max(wsum, 1e-4);
        }

        color.rgb = rgb;
    }

    float2 frag      = uiClipPosition(fragCoord.xy, inData.view.xy);
    float2 localFrag = frag;
    {
        float2 center = lerp(inData.box.xy, inData.box.zw, 0.5);
        float2x2 linear = float2x2(inData.linear.xy, inData.linear.zw);
        float det     = (linear[0][0] * linear[1][1]) - (linear[0][1] * linear[1][0]);

        if (abs(det) >= 1e-6)
        {
            localFrag = center + mul(inverse2x2(linear), (frag - center));
        }
    }

    if (frag.x < inData.clip.x || frag.y < inData.clip.y || frag.x > inData.clip.z || frag.y > inData.clip.w)
    {
        discard;
    }

    if (hasRoundedCorners(inData.innerClipRadiusX, inData.innerClipRadiusY) &&
        !insideRoundedRect(frag, inData.innerClip, inData.innerClipRadiusX, inData.innerClipRadiusY))
    {
        discard;
    }

    if (hasRoundedCorners(inData.outerClipRadiusX, inData.outerClipRadiusY) &&
        !insideRoundedRect(frag, inData.outerClip, inData.outerClipRadiusX, inData.outerClipRadiusY))
    {
        discard;
    }

    if (inData.glyphIndex < 0 && inData.normal.x < 0.5)
    {
        float coverage = 1.0;

        if (inData.view.z > 0.0)
        {
            coverage = filterGaussianCoverage(roundedRectSDF(localFrag, inData.box, inData.radiusX, inData.radiusY), inData.view.z);
        }
        else if (hasRoundedCorners(inData.radiusX, inData.radiusY))
        {
            coverage = roundedRectCoverage(localFrag, inData.box, inData.radiusX, inData.radiusY);
        }

        color = applyRoundedBorder(
            color,
            localFrag,
            inData.box,
            inData.radiusX,
            inData.radiusY,
            inData.borderWidth,
            inData.borderColorTop,
            inData.borderColorRight,
            inData.borderColorBottom,
            inData.borderColorLeft
        );

        if (inData.view.w > 0.0)
        {
            float3 backdropColor = sampleBackdropBlur(gBackdrop, fragCoord.xy, inData.view.xy, inData.view.w);

            color.rgb = (color.rgb * color.a) + (backdropColor * (1.0 - color.a));
            color.a   = coverage;
        }
        else
        {
            color.a *= coverage;
        }
    }

    return color;
}
