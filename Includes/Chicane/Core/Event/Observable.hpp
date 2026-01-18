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
        ~EventObservable() { m_subscriptions.clear(); }

    public:
        EventSubscription<T> subscribe(
            EmptyCallback    inNext,
            ErrorCallback    inError    = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(EventSubscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        EventSubscription<T> subscribe(
            NextCallback     inNext,
            ErrorCallback    inError    = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(EventSubscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        void next() { next(nullptr); }

        void next(T inData)
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.next(inData);
            }
        }

        void error(const String& inMessage)
        {
            for (EventSubscription<T>& subscription : m_subscriptions)
            {
                subscription.error(inMessage);
            }
        }

        void complete()
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