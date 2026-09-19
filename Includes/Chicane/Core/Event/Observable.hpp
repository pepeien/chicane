#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template <typename T = void*>
    class EventObservable
    {
    public:
        using EmptyCallback    = std::function<void()>;
        using NextCallback     = std::function<void(T)>;
        using ErrorCallback    = std::function<void(const String&)>;
        using CompleteCallback = std::function<void()>;

    public:
        inline EventObservable()
            : m_subscriptions({})
        {}

        inline ~EventObservable() { m_subscriptions.clear(); }

    public:
        inline EventSubscription<T> subscribe(
            EmptyCallback inNext, ErrorCallback inError = nullptr, CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(EventSubscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        inline EventSubscription<T> subscribe(
            NextCallback inNext, ErrorCallback inError = nullptr, CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(EventSubscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        inline bool isEmpty() const
        {
            for (const EventSubscription<T>& subscription : m_subscriptions)
            {
                if (!subscription.isCompleted())
                {
                    return false;
                }
            }

            return true;
        }

        inline void next() { next(nullptr); }

        inline void next(const T& inData)
        {
            if (m_subscriptions.empty())
            {
                return;
            }

            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                if (!subscription.isCompleted())
                {
                    subscription.next(inData);
                }
            }

            compact();
        }

        inline void error(const String& inMessage)
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                if (!subscription.isCompleted())
                {
                    subscription.error(inMessage);
                }
            }

            compact();
        }

        inline void complete()
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.complete();
            }

            compact();
        }

    private:
        inline void compact()
        {
            m_subscriptions.erase(
                std::remove_if(
                    m_subscriptions.begin(),
                    m_subscriptions.end(),
                    [](const EventSubscription<T>& subscription) { return subscription.isCompleted(); }
                ),
                m_subscriptions.end()
            );
        }

    private:
        std::vector<EventSubscription<T>> m_subscriptions;
    };
}
