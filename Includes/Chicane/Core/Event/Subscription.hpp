#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    template<typename T = void*>
    class Subscription
    {
    public:
        using EmptyCallback    = std::function<void ()>;
        using NextCallback     = std::function<void (T)>;
        using ErrorCallback    = std::function<void (const std::string&)>;
        using CompleteCallback = std::function<void ()>;

    public:
        Subscription(EmptyCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : Subscription(
                [inNext](T inValue) { inNext(); },
                inError,
                inComplete
            )
        {}

        Subscription(NextCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : m_bIsCompleted(false),
            m_next(inNext),
            m_error(inError),
            m_complete(inComplete)
        {}

    public:
        Subscription<T>* next()
        {
            return next(nullptr);
        }

        Subscription<T>* next(T inData)
        {
            if (isCompleted())
            {
                return this;
            }

            if (!m_next)
            {
                return this;
            }

            m_next(inData);

            return this;
        }

        Subscription<T>* error(const std::string& inMessage)
        {
            if (isCompleted())
            {
                return this;
            }

            if (!m_error)
            {
                return this;
            }

            m_error(inMessage);

            return this;
        }

        bool isCompleted() const
        {
            return m_bIsCompleted;
        }

        Subscription<T>* complete()
        {
            if (isCompleted())
            {
                return this;
            }

            m_bIsCompleted = true;

            if (!m_complete)
            {
                return this;
            }

            m_complete();

            return this;
        }

    private:
        bool             m_bIsCompleted;

        NextCallback     m_next;
        ErrorCallback    m_error;
        CompleteCallback m_complete;
    };
}