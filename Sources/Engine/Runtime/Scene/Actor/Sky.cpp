#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

namespace Chicane
{
    ASky::ASky()
        : Super(),
          m_asset(nullptr),
          m_assetObservable({})
    {}

    const Box::Sky* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky* inSky)
    {
        m_asset = inSky;

        m_assetObservable.next(inSky);
    }

    ASky::AssetSubscription ASky::watchSky(
        AssetSubscription::NextCallback     inNext,
        AssetSubscription::ErrorCallback    inError,
        AssetSubscription::CompleteCallback inComplete
    )
    {
        return m_assetObservable.subscribe(inNext, inError, inComplete)
            .next(m_asset);
    }
}