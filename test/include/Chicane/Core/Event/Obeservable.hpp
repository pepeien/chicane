#pragma once

#include "Chicane/Core/Essentials.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    template<typename T>
    class CHICANE Observable
    {
    public:
        ~Observable()
        {
            for (std::uint32_t i = 0; i < m_subscriptions.size(); i++)
            {
                m_subscriptions.at(i).release();
            }

            m_subscriptions.clear();
        }

    public:
        Subscription<T>* subscribe(
            std::function<void (T)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        )
        {
            m_subscriptions.push_back(
                std::make_unique<Subscription<T>>(
                    inNext,
                    inError,
                    inComplete
                )
            );

            return m_subscriptions.back().get();
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

    protected:
        std::vector<std::unique_ptr<Subscription<T>>> m_subscriptions;
    };
}