#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Preview.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX PreviewService
        {
        public:
            static PreviewService& instance();

        public:
            PreviewService() = default;

        public:
            void enqueue(const FileSystem::Path& inFilePath);
            void drain(std::vector<std::unique_ptr<AssetPreview>>& outReady);

        private:
            void finish(const FileSystem::Path& inFilePath, std::unique_ptr<AssetPreview> inPreview);

        private:
            std::mutex                                 m_mutex;
            std::unordered_set<FileSystem::Path>       m_inFlight;
            std::vector<std::unique_ptr<AssetPreview>> m_ready;
        };
    }
}
