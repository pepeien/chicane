#include "Chicane/Core/Event/Changeable.hpp"

namespace Chicane
{
    Changeable::Changeable()
        : m_observable({})
    {}

    void Changeable::emmitChanges()
    {
        m_observable.next();
    }

    Changeable::ChangesSubscription Changeable::watchChanges(
        ChangesSubscription::EmptyCallback inNext,
        ChangesSubscription::ErrorCallback inError,
        ChangesSubscription::CompleteCallback inComplete
    )
    {
        return m_observable
            .subscribe(inNext, inError, inComplete)
            .next();
    }
}