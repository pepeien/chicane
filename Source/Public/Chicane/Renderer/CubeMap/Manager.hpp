#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/CubeMap.hpp"
#include "Chicane/Core/CubeMap.hpp"
#include "Chicane/Renderer/Manager.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        class Manager : public Chicane::Manager::Instance<CubeMap::Instance, CubeMap::Data>
        {
        public:
            Manager();

        public:
            // Setup
            void load(const std::string& inId, const Box::CubeMap* inAsset);

            // Data
            const CubeMap::Instance& getInstance(const std::string& inId) const;
            const CubeMap::Data& getData(const std::string& inId) const;
        };
    }
}