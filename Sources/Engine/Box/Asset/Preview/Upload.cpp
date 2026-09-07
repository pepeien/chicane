#include "Chicane/Box/Asset/Preview/Upload.hpp"

#include <mutex>

namespace Chicane
{
    namespace Box
    {
        static std::mutex                 g_mutex   = {};
        static std::vector<PreviewUpload> g_pending = {};

        void PreviewUpload::enqueue(const String& inReference, const Image::Instance& inImage)
        {
            if (inReference.isEmpty() || !inImage)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(g_mutex);
            g_pending.push_back({inReference, inImage});
        }

        void PreviewUpload::drain(std::vector<PreviewUpload>& outPending)
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            outPending.swap(g_pending);
        }
    }
}
