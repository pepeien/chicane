#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Event/Obeservable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    class CHICANE_CORE Changeable
    {
    public:
        using ChangesObservable   = Observable<>;
        using ChangesSubscription = Subscription<>;

    public:
        Changeable();
        virtual ~Changeable() = default;

    public:
        void emmitChanges();
        ChangesSubscription* watchChanges(
            ChangesSubscription::EmptyCallback inNext,
            ChangesSubscription::ErrorCallback inError = nullptr,
            ChangesSubscription::CompleteCallback inComplete = nullptr
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
        std::unique_ptr<ChangesObservable> m_observable;
    };
}