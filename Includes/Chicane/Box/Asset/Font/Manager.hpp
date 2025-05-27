#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"
#include "Chicane/Box/Asset/Font/Instance.hpp"

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