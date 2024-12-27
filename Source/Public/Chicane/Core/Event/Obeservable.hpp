#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    template<typename T>
    class Observable
    {
    public:
        ~Observable()
        {
            for (Subscription<T>* subscription : m_subscriptions)
            {
                delete subscription;
            }

            m_subscriptions.clear();
        }

    public:
        Subscription<T>* subscribe(
            std::function<void (T)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        )
        {
            m_subscriptions.push_back(
                new Subscription<T>(
                    inNextCallback,
                    inErrorCallback,
                    inCompleteCallback
                )
            );

            return m_subscriptions.back();
        }

        void next(const T& inData)
        {
            for (Subscription<T>* subscription : m_subscriptions)
            {
                subscription->next(inData);
            }
        }

        void error(const std::string& inMessage)
        {
            for (Subscription<T>* subscription : m_subscriptions)
            {
                subscription->error(inMessage);
            }
        }

        void complete()
        {
            for (Subscription<T>* subscription : m_subscriptions)
            {
                subscription->complete();
            }
        }

    protected:
        std::vector<Subscription<T>*> m_subscriptions;
    };
}