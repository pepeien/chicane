#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/CubeMap/Data.hpp"
#include "Chicane/Box/Asset/CubeMap/Instance.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace CubeMap
        {
            class Manager : public Box::Manager::Instance<RawData, CompiledData>
            {
            public:
                Manager();

            public:
                // Setup
                void load(const std::string& inId, const CubeMap::Instance* inAsset);

                // Data
                const RawData& getInstance(const std::string& inId) const;
                const CompiledData& getData(const std::string& inId) const;
            };
        }
    }
}