#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template<typename T = void*>
    class Observable
    {
    public:
        using EmptyCallback    = std::function<void ()>;
        using NextCallback     = std::function<void (T)>;
        using ErrorCallback    = std::function<void (const String&)>;
        using CompleteCallback = std::function<void ()>;

    public:
        ~Observable()
        {
            m_subscriptions.clear();
        }

    public:
        Subscription<T> subscribe(
            EmptyCallback inNext,
            ErrorCallback inError = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(Subscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        Subscription<T> subscribe(
            NextCallback inNext,
            ErrorCallback inError = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(Subscription<T>(inNext, inError, inComplete));

            return m_subscriptions.back();
        }

        void next()
        {
            next(nullptr);
        }

        void next(T inData)
        {
            for (Subscription<T>& subscription : m_subscriptions)
            {
                subscription.next(inData);
            }
        }

        void error(const String& inMessage)
        {
            for (Subscription<T>& subscription : m_subscriptions)
            {
                subscription.error(inMessage);
            }
        }

        void complete()
        {
            for (Subscription<T>& subscription : m_subscriptions)
            {
                subscription.complete();
            }
        }

    private:
        std::vector<Subscription<T>> m_subscriptions;
    };
}