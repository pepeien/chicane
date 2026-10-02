#include "Chicane/Box/Asset/Preview/Upload.hpp"

#include "Chicane/Core/Mailbox.hpp"

namespace Chicane
{
    namespace Box
    {
        static Mailbox<PreviewUpload> g_pending;

        void PreviewUpload::sEnqueue(const String& inReference, const Image::Instance& inImage)
        {
            if (inReference.isEmpty() || !inImage)
            {
                return;
            }

            g_pending.push({inReference, inImage});
        }

        void PreviewUpload::sDrain(std::vector<PreviewUpload>& outPending)
        {
            outPending = g_pending.drain();
        }
    }
}
