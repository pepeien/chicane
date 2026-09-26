#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Script/Bus/Slot.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Script
    {
        class CHICANE_CORE Bus
        {
        public:
            using Callback = std::function<void(const String&)>;

        public:
            Bus();

        public:
            std::uint64_t subscribe(const String& inName, Callback inCallback);
            void unsubscribe(std::uint64_t inToken);
            void send(const String& inName, const String& inData);

        private:
            std::uint64_t                                 m_next;
            std::unordered_map<String, std::vector<Slot>> m_slots;
        };
    }
}
