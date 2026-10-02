#include "Chicane/Core/FileSystem/Listing/Service.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        void ListingService::enqueue(const Path& inDir)
        {
            const Path dir = inDir.lexicallyNormal();

            const auto cached = m_cache.find(dir);
            if (cached != m_cache.end())
            {
                m_ready.push_back({dir, cached->second});

                return;
            }

            if (m_inFlight.find(dir) != m_inFlight.end())
            {
                return;
            }

            m_inFlight.insert(dir);

            Worker::sSubmit(
                [dir]()
                {
                    Item::List children;
                    try
                    {
                        children = ls(dir, 1);
                    }
                    catch (...)
                    {
                        children.clear();
                    }

                    ListingService::sInstance().finish(dir, std::move(children));
                }
            );
        }

        void ListingService::drain(std::vector<Listing>& outReady)
        {
            for (Listing& listing : m_mailbox.drain())
            {
                m_cache[listing.path] = listing.children;
                m_inFlight.erase(listing.path);
                m_ready.push_back(std::move(listing));
            }

            outReady.swap(m_ready);
        }

        void ListingService::finish(const Path& inDir, Item::List inChildren)
        {
            m_mailbox.push({inDir, std::move(inChildren)});
        }
    }
}
