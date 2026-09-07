#pragma once

#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX PreviewUpload
        {
        public:
            static void enqueue(const String& inReference, const Image::Instance& inImage);
            static void drain(std::vector<PreviewUpload>& outPending);

        public:
            String          reference;
            Image::Instance image;
        };
    }
}
