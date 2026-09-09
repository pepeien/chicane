#include "Chicane/Grid/Component/Svg/Tessellation.hpp"

#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    namespace Grid
    {
        SvgTessellation& SvgTessellation::instance()
        {
            static SvgTessellation service;

            return service;
        }

        const Primitive* SvgTessellation::find(const std::string& inKey) const
        {
            const auto found = m_cache.find(inKey);
            if (found == m_cache.end())
            {
                return nullptr;
            }

            return &found->second;
        }

        void SvgTessellation::request(const std::string& inKey, Builder inBuild)
        {
            if (inKey.empty() || !inBuild || find(inKey))
            {
                return;
            }

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_inFlight.find(inKey) != m_inFlight.end())
                {
                    return;
                }

                m_inFlight.insert(inKey);
            }

            Worker::submit(
                [inKey, inBuild = std::move(inBuild)]()
                {
                    Primitive primitive;
                    try
                    {
                        primitive = inBuild();
                    }
                    catch (...)
                    {
                        primitive.clear();
                    }

                    SvgTessellation::instance().finish(inKey, std::move(primitive));
                }
            );
        }

        bool SvgTessellation::pump()
        {
            std::vector<SvgTessellationReady> ready;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                ready.swap(m_ready);

                for (const SvgTessellationReady& entry : ready)
                {
                    m_inFlight.erase(entry.key);
                }
            }

            if (ready.empty())
            {
                return false;
            }

            for (SvgTessellationReady& entry : ready)
            {
                m_cache[std::move(entry.key)] = std::move(entry.primitive);
            }

            m_generation++;

            return true;
        }

        std::uint64_t SvgTessellation::generation() const
        {
            return m_generation;
        }

        void SvgTessellation::finish(const std::string& inKey, Primitive inPrimitive)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_ready.push_back({inKey, std::move(inPrimitive)});
        }
    }
}
