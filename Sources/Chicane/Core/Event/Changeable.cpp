#include "Chicane/Core/Event/Changeable.hpp"

namespace Chicane
{
    Changeable::Changeable()
        : m_observable(std::make_unique<Observable<>>())
    {}

    void Changeable::emmitChanges()
    {
        m_observable->next();
    }

    Subscription<>* Changeable::watchChanges(
        std::function<void ()> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    ) const
    {
        Subscription<>* subscription = m_observable->subscribe(inNext, inError, inComplete);
        subscription->next();

        return subscription;
    }
}