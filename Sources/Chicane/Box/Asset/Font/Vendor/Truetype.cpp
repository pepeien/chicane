#include "Chicane/Box/Asset/Font/Vendor/TrueType.hpp"

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
            namespace Vendor
            {
                namespace TrueType
                {
                    constexpr const float PIXEL_SIZE_MULTIPLIER = 100.0f;
                    constexpr const float FONT_WIDTH_MULTIPLIER = 1.0f / 1000.0f;
                    constexpr const float FORMAT_26_6_MULTIPLIER = 1.0f / 64.0f;
                    constexpr const float FONT_MULTIPLIER = FORMAT_26_6_MULTIPLIER / PIXEL_SIZE_MULTIPLIER;

                    std::vector<Vec<3, float>> triangulateGlyphOutlines(const GlyphOutline& inOutline)
                    {
                        std::vector<Vec<3, float>> result = {};

                        for (const Vec<2, float>& vertex : inOutline.curve.getPoints())
                        {
                            result.push_back(
                                Vec<3, float>(
                                    vertex.x * FONT_MULTIPLIER,
                                    vertex.y * FONT_MULTIPLIER,
                                    0.0f
                                )
                            );
                        }

                        if (!result.empty())
                        {
                            if (result.back() != result.front())
                            {
                                result.push_back(result.front());
                            }
                        }

                        return result;
                    }

                    Curve parseGlyphCurves(FT_GlyphSlot inGlyph)
                    {
                        FT_Outline_Funcs funcs;
                        funcs.shift = 0;
                        funcs.delta = 0;
                        funcs.move_to = [](const FT_Vector* inPoint, void* inData)
                        {
                            Curve* curve = static_cast<Curve*>(inData);
                            curve->addPoint(
                                Vec<2, float>(inPoint->x, inPoint->y)
                            );

                            return 0;
                        };
                        funcs.line_to = [](const FT_Vector* inPoint, void* inData)
                        {
                            Curve* curve = static_cast<Curve*>(inData);
                            curve->addPoint(
                                Vec<2, float>(inPoint->x, inPoint->y)
                            );

                            return 0;
                        };
                        funcs.conic_to = [](
                            const FT_Vector* inControl,
                            const FT_Vector* inPoint,
                            void* inData
                        )
                        {
                            Curve* curve = static_cast<Curve*>(inData);
                            curve->addQuadraticPoint(
                                Vec<2, float>(inControl->x, inControl->y),
                                Vec<2, float>(inPoint->x,   inPoint->y)
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
                            Curve* curve = static_cast<Curve*>(inData);
                            curve->addBezierPoint(
                                Vec<2, float>(inControlA->x, inControlA->y),
                                Vec<2, float>(inControlB->x, inControlB->y),
                                Vec<2, float>(inPoint->x,    inPoint->y)
                            );

                            return 0;
                        };

                        Curve result = {};
                        result.setSegmentCount(10);

                        FT_Outline_Decompose(&inGlyph->outline, &funcs, &result);

                        return result;
                    }

                    Glyph parseGlyph(std::uint32_t inCode, FT_GlyphSlot inGlyph)
                    {
                        GlyphOutline outline = {};
                        outline.code      = inCode;
                        outline.advance.x = inGlyph->advance.x * FONT_MULTIPLIER;
                        outline.advance.y = inGlyph->advance.y * FONT_MULTIPLIER;
                        outline.bearing.x = inGlyph->metrics.horiBearingX * FONT_MULTIPLIER;
                        outline.bearing.y = inGlyph->metrics.horiBearingY * FONT_MULTIPLIER;
                        outline.curve    = parseGlyphCurves(inGlyph);

                        Glyph result = {};
                        result.code     = inCode;
                        result.width    = inGlyph->metrics.width * FONT_WIDTH_MULTIPLIER;
                        result.height   = inGlyph->metrics.height * FONT_WIDTH_MULTIPLIER;
                        result.advance  = outline.advance;
                        result.bearing  = outline.bearing;
                        result.vertices = triangulateGlyphOutlines(outline);

                        return result;
                    }

                    RawData parse(const std::vector<unsigned char>& inData)
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

                        RawData result = {};
                        result.name = face->family_name;

                        for (FT_ULong code = 0; code < 0x10FFFF; code++)
                        {
                            std::uint32_t index = FT_Get_Char_Index(face, code);

                            if (index <= 0)
                            {
                                continue;
                            }

                            if (FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING))
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
}