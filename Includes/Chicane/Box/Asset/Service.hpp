#pragma once

#include <mutex>
#include <unordered_set>
#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Mailbox.hpp"

namespace Chicane
{
    namespace Box
    {
        const Asset* decodeAsset(const FileSystem::Path& inFilePath);

        class CHICANE_BOX AssetService
        {
        public:
            static inline AssetService& sInstance()
            {
                static AssetService service;

                return service;
            }

        public:
            AssetService() = default;

        public:
            void enqueue(const FileSystem::Path& inFilePath);
            bool isPending(const FileSystem::Path& inFilePath) const;
            void queueNotify(const Asset* inAsset);
            void drain(std::vector<const Asset*>& outReady);

        private:
            void finish(const FileSystem::Path& inFilePath);

        private:
            mutable std::mutex                   m_mutex;
            std::unordered_set<FileSystem::Path> m_inFlight;
            Mailbox<FileSystem::Path>            m_ready;
            Mailbox<const Asset*>                m_notify;
        };
    }
}
