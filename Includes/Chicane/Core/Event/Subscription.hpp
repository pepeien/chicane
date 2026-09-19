#pragma once

#include <functional>
#include <memory>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Subscription/State.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template <typename T = void*>
    class EventSubscription
    {
    public:
        using EmptyCallback    = std::function<void()>;
        using NextCallback     = std::function<void(T)>;
        using ErrorCallback    = std::function<void(const String&)>;
        using CompleteCallback = std::function<void()>;

    public:
        inline EventSubscription(EmptyCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : EventSubscription([inNext](const T&) { inNext(); }, inError, inComplete)
        {}

        inline EventSubscription(NextCallback inNext, ErrorCallback inError, CompleteCallback inComplete)
            : m_state(std::make_shared<EventSubscriptionState<T>>())
        {
            m_state->next     = inNext;
            m_state->error    = inError;
            m_state->complete = inComplete;
        }

        inline EventSubscription()
            : m_state(std::make_shared<EventSubscriptionState<T>>())
        {}

    public:
        inline EventSubscription<T> next() { return next(nullptr); }

        inline EventSubscription<T> next(const T& inData)
        {
            if (isCompleted())
            {
                return *this;
            }

            if (m_state->next)
            {
                m_state->next(inData);
            }

            return *this;
        }

        inline EventSubscription<T> error(const String& inMessage)
        {
            if (isCompleted())
            {
                return *this;
            }

            if (m_state->error)
            {
                m_state->error(inMessage);
            }

            return *this;
        }

        inline bool isCompleted() const { return !m_state || m_state->bIsCompleted; }

        inline EventSubscription<T> complete()
        {
            if (isCompleted())
            {
                return *this;
            }

            m_state->bIsCompleted = true;

            if (m_state->complete)
            {
                m_state->complete();
            }

            return *this;
        }

    private:
        std::shared_ptr<EventSubscriptionState<T>> m_state;
    };
}
