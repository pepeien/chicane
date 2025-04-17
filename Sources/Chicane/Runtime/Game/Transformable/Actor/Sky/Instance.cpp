#include "Runtime/Game/Transformable/Actor/Sky/Instance.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor(),
        m_asset(nullptr),
        m_assetObservable(std::make_unique<Observable<const Box::Sky::Instance*>>())
    {}

    const Box::Sky::Instance* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky::Instance* inSky)
    {
        m_asset = inSky;

        m_assetObservable->next(inSky);
    }

    Subscription<const Box::Sky::Instance*>* ASky::watchSky(
        std::function<void (const Box::Sky::Instance*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<const Box::Sky::Instance*>* subscription = m_assetObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_asset);

        return subscription;
    }
}