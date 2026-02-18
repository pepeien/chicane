#include "Chicane/Box/Font/TrueType.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Font.hpp"

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

                    Curve curve = {};
                    curve.setSegmentCount(12);
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

                std::vector<Curve> result = {};

                FT_Outline_Decompose(&inGlyph->outline, &funcs, &result);

                return result;
            }

            FontGlyph parseGlyph(std::uint32_t inCode, FT_GlyphSlot inGlyph)
            {
                Contour contour;
                contour.triangulate(parseGlyphContours(inGlyph));

                FontGlyph result;
                result.code     = inCode;
                result.units    = inGlyph->face->units_per_EM;
                result.width    = inGlyph->metrics.width;
                result.height   = inGlyph->metrics.height;
                result.advance  = inGlyph->advance.x;
                result.bearing  = {inGlyph->metrics.horiBearingX, inGlyph->metrics.vertBearingY};
                result.vertices = contour.getVertices();
                result.indices  = contour.getIndices();

                float invUnits = 1.0f / result.units;
                for (auto& vertex : result.vertices)
                {
                    vertex.x *= invUnits;
                    vertex.y *= invUnits;
                }

                return result;
            }

            FontParsed parse(const FontRaw& inData)
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

                FontParsed result = {};
                result.name       = face->family_name;

                FT_UInt  glyphIndex;
                FT_ULong code = FT_Get_First_Char(face, &glyphIndex);
                while (glyphIndex != 0)
                {
                    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE) == 0)
                    {
                        result.glyphs.emplace(code, parseGlyph(code, face->glyph));
                    }

                    code = FT_Get_Next_Char(face, code, &glyphIndex);
                }

                FT_Done_Face(face);
                FT_Done_FreeType(library);

                return result;
            }
        }
    }
}