#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Manager/EventType.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    namespace Box
    {
        template <typename I, typename E>
        class CHICANE_BOX Manager
        {
        public:
            using EventObservable   = Observable<ManagerEventType>;
            using EventSubscription = Subscription<ManagerEventType>;

        protected:
            using Super = Manager;

        public:
            Manager()
                : m_observable({})
            {}

            virtual ~Manager() = default;

        protected:
            // Event
            virtual void onLoad(const String& inId, const I& inInstance) { return; }
            virtual void onAllocation(const String& inId, const E& inData) { return; }
            virtual void onDeallocation(const String& inId) { return; }
            virtual void onActivation(const String& inId) { return; }
            virtual void onDeactivation(const String& inId) { return; }

        public:
            // Status
            bool isEmpty() const { return m_instances.empty() || m_datum.empty(); }

            bool isLoaded(const String& inId) const { return m_instances.find(inId) != m_instances.end(); }

            bool isAllocated(const String& inId) const { return m_datum.find(inId) != m_datum.end(); }

            bool isActive(const String& inId) const
            {
                return std::find(m_activeIds.begin(), m_activeIds.end(), inId) != m_activeIds.end();
            }

            bool isUsing(const String& inId) const
            {
                return std::find(m_usedIds.begin(), m_usedIds.end(), inId) != m_usedIds.end();
            }

            // Data
            const std::vector<String>& getActiveIds() const { return m_activeIds; }

            const std::vector<String>& getUsedIds() const { return m_usedIds; }

            std::uint32_t getActiveCount() const { return static_cast<std::uint32_t>(m_activeIds.size()); }

            std::uint32_t getUsedCount() const { return m_usedIds.size(); }

            std::uint32_t getUseCount(const String& inId) const
            {
                if (!isUsing(inId))
                {
                    return 0;
                }

                return static_cast<std::uint32_t>(std::count(m_usedIds.begin(), m_usedIds.end(), inId));
            }

            // Lifecycle
            void load(const String& inId, const I& inInstance)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                m_instances.insert(std::make_pair(inId, inInstance));

                onLoad(inId, inInstance);

                m_observable.next(ManagerEventType::Load);
            }

            void allocate(const String& inId, const E& inData)
            {
                if (!isLoaded(inId) || isAllocated(inId))
                {
                    return;
                }

                m_datum.insert(std::make_pair(inId, inData));

                onAllocation(inId, inData);

                m_observable.next(ManagerEventType::Allocation);
            }

            void deallocate(const String& inId)
            {
                if (!isLoaded(inId) || isAllocated(inId))
                {
                    return;
                }

                m_datum.erase(inId);

                onDeallocation(inId);

                m_observable.next(ManagerEventType::Allocation);
            }

            void activate(const String& inId)
            {
                if (!isLoaded(inId))
                {
                    return;
                }

                m_usedIds.push_back(inId);
                std::sort(m_usedIds.begin(), m_usedIds.end(), [](const String& inA, const String& inB) {
                    return inA.compare(inB) > 0;
                });

                m_observable.next(ManagerEventType::Use);

                if (!isActive(inId))
                {
                    m_activeIds.push_back(inId);
                    std::sort(m_activeIds.begin(), m_activeIds.end(), [](const String& inA, const String& inB) {
                        return inA.compare(inB) > 0;
                    });

                    onActivation(inId);

                    m_observable.next(ManagerEventType::Activation);
                }
            }

            void deactivate(const String& inId)
            {
                if (!isLoaded(inId) || !isAllocated(inId) || !isActive(inId))
                {
                    return;
                }

                m_usedIds.erase(std::find(m_usedIds.begin(), m_usedIds.end(), inId));
                m_usedIds.shrink_to_fit();
                std::sort(m_usedIds.begin(), m_usedIds.end(), [](const String& inA, const String& inB) {
                    return inA.compare(inB) > 0;
                });

                m_observable.next(ManagerEventType::Use);

                if (isUsing(inId))
                {
                    return;
                }

                m_activeIds.erase(std::find(m_activeIds.begin(), m_activeIds.end(), inId));
                m_activeIds.shrink_to_fit();
                std::sort(m_activeIds.begin(), m_activeIds.end(), [](const String& inA, const String& inB) {
                    return inA.compare(inB) > 0;
                });

                m_observable.next(ManagerEventType::Activation);
            }

            // Events
            EventSubscription watchChanges(
                EventSubscription::NextCallback     inNext,
                EventSubscription::ErrorCallback    inError    = nullptr,
                EventSubscription::CompleteCallback inComplete = nullptr
            )
            {
                return m_observable.subscribe(inNext, inError, inComplete);
            }

        protected:
            std::map<String, I> m_instances;
            std::map<String, E> m_datum;
            std::vector<String> m_activeIds;
            std::vector<String> m_usedIds;
            EventObservable     m_observable;
        };
    }
}