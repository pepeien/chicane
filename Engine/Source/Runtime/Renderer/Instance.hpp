#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Instance
    {
        bool areExtensionsSupported(const std::vector<const char*> inExtensions);
        bool areValidationLayersSupported(const std::vector<const char*> inValidationLayers);

        void init(vk::Instance& outInstance, vk::DispatchLoaderDynamic& outDldi);
    }
}