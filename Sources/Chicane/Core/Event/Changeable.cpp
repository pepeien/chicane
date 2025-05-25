#include "Chicane/Core/Event/Changeable.hpp"

namespace Chicane
{
    Changeable::Changeable()
        : m_observable(std::make_unique<ChangesObservable>())
    {}

    void Changeable::emmitChanges()
    {
        m_observable->next();
    }

    Changeable::ChangesSubscription* Changeable::watchChanges(
        ChangesSubscription::EmptyCallback inNext,
        ChangesSubscription::ErrorCallback inError,
        ChangesSubscription::CompleteCallback inComplete
    ) const
    {
        return m_observable
            ->subscribe(inNext, inError, inComplete)
            ->next();
    }
}