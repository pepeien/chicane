#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    template<typename T>
    class Subscription
    {
    public:
        Subscription(
            std::function<void (T)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        ) : m_bIsCompleted(false),
            m_next(inNext),
            m_error(inError),
            m_complete(inComplete)
        {}

    public:
        void next(T inData)
        {
            if (isCompleted())
            {
                return;
            }

            if (!m_next)
            {
                return;
            }

            m_next(inData);
        }

        void error(const std::string& inMessage)
        {
            if (isCompleted())
            {
                return;
            }

            if (!m_error)
            {
                return;
            }

            m_error(inMessage);
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

            if (!m_complete)
            {
                return;
            }

            m_complete();
        }

    protected:
        bool m_bIsCompleted;

        std::function<void (T)> m_next;
        std::function<void (const std::string&)> m_error;
        std::function<void ()> m_complete;
    };
}