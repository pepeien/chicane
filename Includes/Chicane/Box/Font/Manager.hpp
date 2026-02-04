#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Font/Parsed.hpp"
#include "Chicane/Box/Manager.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX FontManager : public Manager<Font, FontParsed>
        {
        public:
            FontManager();

        protected:
            void onLoad(const String& inId, const Font& inData) override;

        public:
            bool containsFamily(const String& inName) const;
            const FontParsed& getFamily(const String& inName) const;
        };
    }
}