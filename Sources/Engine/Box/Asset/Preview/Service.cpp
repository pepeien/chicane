#include "Chicane/Box/Asset/Preview/Service.hpp"

#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    namespace Box
    {
        void PreviewService::enqueue(const FileSystem::Path& inFilePath)
        {
            if (m_inFlight.find(inFilePath) != m_inFlight.end())
            {
                return;
            }

            m_inFlight.insert(inFilePath);

            Worker::sSubmit(
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

                    PreviewService::sInstance().finish(inFilePath, std::move(preview));
                }
            );
        }

        void PreviewService::drain(std::vector<std::unique_ptr<AssetPreview>>& outReady)
        {
            for (Ready& ready : m_ready.drain())
            {
                m_inFlight.erase(ready.path);

                if (ready.preview)
                {
                    outReady.push_back(std::move(ready.preview));
                }
            }
        }

        void PreviewService::finish(const FileSystem::Path& inFilePath, std::unique_ptr<AssetPreview> inPreview)
        {
            m_ready.push({inFilePath, std::move(inPreview)});
        }
    }
}
