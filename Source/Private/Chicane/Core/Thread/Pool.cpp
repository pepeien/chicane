#include "Chicane/Core/Thread/Pool.hpp"

#include "Chicane/Core/Thread.hpp"

namespace Chicane
{
    namespace Thread
    {
        Pool::Pool(std::uint32_t inCount)
        {
            for (std::uint32_t i = 0; i < inCount; i++)
            {
                threads.push_back(std::make_unique<Instance>());
            }
        }

        void Pool::wait()
        {
            for (const auto& thread : threads)
            {
                thread->wait();
            }
        }
    }
}