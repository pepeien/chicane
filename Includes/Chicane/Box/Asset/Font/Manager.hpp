#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"
#include "Chicane/Box/Asset/Font/Instance.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<RawData, CompiledData>
            {
            public:
                Manager();

            public:
                bool isFamilyLoaded(const std::string& inFamily) const;

                // Setup
                void load(const std::string& inId, const Font::Instance* inFont);

                // Data
                const RawData& getInstance(const std::string& inId) const;
                const CompiledData& getData(const std::string& inId) const;

                const RawData& getByFamily(const std::string& inFamily) const;
            };
        }
    }
}