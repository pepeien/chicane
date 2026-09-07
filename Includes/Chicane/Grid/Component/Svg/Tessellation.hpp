#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Primitive.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID SvgTessellation
        {
        public:
            using Builder = std::function<Primitive()>;

        public:
            static SvgTessellation& instance();

        public:
            SvgTessellation() = default;

        public:
            const Primitive* find(const std::string& inKey) const;
            void request(const std::string& inKey, Builder inBuild);
            bool pump();
            std::uint64_t generation() const;

        private:
            struct Ready
            {
                std::string key;
                Primitive   primitive;
            };

        private:
            void finish(const std::string& inKey, Primitive inPrimitive);

        private:
            std::mutex                                 m_mutex;
            std::unordered_set<std::string>            m_inFlight;
            std::vector<Ready>                         m_ready;
            std::unordered_map<std::string, Primitive> m_cache;
            std::uint64_t                              m_generation = 0;
        };
    }
}
