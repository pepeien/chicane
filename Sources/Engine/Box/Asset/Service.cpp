#include "Chicane/Box/Asset/Service.hpp"

#include <exception>

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Worker/Pool.hpp"

namespace Chicane
{
    namespace Box
    {
        void AssetService::enqueue(const FileSystem::Path& inFilePath)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_inFlight.find(inFilePath) != m_inFlight.end())
                {
                    return;
                }

                m_inFlight.insert(inFilePath);
            }

            WorkerPool::sDetach(
                [inFilePath]()
                {
                    try
                    {
                        decodeAsset(inFilePath);
                    }
                    catch (const std::exception& exception)
                    {
                        Log::warning(
                            "Failed to load asset [%s]: %s",
                            inFilePath.toChar(),
                            exception.what()
                        );
                    }
                    catch (...)
                    {
                        Log::warning("Failed to load asset [%s]", inFilePath.toChar());
                    }

                    AssetService::sInstance().finish(inFilePath);
                }
            );
        }

        bool AssetService::isPending(const FileSystem::Path& inFilePath) const
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            return m_inFlight.find(inFilePath) != m_inFlight.end();
        }

        void AssetService::queueNotify(const Asset* inAsset)
        {
            if (!inAsset)
            {
                return;
            }

            m_notify.push(inAsset);
        }

        void AssetService::drain(std::vector<const Asset*>& outReady)
        {
            std::vector<FileSystem::Path> ready;
            std::vector<const Asset*>     notify;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                ready  = m_ready.drain();
                notify = m_notify.drain();

                for (const FileSystem::Path& path : ready)
                {
                    m_inFlight.erase(path);
                }
            }

            outReady.reserve(outReady.size() + notify.size());
            for (const Asset* asset : notify)
            {
                if (!asset)
                {
                    continue;
                }

                outReady.push_back(asset);
            }
        }

        void AssetService::finish(const FileSystem::Path& inFilePath)
        {
            m_ready.push(inFilePath);
        }
    }
}
