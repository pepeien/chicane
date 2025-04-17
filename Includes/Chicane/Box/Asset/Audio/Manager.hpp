#pragma once

#include "Core.hpp"
#include "Box/Asset/Audio/Data.hpp"
#include "Box/Asset/Audio/Instance.hpp"
#include "Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            class CHICANE Manager : public Box::Manager::Instance<RawData, CompiledData>
            {
            public:
                Manager();
                ~Manager();

            protected:
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const std::string& inId, const Audio::Instance* inAudio);

                // Data
                const RawData& getInstance(const std::string& inId) const;
                const CompiledData& getData(const std::string& inId) const;
            };
        }
    }
}