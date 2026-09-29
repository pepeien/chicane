#include "Chicane/Core/Script/Channel.hpp"

namespace Chicane
{
    namespace Script
    {
        Queue& Channel::sToView()
        {
            static Queue queue;

            return queue;
        }

        Queue& Channel::sToScene()
        {
            static Queue queue;

            return queue;
        }
    }
}
