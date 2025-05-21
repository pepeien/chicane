#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Event/Obeservable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    class CHICANE_CORE Changeable
    {
    public:
        Changeable();
        virtual ~Changeable() = default;

    public:
        void emmitChanges();
        Subscription<>* watchChanges(
            std::function<void ()> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        ) const;

        template<typename T>
        void setProperty(T& outProperty, T inNewValue)
        {
            if (outProperty == inNewValue)
            {
                return;
            }

            outProperty = inNewValue;

            emmitChanges();
        }

    private:
        std::unique_ptr<Observable<>> m_observable;
    };
}