#include "Chicane/Box/Asset/Preview/Service.hpp"

#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    namespace Box
    {
        PreviewService& PreviewService::instance()
        {
            static PreviewService service;

            return service;
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
            }

            Worker::submit(
                [inFilePath]()
                {
                    std::unique_ptr<AssetPreview> preview;
                    try
                    {
                        preview = decodePreview(inFilePath);
                    }
                    catch (...)
                    {
                        preview.reset();
                    }

                    PreviewService::instance().finish(inFilePath, std::move(preview));
                }
            );
        }

        void PreviewService::drain(std::vector<std::unique_ptr<AssetPreview>>& outReady)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            outReady.swap(m_ready);
        }

        void PreviewService::finish(const FileSystem::Path& inFilePath, std::unique_ptr<AssetPreview> inPreview)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_inFlight.erase(inFilePath);

            if (inPreview)
            {
                m_ready.push_back(std::move(inPreview));
            }
        }
    }
}
