#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"

namespace Chicane
{
    class CHICANE_CORE Changeable
    {
    public:
        using ChangesObservable   = EventObservable<>;
        using ChangesSubscription = EventSubscription<>;

    public:
        Changeable();
        virtual ~Changeable() = default;

    public:
        void emmitChanges();
        ChangesSubscription watchChanges(
            ChangesSubscription::EmptyCallback    inNext,
            ChangesSubscription::ErrorCallback    inError    = nullptr,
            ChangesSubscription::CompleteCallback inComplete = nullptr
        );

        template <typename T>
        void setValue(T& outProperty, T inNewValue)
        {
            if (outProperty == inNewValue)
            {
                return;
            }

            outProperty = inNewValue;

            emmitChanges();
        }

    private:
        ChangesObservable m_observable;
    };
}