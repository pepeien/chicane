#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
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
            PreviewService();
            ~PreviewService();

        public:
            void enqueue(const FileSystem::Path& inFilePath);
            void drain(std::vector<std::unique_ptr<AssetPreview>>& outReady);

        private:
            void start();
            void loop();

        private:
            std::mutex                                 m_mutex;
            std::condition_variable                    m_readySignal;
            std::queue<FileSystem::Path>               m_pending;
            std::unordered_set<FileSystem::Path>       m_inFlight;
            std::vector<std::unique_ptr<AssetPreview>> m_ready;
            std::thread                                m_worker;
            bool                                       m_bRunning;
        };
    }
}
