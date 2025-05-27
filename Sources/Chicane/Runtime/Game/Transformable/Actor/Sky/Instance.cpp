#include "Chicane/Runtime/Game/Transformable/Actor/Sky/Instance.hpp"

namespace Chicane
{
    ASky::ASky()
        : Super(),
        m_asset(nullptr),
        m_assetObservable({})
    {}

    const Box::Sky::Instance* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky::Instance* inSky)
    {
        m_asset = inSky;

        m_assetObservable.next(inSky);
    }

    ASky::AssetSubscription ASky::watchSky(
        AssetSubscription::NextCallback inNext,
        AssetSubscription::ErrorCallback inError,
        AssetSubscription::CompleteCallback inComplete
    )
    {
        return m_assetObservable
            .subscribe(inNext, inError, inComplete)
            .next(m_asset);
    }
}