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
                bool isFamilyAllocated(const std::string& inFamily) const;

                // Setup
                void load(const std::string& inId, const Font::Instance& inFont);
                void allocate(const std::string& inId);

                // Data
                const Extracted& getData(const std::string& inId) const;
                const Parsed& getParsed(const std::string& inId) const;

                const Parsed& getByFamily(const std::string& inFamily) const;
            };
        }
    }
}