#include "Chicane/Box/Instance.hpp"

const Chicane::Box::Instance EMPTY_INSTANCE {};

namespace Chicane
{
    namespace Box
    {
        const Instance& Instance::empty()
        {
            return EMPTY_INSTANCE;
        }
    }
}