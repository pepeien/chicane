#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Box/Manager/EventType.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Manager
        {
            template<typename I, typename E>
            class CHICANE Instance
            {
            protected:
                typedef Instance Super;

            public:
                Instance()
                    : m_observable(std::make_unique<Observable<EventType>>())
                {}

                virtual ~Instance() = default;

            protected:
                // Event
                virtual void onLoad(const std::string& inId, const I& inInstance) { return; };
                virtual void onAllocation(const std::string& inId, const E& inData) { return; };
                virtual void onDeallocation(const std::string& inId) { return; };
                virtual void onActivation(const std::string& inId) { return; };
                virtual void onDeactivation(const std::string& inId) { return; };

            public:
                // Status
                bool isEmpty() const
                {
                    return m_instances.empty() || m_datum.empty();
                }

                bool isLoaded(const std::string& inId) const
                {
                    return m_instances.find(inId) != m_instances.end();
                }

                bool isAllocated(const std::string& inId) const
                {
                    return m_datum.find(inId) != m_datum.end();
                }

                bool isActive(const std::string& inId) const
                {
                    return std::find(
                        m_activeIds.begin(),
                        m_activeIds.end(),
                        inId
                    ) != m_activeIds.end();
                }

                bool isUsing(const std::string& inId) const
                {
                    return std::find(
                        m_usedIds.begin(),
                        m_usedIds.end(),
                        inId
                    ) != m_usedIds.end();
                }

                // Data
                const std::vector<std::string>& getActiveIds() const
                {
                    return m_activeIds;
                }

                const std::vector<std::string>& getUsedIds() const
                {
                    return m_usedIds;
                }

                std::uint32_t getActiveCount() const
                {
                    return static_cast<std::uint32_t>(m_activeIds.size());
                }

                std::uint32_t getUsedCount() const
                {
                    return m_usedIds.size();
                }

                std::uint32_t getUseCount(const std::string& inId) const
                {
                    if (!isUsing(inId))
                    {
                        return 0;
                    }

                    return static_cast<std::uint32_t>(std::count(m_usedIds.begin(),  m_usedIds.end(), inId));
                }

                // Lifecycle
                void load(const std::string& inId, const I& inInstance)
                {
                    if (isLoaded(inId))
                    {
                        return;
                    }

                    m_instances.insert(std::make_pair(inId, inInstance));

                    onLoad(inId, inInstance);

                    m_observable->next(EventType::Load);
                }

                void allocate(const std::string& inId, const E& inData)
                {
                    if (!isLoaded(inId) || isAllocated(inId))
                    {
                        return;
                    }

                    m_datum.insert(std::make_pair(inId, inData));

                    onAllocation(inId, inData);

                    m_observable->next(EventType::Allocation);
                }

                void deallocate(const std::string& inId)
                {
                    if (!isLoaded(inId) || isAllocated(inId))
                    {
                        return;
                    }

                    m_datum.erase(inId);

                    onDeallocation(inId);

                    m_observable->next(EventType::Allocation);
                }

                void activate(const std::string& inId)
                {
                    if (!isLoaded(inId))
                    {
                        return;
                    }

                    m_usedIds.push_back(inId);
                    String::sort(m_usedIds);

                    if (!isActive(inId))
                    {
                        m_activeIds.push_back(inId);
                        String::sort(m_activeIds);

                        onActivation(inId);
                    }

                    m_observable->next(EventType::Activation);
                }

                void deactivate(const std::string& inId)
                {
                    if (!isLoaded(inId) || !isAllocated(inId) || !isActive(inId))
                    {
                        return;
                    }

                    m_usedIds.erase(
                        std::find(
                            m_usedIds.begin(),
                            m_usedIds.end(),
                            inId
                        )
                    );
                    m_usedIds.shrink_to_fit();
                    String::sort(m_usedIds);

                    if (!isUsing(inId))
                    {
                        m_activeIds.erase(
                            std::find(
                                m_activeIds.begin(),
                                m_activeIds.end(),
                                inId
                            )
                        );
                        m_activeIds.shrink_to_fit();
                        String::sort(m_activeIds);
                    }

                    m_observable->next(EventType::Activation);
                }

                // Events
                Subscription<EventType>* watchChanges(
                    std::function<void (EventType)> inNext,
                    std::function<void (const std::string&)> inError = nullptr,
                    std::function<void ()> inComplete = nullptr
                )
                {
                    return m_observable->subscribe(
                        inNext,
                        inError,
                        inComplete
                    );
                }

            protected:
                std::map<std::string, I>               m_instances;
                std::map<std::string, E>               m_datum;
                std::vector<std::string>               m_activeIds;
                std::vector<std::string>               m_usedIds;
                std::unique_ptr<Observable<EventType>> m_observable;
            };
        }
    }
}