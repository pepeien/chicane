#include "Chicane/Box/Asset/Font/TrueType.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            namespace TrueType
            {
                std::vector<Curve> parseGlyphContours(FT_GlyphSlot inGlyph)
                {
                    FT_Outline_Funcs funcs;
                    funcs.shift = 0;
                    funcs.delta = 0;
                    funcs.move_to = [](const FT_Vector* inPoint, void* inData)
                    {
                        std::vector<Curve>* contours = static_cast<std::vector<Curve>*>(inData);

                        if (!contours->empty() && contours->back().isEmpty())
                        {
                            contours->back().addPoint(Vec2(inPoint->x, inPoint->y));

                            return 0;
                        }

                        Curve curve = {};
                        curve.setSegmentCount(5);
                        curve.addPoint(Vec2(inPoint->x, inPoint->y));

                        contours->push_back(curve);

                        return 0;
                    };
                    funcs.line_to = [](const FT_Vector* inPoint, void* inData)
                    {
                        Curve* curve = &static_cast<std::vector<Curve>*>(inData)->back();

                        if (!curve)
                        {
                            return 0;
                        }

                        curve->addPoint(Vec2(inPoint->x, inPoint->y));

                        return 0;
                    };
                    funcs.conic_to = [](
                        const FT_Vector* inControl,
                        const FT_Vector* inPoint,
                        void* inData
                    )
                    {
                        Curve* curve = &static_cast<std::vector<Curve>*>(inData)->back();

                        if (!curve)
                        {
                            return 0;
                        }

                        curve->addQuadraticPoint(
                            Vec2(inControl->x, inControl->y),
                            Vec2(inPoint->x,   inPoint->y)
                        );

                        return 0;
                    };
                    funcs.cubic_to = [](
                        const FT_Vector* inControlA,
                        const FT_Vector* inControlB, 
                        const FT_Vector* inPoint,
                        void* inData
                    )
                    {
                        Curve* curve = &static_cast<std::vector<Curve>*>(inData)->back();

                        if (!curve)
                        {
                            return 0;
                        }

                        curve->addBezierPoint(
                            Vec2(inControlA->x, inControlA->y),
                            Vec2(inControlB->x, inControlB->y),
                            Vec2(inPoint->x,    inPoint->y)
                        );

                        return 0;
                    };

                    std::vector<Curve> result = {};

                    FT_Outline_Decompose(&inGlyph->outline, &funcs, &result);

                    return result;
                }

                Glyph parseGlyph(std::uint32_t inCode, FT_GlyphSlot inGlyph)
                {
                    const std::vector<Curve> contours = parseGlyphContours(inGlyph);

                    Glyph result = {};
                    result.code      = inCode;
                    result.advance.x = inGlyph->advance.x;
                    result.advance.y = inGlyph->advance.y;
                    result.bearing.x = inGlyph->metrics.horiBearingX;
                    result.bearing.y = inGlyph->metrics.horiBearingY;
                    result.vertices  = Curve::getTriangleVertices(contours);
                    result.indices   = Curve::getTriangleIndices(contours);

                    return result;
                }

                Parsed parse(const std::vector<unsigned char>& inData)
                {
                    FT_Library library = nullptr;
                    FT_Init_FreeType(&library);

                    if (!library)
                    {
                        throw std::runtime_error("Failed to parse the font");
                    }

                    FT_Face face = nullptr;
                    FT_New_Memory_Face(library, inData.data(), inData.size(), 0, &face);

                    if (!face)
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
                            FT_Set_Charmap(face, charmap);

                            break;
                        }
                    }

                    if (!charmap) {
                        FT_Done_Face(face);
                        FT_Done_FreeType(library);

                        throw std::runtime_error("Failed to load the font char map");
                    }

                    Parsed result = {};
                    result.name = face->family_name;

                    for (FT_ULong code = 0; code < 0x10FFFF; code++)
                    {
                        std::uint32_t index = FT_Get_Char_Index(face, code);

                        if (index <= 0)
                        {
                            continue;
                        }

                        if (FT_Load_Glyph(face, index, FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING))
                        {
                            continue;
                        }

                        result.glyphs.insert(
                            std::make_pair(
                                code,
                                parseGlyph(code, face->glyph)
                            )
                        );
                    }

                    FT_Done_Face(face);
                    FT_Done_FreeType(library);

                    return result;
                }
            }
        }
    }
}