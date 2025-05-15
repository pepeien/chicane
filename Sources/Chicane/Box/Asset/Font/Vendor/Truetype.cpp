#include "Chicane/Box/Asset/Font/Vendor/TrueType.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

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

                        FT_Done_Face(face);
                        FT_Done_FreeType(library);

                        return {};
                    }
                }
            }
        }
    }
}