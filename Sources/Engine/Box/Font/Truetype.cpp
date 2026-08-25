#include "Chicane/Box/Font/TrueType.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H
#include FT_MULTIPLE_MASTERS_H

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Font/Glyph/Curve/Collector.hpp"

#include "Chicane/Core/Math/Contour.hpp"
#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace FontTrueType
        {
            std::vector<Curve> parseGlyphContours(FT_GlyphSlot inGlyph)
            {
                FT_Outline_Funcs funcs;
                funcs.shift   = 0;
                funcs.delta   = 0;
                funcs.move_to = [](const FT_Vector* inPoint, void* inData)
                {
                    std::vector<Curve>* contours = static_cast<std::vector<Curve>*>(inData);

                    Curve curve;
                    curve.setSegmentCount(8);
                    curve.addPoint(Vec2(inPoint->x, inPoint->y));

                    contours->push_back(curve);

                    return 0;
                };
                funcs.line_to = [](const FT_Vector* inPoint, void* inData)
                {
                    std::vector<Curve>* contours = static_cast<std::vector<Curve>*>(inData);

                    if (contours->empty())
                    {
                        return 0;
                    }

                    contours->back().addPoint(Vec2(inPoint->x, inPoint->y));

                    return 0;
                };
                funcs.conic_to = [](const FT_Vector* inControl, const FT_Vector* inPoint, void* inData)
                {
                    std::vector<Curve>* contours = static_cast<std::vector<Curve>*>(inData);

                    if (contours->empty())
                    {
                        return 0;
                    }

                    contours->back().addQuadraticPoint(Vec2(inControl->x, inControl->y), Vec2(inPoint->x, inPoint->y));

                    return 0;
                };
                funcs.cubic_to =
                    [](const FT_Vector* inControlA, const FT_Vector* inControlB, const FT_Vector* inPoint, void* inData)
                {
                    std::vector<Curve>* contours = static_cast<std::vector<Curve>*>(inData);

                    if (contours->empty())
                    {
                        return 0;
                    }

                    contours->back().addBezierPoint(
                        Vec2(inControlA->x, inControlA->y),
                        Vec2(inControlB->x, inControlB->y),
                        Vec2(inPoint->x, inPoint->y)
                    );

                    return 0;
                };

                std::vector<Curve> result;

                FT_Outline_Decompose(&inGlyph->outline, &funcs, &result);

                return result;
            }

            FontGlyphCurve::List parseGlyphCurves(FT_GlyphSlot inGlyph)
            {
                FT_Outline_Funcs funcs;
                funcs.shift   = 0;
                funcs.delta   = 0;
                funcs.move_to = [](const FT_Vector* inPoint, void* inData)
                {
                    static_cast<FontGlyphCurveCollector*>(inData)->moveTo(Vec2(inPoint->x, inPoint->y));

                    return 0;
                };
                funcs.line_to = [](const FT_Vector* inPoint, void* inData)
                {
                    static_cast<FontGlyphCurveCollector*>(inData)->lineTo(Vec2(inPoint->x, inPoint->y));

                    return 0;
                };
                funcs.conic_to = [](const FT_Vector* inControl, const FT_Vector* inPoint, void* inData)
                {
                    static_cast<FontGlyphCurveCollector*>(inData)->quadraticTo(
                        Vec2(inControl->x, inControl->y),
                        Vec2(inPoint->x, inPoint->y)
                    );

                    return 0;
                };
                funcs.cubic_to =
                    [](const FT_Vector* inControlA, const FT_Vector* inControlB, const FT_Vector* inPoint, void* inData)
                {
                    static_cast<FontGlyphCurveCollector*>(inData)->cubicTo(
                        Vec2(inControlA->x, inControlA->y),
                        Vec2(inControlB->x, inControlB->y),
                        Vec2(inPoint->x, inPoint->y)
                    );

                    return 0;
                };

                FontGlyphCurveCollector collector;

                FT_Outline_Decompose(&inGlyph->outline, &funcs, &collector);

                collector.close();

                return collector.curves;
            }

            FontGlyph parseGlyph(std::uint32_t inCode, FT_GlyphSlot inGlyph)
            {
                Contour contour;
                contour.triangulate(parseGlyphContours(inGlyph));

                const float units = 1.0f / inGlyph->face->units_per_EM;

                FontGlyph result;
                result.code      = inCode;
                result.width     = inGlyph->metrics.width * units;
                result.height    = inGlyph->metrics.height * units;
                result.advance   = inGlyph->metrics.horiAdvance * units;
                result.ascender  = inGlyph->face->ascender * units;
                result.descender = inGlyph->face->descender * units;
                result.bearing   = {inGlyph->metrics.horiBearingX * units, inGlyph->metrics.horiBearingY * units};
                result.indices   = contour.getIndices();

                Vertex vertex;
                vertex.color = Vec4(255.0f);
                for (const Vertex::Position& position : contour.getPositions())
                {
                    vertex.position.x = position.x * units;
                    vertex.position.y = position.y * units;

                    result.vertices.push_back(vertex);
                }

                result.curves = parseGlyphCurves(inGlyph);

                bool bHasBounds = false;
                for (FontGlyphCurve& curve : result.curves)
                {
                    curve.start.x *= units;
                    curve.start.y *= units;
                    curve.control.x *= units;
                    curve.control.y *= units;
                    curve.end.x *= units;
                    curve.end.y *= units;

                    for (const Vec2& point : {curve.start, curve.control, curve.end})
                    {
                        if (!bHasBounds)
                        {
                            result.boundsMin = point;
                            result.boundsMax = point;
                            bHasBounds       = true;

                            continue;
                        }

                        result.boundsMin.x = std::min(result.boundsMin.x, point.x);
                        result.boundsMin.y = std::min(result.boundsMin.y, point.y);
                        result.boundsMax.x = std::max(result.boundsMax.x, point.x);
                        result.boundsMax.y = std::max(result.boundsMax.y, point.y);
                    }
                }

                return result;
            }

            FontFamily parse(const String& inFamily, const FontRaw& inData)
            {
                return parse(inFamily, inData, -1.0f);
            }

            FontFamily parse(const String& inFamily, const FontRaw& inData, float inWeight)
            {
                FT_Library library = nullptr;
                if (FT_Init_FreeType(&library))
                {
                    throw std::runtime_error("Failed to parse the font");
                }

                FT_Face face = nullptr;
                if (FT_New_Memory_Face(library, inData.data(), inData.size(), 0, &face))
                {
                    FT_Done_FreeType(library);

                    throw std::runtime_error("Failed to find the font face");
                }

                FT_CharMap charmap = nullptr;
                for (int i = 0; i < face->num_charmaps; i++)
                {
                    charmap = face->charmaps[i];

                    if (charmap->encoding == FT_ENCODING_UNICODE)
                    {
                        if (FT_Set_Charmap(face, charmap))
                        {
                            FT_Done_Face(face);
                            FT_Done_FreeType(library);

                            throw std::runtime_error("Failed to load the font char map");
                        };

                        break;
                    }
                }

                FontFamily result;
                result.setName(inFamily);
                result.setFamily(face->family_name ? String(face->family_name) : inFamily);

                const float units = 1.0f / face->units_per_EM;

                float weight    = (face->style_flags & FT_STYLE_FLAG_BOLD) ? 700.0f : 400.0f;
                float weightMin = weight;
                float weightMax = weight;

                if (TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2)))
                {
                    if (os2->version != 0xFFFF && os2->usWeightClass > 0)
                    {
                        weight    = static_cast<float>(os2->usWeightClass);
                        weightMin = weight;
                        weightMax = weight;
                    }
                }

                FT_MM_Var* variation = nullptr;
                if (FT_Get_MM_Var(face, &variation) == 0 && variation)
                {
                    std::vector<FT_Fixed> coordinates(variation->num_axis);

                    for (FT_UInt i = 0; i < variation->num_axis; ++i)
                    {
                        coordinates.at(i) = variation->axis[i].def;

                        if (variation->axis[i].tag != FT_MAKE_TAG('w', 'g', 'h', 't'))
                        {
                            continue;
                        }

                        weightMin = variation->axis[i].minimum / 65536.0f;
                        weightMax = variation->axis[i].maximum / 65536.0f;
                        weight    = variation->axis[i].def / 65536.0f;

                        if (inWeight >= 0.0f)
                        {
                            weight = std::clamp(inWeight, weightMin, weightMax);
                        }

                        coordinates.at(i) = static_cast<FT_Fixed>(weight * 65536.0f);
                    }

                    if (weightMin < weightMax)
                    {
                        FT_Set_Var_Design_Coordinates(face, variation->num_axis, coordinates.data());
                    }

                    FT_Done_MM_Var(library, variation);
                }
                else if (inWeight >= 0.0f)
                {
                    weight = inWeight;
                }

                result.setWeight(weight);
                result.setWeightRange(weightMin, weightMax);
                result.setMetrics(face->ascender * units, face->descender * units);

                std::unordered_map<FT_UInt, char32_t> indexToCode;

                FT_UInt  glyphIndex;
                FT_ULong code = FT_Get_First_Char(face, &glyphIndex);
                while (glyphIndex != 0)
                {
                    indexToCode[glyphIndex] = static_cast<char32_t>(code);

                    if (FT_Load_Glyph(
                            face,
                            glyphIndex,
                            FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_SCALE |
                                FT_LOAD_LINEAR_DESIGN | FT_LOAD_IGNORE_TRANSFORM
                        ) == 0)
                    {
                        result.addGlyph(parseGlyph(code, face->glyph));
                    }

                    code = FT_Get_Next_Char(face, code, &glyphIndex);
                }

                if (FT_HAS_KERNING(face))
                {
                    for (FT_UInt leftIndex = 0; leftIndex < face->num_glyphs; ++leftIndex)
                    {
                        const auto leftIt = indexToCode.find(leftIndex);

                        if (leftIt == indexToCode.end())
                        {
                            continue;
                        }

                        for (FT_UInt rightIndex = 0; rightIndex < face->num_glyphs; ++rightIndex)
                        {
                            const auto rightIt = indexToCode.find(rightIndex);

                            if (rightIt == indexToCode.end())
                            {
                                continue;
                            }

                            FT_Vector delta;

                            if (FT_Get_Kerning(face, leftIndex, rightIndex, FT_KERNING_UNSCALED, &delta) != 0 ||
                                delta.x == 0)
                            {
                                continue;
                            }

                            result.addKerning(leftIt->second, rightIt->second, delta.x * units);
                        }
                    }
                }

                FT_Done_Face(face);
                FT_Done_FreeType(library);

                return result;
            }
        }
    }
}