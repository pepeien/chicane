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
            static void sEnqueue(const String& inReference, const Image::Instance& inImage);
            static void sDrain(std::vector<PreviewUpload>& outPending);

        public:
            String          reference;
            Image::Instance image;
        };
    }
}
