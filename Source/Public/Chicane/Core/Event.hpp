#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    template<typename T>
    class Subscription
    {
    public:
        Subscription(
            std::function<void (T)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        ) : m_nextCallback(inNextCallback),
            m_errorCallback(inErrorCallback),
            m_completeCallback(inCompleteCallback)
        {}

    public:
        void next(const T& inData)
        {
            if (m_nextCallback == nullptr)
            {
                return;
            }

            m_nextCallback(inData);
        }

        void error(const std::string& inMessage)
        {
            if (m_errorCallback == nullptr)
            {
                return;
            }

            m_errorCallback(inMessage);
        }

        void complete()
        {
            if (m_completeCallback == nullptr)
            {
                return;
            }

            m_completeCallback();
        }

    protected:
        std::function<void (T)> m_nextCallback;
        std::function<void (const std::string&)> m_errorCallback;
        std::function<void ()> m_completeCallback;
    };

    template<typename A>
    class Observable
    {
    public:
        Subscription<A>* subscribe(
            std::function<void (A)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        )
        {
            Subscription<A>* newSubscription = new Subscription<A>(
                inNextCallback,
                inErrorCallback,
                inCompleteCallback
            );
            m_subscriptions.push_back(newSubscription);

            return newSubscription;
        }

        void next(const A& inData)
        {
            for (Subscription<A>* subcription : m_subscriptions)
            {
                subcription->next(inData);
            }
        }

        void error(const std::string& inMessage)
        {
            for (Subscription<A>* subcription : m_subscriptions)
            {
                subcription->error(inMessage);
            }
        }

        void complete()
        {
            for (Subscription<A>* subcription : m_subscriptions)
            {
                subcription->complete();
            }
        }

    protected:
        std::vector<Subscription<A>*> m_subscriptions;
    };
}