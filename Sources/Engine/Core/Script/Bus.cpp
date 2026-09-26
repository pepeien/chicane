#include "Chicane/Core/Script/Bus.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Script
    {
        Bus::Bus()
            : m_next(1),
              m_slots({})
        {}

        std::uint64_t Bus::subscribe(const String& inName, Callback inCallback)
        {
            const std::uint64_t token = m_next++;
            m_slots[inName].push_back({token, std::move(inCallback)});

            return token;
        }

        void Bus::unsubscribe(std::uint64_t inToken)
        {
            for (auto& [name, slots] : m_slots)
            {
                const auto found = std::remove_if(
                    slots.begin(),
                    slots.end(),
                    [inToken](const Slot& slot) { return slot.token == inToken; }
                );

                if (found == slots.end())
                {
                    continue;
                }

                slots.erase(found, slots.end());

                return;
            }
        }

        void Bus::send(const String& inName, const String& inData)
        {
            const auto found = m_slots.find(inName);
            if (found == m_slots.end())
            {
                return;
            }

            const std::vector<Slot> slots = found->second;
            for (const Slot& slot : slots)
            {
                if (slot.callback)
                {
                    slot.callback(inData);
                }
            }
        }
    }
}
