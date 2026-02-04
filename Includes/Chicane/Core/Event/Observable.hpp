#pragma once

#include <functional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template <typename T = void*>
    class CHICANE_CORE EventObservable
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

        inline void next() { next(nullptr); }

        inline void next(T inData)
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.next(inData);
            }
        }

        inline void error(const String& inMessage)
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.error(inMessage);
            }
        }

        inline void complete()
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.complete();
            }
        }

    private:
        std::vector<EventSubscription<T>> m_subscriptions;
    };
}