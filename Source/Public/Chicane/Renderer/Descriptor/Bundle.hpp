#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Descriptor
    {
        struct Bundle
        {
        public:
            vk::DescriptorSetLayout setLayout = {};
            vk::DescriptorSet       set       = {};
            vk::DescriptorPool      pool      = VK_NULL_HANDLE;
        };
    }
}