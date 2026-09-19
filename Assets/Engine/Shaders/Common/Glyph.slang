module Glyph;

public static const int GLYPH_HEADER_SIZE = 6;
public static const int GLYPH_CURVE_SIZE  = 6;

public float2 getGlyphBoundsMin(StructuredBuffer<float> outlines, int inOffset)
{
    return float2(outlines[inOffset], outlines[inOffset + 1]);
}

public float2 getGlyphBoundsMax(StructuredBuffer<float> outlines, int inOffset)
{
    return float2(outlines[inOffset + 2], outlines[inOffset + 3]);
}

public int getGlyphRootCode(float inStart, float inControl, float inEnd)
{
    int index = (inStart > 0.0 ? 2 : 0) + (inControl > 0.0 ? 4 : 0) + (inEnd > 0.0 ? 8 : 0);

    return (0x2E74 >> index) & 3;
}

public float getGlyphSegmentCoverage(float2 inStart, float2 inControl, float2 inEnd, float inInverseWidth)
{
    int code = getGlyphRootCode(inStart.y, inControl.y, inEnd.y);

    if (code == 0)
    {
        return 0.0;
    }

    float2 a = inStart - (2.0 * inControl) + inEnd;
    float2 b = inStart - inControl;
    float2 c = inStart;

    float first;
    float second;

    if (abs(a.y) > 1e-5)
    {
        float radicand = sqrt(max((b.y * b.y) - (a.y * c.y), 0.0));

        first  = (b.y - radicand) / a.y;
        second = (b.y + radicand) / a.y;
    }
    else
    {
        float root = c.y / (2.0 * b.y);

        first  = root;
        second = root;
    }

    float result = 0.0;

    if ((code & 1) != 0)
    {
        float x = (((a.x * first) - (2.0 * b.x)) * first) + c.x;

        result += saturate((x * inInverseWidth) + 0.5);
    }

    if (code > 1)
    {
        float x = (((a.x * second) - (2.0 * b.x)) * second) + c.x;

        result -= saturate((x * inInverseWidth) + 0.5);
    }

    return result;
}

public float getGlyphCoverage(StructuredBuffer<float> outlines, int inOffset, float2 inPosition, float2 inPixel)
{
    int count = int(outlines[inOffset + 4]);
    int base  = inOffset + GLYPH_HEADER_SIZE;

    float2 inverse = 1.0 / max(inPixel, float2(1e-6, 1e-6));

    float coverage = 0.0;

    for (int i = 0; i < count; ++i)
    {
        int index = base + (i * GLYPH_CURVE_SIZE);

        float2 start   = float2(outlines[index], outlines[index + 1]) - inPosition;
        float2 control = float2(outlines[index + 2], outlines[index + 3]) - inPosition;
        float2 end     = float2(outlines[index + 4], outlines[index + 5]) - inPosition;

        coverage += getGlyphSegmentCoverage(start, control, end, inverse.x);
        coverage -= getGlyphSegmentCoverage(start.yx, control.yx, end.yx, inverse.y);
    }

    return saturate(abs(coverage) * 0.5);
}
