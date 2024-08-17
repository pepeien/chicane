#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Thread
    {
        class Instance;

        class Pool
	    {
        public:
            Pool(std::uint32_t inCount);

	    public:
            void wait();

        public:
            std::vector<std::unique_ptr<Instance>> threads; 
	    };
    }
}