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
        ) : m_isCompleted(false),
            m_nextCallback(inNextCallback),
            m_errorCallback(inErrorCallback),
            m_completeCallback(inCompleteCallback)
        {}

    public:
        void next(const T& inData)
        {
            if (isCompleted())
            {
                return;
            }

            if (m_nextCallback == nullptr)
            {
                return;
            }

            m_nextCallback(inData);
        }

        void error(const std::string& inMessage)
        {
            if (isCompleted())
            {
                return;
            }

            if (m_errorCallback == nullptr)
            {
                return;
            }

            m_errorCallback(inMessage);
        }

        bool isCompleted() const
        {
            return m_isCompleted;
        }

        void complete()
        {
            if (isCompleted())
            {
                return;
            }

            m_isCompleted = true;

            if (m_completeCallback == nullptr)
            {
                return;
            }

            m_completeCallback();
        }

    protected:
        bool m_isCompleted;

        std::function<void (T)> m_nextCallback;
        std::function<void (const std::string&)> m_errorCallback;
        std::function<void ()> m_completeCallback;
    };

    template<typename A>
    class Observable
    {
    public:
        ~Observable()
        {
            for (Subscription<A>* subscription : m_subscriptions)
            {
                delete subscription;
            }

            m_subscriptions.clear();
        }

    public:
        Subscription<A>* subscribe(
            std::function<void (A)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        )
        {
            m_subscriptions.push_back(
                new Subscription<A>(
                    inNextCallback,
                    inErrorCallback,
                    inCompleteCallback
                )
            );

            return m_subscriptions.back();
        }

        void next(const A& inData)
        {
            for (Subscription<A>* subscription : m_subscriptions)
            {
                subscription->next(inData);
            }
        }

        void error(const std::string& inMessage)
        {
            for (Subscription<A>* subscription : m_subscriptions)
            {
                subscription->error(inMessage);
            }
        }

        void complete()
        {
            for (Subscription<A>* subscription : m_subscriptions)
            {
                subscription->complete();
            }
        }

    protected:
        std::vector<Subscription<A>*> m_subscriptions;
    };
}