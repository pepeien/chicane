#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Preview.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Mailbox.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX PreviewService
        {
        public:
            static inline PreviewService& sInstance()
            {
                static PreviewService service;

                return service;
            }

        public:
            PreviewService() = default;

        public:
            void enqueue(const FileSystem::Path& inFilePath);
            void drain(std::vector<std::unique_ptr<AssetPreview>>& outReady);

        private:
            struct Ready
            {
            public:
                FileSystem::Path              path;
                std::unique_ptr<AssetPreview> preview;
            };

        private:
            void finish(const FileSystem::Path& inFilePath, std::unique_ptr<AssetPreview> inPreview);

        private:
            std::unordered_set<FileSystem::Path> m_inFlight;
            Mailbox<Ready>                       m_ready;
        };
    }
}
