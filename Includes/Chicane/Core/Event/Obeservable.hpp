#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    template<typename T = void*>
    class Observable
    {
    public:
        using EmptyCallback    = std::function<void ()>;
        using NextCallback     = std::function<void (T)>;
        using ErrorCallback    = std::function<void (const std::string&)>;
        using CompleteCallback = std::function<void ()>;

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
            EmptyCallback inNext,
            ErrorCallback inError = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(
                new Subscription<T>(
                    inNext,
                    inError,
                    inComplete
                )
            );

            return m_subscriptions.back();
        }

        Subscription<T>* subscribe(
            NextCallback inNext,
            ErrorCallback inError = nullptr,
            CompleteCallback inComplete = nullptr
        )
        {
            m_subscriptions.push_back(
                new Subscription<T>(
                    inNext,
                    inError,
                    inComplete
                )
            );

            return m_subscriptions.back();
        }

        void next()
        {
            next(nullptr);
        }

        void next(T inData)
        {
            for (std::uint32_t i = 0; i < m_subscriptions.size(); i++)
            {
                m_subscriptions.at(i)->next(inData);
            }
        }

        void error(const std::string& inMessage)
        {
            for (std::uint32_t i = 0; i < m_subscriptions.size(); i++)
            {
                m_subscriptions.at(i)->error(inMessage);
            }
        }

        void complete()
        {
            for (std::uint32_t i = 0; i < m_subscriptions.size(); i++)
            {
                m_subscriptions.at(i)->complete();
            }
        }

    private:
        std::vector<Subscription<T>*> m_subscriptions;
    };
}