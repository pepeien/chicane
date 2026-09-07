#include "Chicane/Box/Asset/Preview/Service.hpp"

namespace Chicane
{
    namespace Box
    {
        PreviewService& PreviewService::instance()
        {
            static PreviewService service;

            return service;
        }

        PreviewService::PreviewService()
            : m_bRunning(false)
        {}

        PreviewService::~PreviewService()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_bRunning = false;
            }

            m_readySignal.notify_all();

            if (m_worker.joinable())
            {
                m_worker.join();
            }
        }

        void PreviewService::start()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_bRunning)
            {
                return;
            }

            m_bRunning = true;
            m_worker   = std::thread(&PreviewService::loop, this);
        }

        void PreviewService::enqueue(const FileSystem::Path& inFilePath)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_inFlight.find(inFilePath) != m_inFlight.end())
                {
                    return;
                }

                m_inFlight.insert(inFilePath);
                m_pending.push(inFilePath);
            }

            start();
            m_readySignal.notify_one();
        }

        void PreviewService::drain(std::vector<std::unique_ptr<AssetPreview>>& outReady)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            outReady.swap(m_ready);
        }

        void PreviewService::loop()
        {
            while (true)
            {
                FileSystem::Path path;

                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_readySignal.wait(
                        lock,
                        [this]()
                        {
                            return !m_bRunning || !m_pending.empty();
                        }
                    );

                    if (!m_bRunning && m_pending.empty())
                    {
                        return;
                    }

                    path = m_pending.front();
                    m_pending.pop();
                }

                std::unique_ptr<AssetPreview> preview;
                try
                {
                    preview = decodePreview(path);
                }
                catch (...)
                {
                    preview.reset();
                }

                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_inFlight.erase(path);

                    if (preview)
                    {
                        m_ready.push_back(std::move(preview));
                    }
                }
            }
        }
    }
}
