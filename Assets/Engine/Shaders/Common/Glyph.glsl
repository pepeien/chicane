// Analytic glyph coverage, after Eric Lengyel's "GPU-Centered Font Rendering Directly from Glyph Outlines" (Slug).
//
// A glyph is stored as a flat run of floats: a header holding the outline bounds and the segment count, followed by
// the quadratic Bezier segments themselves. Coverage at a pixel is resolved by casting a ray along each axis and
// accumulating the signed crossings, band limited by the pixel footprint so the result is already antialiased.
//
// Every shader that includes this file must first declare the outline buffer:
//   readonly buffer Glyph { float outlines[]; } glyph;

#define GLYPH_HEADER_SIZE 6
#define GLYPH_CURVE_SIZE  6

vec2 getGlyphBoundsMin(int inOffset) {
    return vec2(glyph.outlines[inOffset], glyph.outlines[inOffset + 1]);
}

vec2 getGlyphBoundsMax(int inOffset) {
    return vec2(glyph.outlines[inOffset + 2], glyph.outlines[inOffset + 3]);
}

// Tells which of the two roots actually cross the ray, from the signs of the three control point ordinates.
// The constant is an eight entry, two bit wide lookup table.
int getGlyphRootCode(float inStart, float inControl, float inEnd) {
    int index = (inStart > 0.0 ? 2 : 0) + (inControl > 0.0 ? 4 : 0) + (inEnd > 0.0 ? 8 : 0);

    return (0x2E74 >> index) & 3;
}

// Signed coverage contributed by one segment to a ray cast along +x, with the control points already made
// relative to the pixel being shaded.
float getGlyphSegmentCoverage(vec2 inStart, vec2 inControl, vec2 inEnd, float inInverseWidth) {
    int code = getGlyphRootCode(inStart.y, inControl.y, inEnd.y);

    if (code == 0) {
        return 0.0;
    }

    vec2 a = inStart - (2.0 * inControl) + inEnd;
    vec2 b = inStart - inControl;
    vec2 c = inStart;

    float first;
    float second;

    if (abs(a.y) > 1e-5) {
        float radicand = sqrt(max((b.y * b.y) - (a.y * c.y), 0.0));

        first  = (b.y - radicand) / a.y;
        second = (b.y + radicand) / a.y;
    } else {
        // The segment degenerated into a straight edge, so the quadratic collapses to a single linear root
        float root = c.y / (2.0 * b.y);

        first  = root;
        second = root;
    }

    float result = 0.0;

    if ((code & 1) != 0) {
        float x = (((a.x * first) - (2.0 * b.x)) * first) + c.x;

        result += clamp((x * inInverseWidth) + 0.5, 0.0, 1.0);
    }

    if (code > 1) {
        float x = (((a.x * second) - (2.0 * b.x)) * second) + c.x;

        result -= clamp((x * inInverseWidth) + 0.5, 0.0, 1.0);
    }

    return result;
}

// inPosition is the shaded point in em units, inPixel the em footprint of one pixel along each axis
float getGlyphCoverage(int inOffset, vec2 inPosition, vec2 inPixel) {
    int count = int(glyph.outlines[inOffset + 4]);
    int base  = inOffset + GLYPH_HEADER_SIZE;

    vec2 inverse = 1.0 / max(inPixel, vec2(1e-6));

    // Rays along +x and +y are accumulated together, with opposite signs because swapping the axes also
    // flips the winding. Averaging them cancels the artifacts either ray produces on its own.
    float coverage = 0.0;

    for (int i = 0; i < count; ++i) {
        int index = base + (i * GLYPH_CURVE_SIZE);

        vec2 start   = vec2(glyph.outlines[index],     glyph.outlines[index + 1]) - inPosition;
        vec2 control = vec2(glyph.outlines[index + 2], glyph.outlines[index + 3]) - inPosition;
        vec2 end     = vec2(glyph.outlines[index + 4], glyph.outlines[index + 5]) - inPosition;

        coverage += getGlyphSegmentCoverage(start, control, end, inverse.x);
        coverage -= getGlyphSegmentCoverage(start.yx, control.yx, end.yx, inverse.y);
    }

    return clamp(abs(coverage) * 0.5, 0.0, 1.0);
}
