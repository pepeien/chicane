#pragma once

#include <algorithm>
#include <map>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Manager/Event.hpp"

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    namespace Box
    {
        template <typename Data, typename Instance>
        class CHICANE_BOX Manager
        {
        public:
            using Instances             = std::map<String, Instance>;
            using InstancesObservable   = EventObservable<Instances>;
            using InstancesSubscription = EventSubscription<Instances>;

        public:
            Manager()
                : m_instances({}),
                  m_instancesObservable({})
            {}

            virtual ~Manager() = default;

        protected:
            virtual void onLoad(const String& inId, const Data& inData) { return; }

        public:
            bool isEmpty() const { return m_instances.empty(); }

            bool isLoaded(const String& inId) const { return m_instances.find(inId) != m_instances.end(); }

            Instance get(const String& inId) const
            {
                if (!isLoaded(inId))
                {
                    return {};
                }

                return m_instances.at(inId);
            }

            void load(const String& inId, const Data& inData)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                onLoad(inId, inData);
            }

            InstancesSubscription watch(
                typename InstancesSubscription::NextCallback     inNext,
                typename InstancesSubscription::ErrorCallback    inError    = nullptr,
                typename InstancesSubscription::CompleteCallback inComplete = nullptr
            )
            {
                return m_instancesObservable.subscribe(inNext, inError, inComplete).next(m_instances);
            }

        protected:
            void add(const String& inId, Instance inInstance)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                m_instances.insert(std::make_pair(inId, inInstance));

                m_instancesObservable.next(m_instances);
            }

        protected:
            Instances           m_instances;
            InstancesObservable m_instancesObservable;
        };
    }
}