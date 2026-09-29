#include "Chicane/Core/Script/Handle.hpp"

#include <mutex>
#include <unordered_set>

namespace Chicane
{
    namespace Script
    {
        static std::mutex& mutex()
        {
            static std::mutex* result = new std::mutex();

            return *result;
        }

        static std::unordered_set<const void*>& values()
        {
            static std::unordered_set<const void*>* result = new std::unordered_set<const void*>();

            return *result;
        }

        void Handle::add(const void* inValue)
        {
            if (!inValue)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(mutex());
            values().insert(inValue);
        }

        void Handle::remove(const void* inValue)
        {
            if (!inValue)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(mutex());
            values().erase(inValue);
        }

        bool Handle::contains(const void* inValue)
        {
            if (!inValue)
            {
                return false;
            }

            std::lock_guard<std::mutex> lock(mutex());

            return values().find(inValue) != values().end();
        }
    }
}
