#pragma once

#include "Chicane/Box/Asset/Font/Extracted.hpp"
#include "Chicane/Box/Asset/Font/Instance.hpp"
#include "Chicane/Box/Asset/Font/Parsed.hpp"
#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Extracted, Parsed>
            {
            public:
                Manager();

            public:
                bool isFamilyAllocated(const String& inFamily) const;

                // Setup
                void load(const String& inId, const Font::Instance& inFont);
                void allocate(const String& inId);

                // Data
                const Extracted& getData(const String& inId) const;
                const Parsed& getParsed(const String& inId) const;

                const Parsed& getByFamily(const String& inFamily) const;
            };
        }
    }
}