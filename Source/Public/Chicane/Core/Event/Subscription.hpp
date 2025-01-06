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
        ) : m_bIsCompleted(false),
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

            if (!m_nextCallback)
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

            if (!m_errorCallback)
            {
                return;
            }

            m_errorCallback(inMessage);
        }

        bool isCompleted() const
        {
            return m_bIsCompleted;
        }

        void complete()
        {
            if (isCompleted())
            {
                return;
            }

            m_bIsCompleted = true;

            if (!m_completeCallback)
            {
                return;
            }

            m_completeCallback();
        }

    protected:
        bool m_bIsCompleted;

        std::function<void (T)> m_nextCallback;
        std::function<void (const std::string&)> m_errorCallback;
        std::function<void ()> m_completeCallback;
    };
}