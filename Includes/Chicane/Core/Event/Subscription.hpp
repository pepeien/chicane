#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template <typename T = void*>
    class CHICANE_CORE EventSubscription
    {
    public:
        using EmptyCallback    = std::function<void()>;
        using NextCallback     = std::function<void(T)>;
        using ErrorCallback    = std::function<void(const String&)>;
        using CompleteCallback = std::function<void()>;

    public:
        EventSubscription(EmptyCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : EventSubscription([inNext](T inValue) { inNext(); }, inError, inComplete)
        {}

        EventSubscription(NextCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : m_bIsCompleted(false),
              m_next(inNext),
              m_error(inError),
              m_complete(inComplete)
        {}

        EventSubscription()
            : m_bIsCompleted(false),
              m_next(nullptr),
              m_error(nullptr),
              m_complete(nullptr)
        {}

    public:
        EventSubscription<T> next() { return next(nullptr); }

        EventSubscription<T> next(T inData)
        {
            if (isCompleted())
            {
                return *this;
            }

            if (m_next)
            {
                m_next(inData);
            }

            return *this;
        }

        EventSubscription<T> error(const String& inMessage)
        {
            if (isCompleted())
            {
                return *this;
            }

            if (m_error)
            {
                m_error(inMessage);
            }

            return *this;
        }

        bool isCompleted() const { return m_bIsCompleted; }

        EventSubscription<T> complete()
        {
            if (isCompleted())
            {
                return *this;
            }

            m_bIsCompleted = true;

            if (m_complete)
            {
                m_complete();
            }

            return *this;
        }

    private:
        bool             m_bIsCompleted;

        NextCallback     m_next;
        ErrorCallback    m_error;
        CompleteCallback m_complete;
    };
}