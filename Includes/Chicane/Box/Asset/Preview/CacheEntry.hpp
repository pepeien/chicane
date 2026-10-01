#pragma once

#include <filesystem>
#include <memory>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Preview.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX PreviewCacheEntry
        {
        public:
            std::unique_ptr<AssetPreview>   preview   = nullptr;
            std::filesystem::file_time_type writeTime = {};
        };
    }
}
