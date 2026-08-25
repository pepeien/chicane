#pragma once

#include <atomic>
#include <memory>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/View/InputQueue/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID ViewInputQueue
        {
        public:
            static constexpr std::size_t CAPACITY = 2048;

        public:
            ViewInputQueueEvent      events[CAPACITY];
            std::atomic<std::size_t> write = {0};
            std::atomic<std::size_t> read  = {0};
        };
    }
}