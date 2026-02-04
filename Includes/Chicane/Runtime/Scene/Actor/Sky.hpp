#pragma once

#include "Chicane/Box/Sky.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        using AssetObservable   = EventObservable<const Box::Sky*>;
        using AssetSubscription = EventSubscription<const Box::Sky*>;

    public:
        ASky();

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);
        AssetSubscription watchSky(
            AssetSubscription::NextCallback     inNext,
            AssetSubscription::ErrorCallback    inError    = nullptr,
            AssetSubscription::CompleteCallback inComplete = nullptr
        );

    protected:
        const Box::Sky* m_asset;
        AssetObservable m_assetObservable;
    };
}