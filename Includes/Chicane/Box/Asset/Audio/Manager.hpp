#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Audio/Data.hpp"
#include "Chicane/Box/Asset/Audio/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Raw, Parsed>
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
                const Raw& getInstance(const std::string& inId) const;
                const Parsed& getData(const std::string& inId) const;
            };
        }
    }
}