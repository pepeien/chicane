#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Font.hpp"
#include "Chicane/Box/Asset/Font/Extracted.hpp"
#include "Chicane/Box/Asset/Font/Parsed.hpp"
#include "Chicane/Box/Manager.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX FontManager : public Box::Manager<FontExtracted, FontParsed>
        {
        public:
            FontManager();

        public:
            bool isFamilyAllocated(const String& inFamily) const;

            // Setup
            void load(const String& inId, const Font& inFont);
            void allocate(const String& inId);

            // Data
            const FontExtracted& getData(const String& inId) const;
            const FontParsed& getParsed(const String& inId) const;

            const FontParsed& getByFamily(const String& inFamily) const;
        };
    }
}