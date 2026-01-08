#pragma once

#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        using AssetObservable   = Observable<const Box::Sky*>;
        using AssetSubscription = Subscription<const Box::Sky*>;

    public:
        ASky();
        ASky(ASky&& other) = default;
        virtual ~ASky() = default;

        ASky& operator=(ASky&& other) = default;

    public:
        const Box::Sky* getSky() const;
        void setSky(const Box::Sky* inSky);
        AssetSubscription watchSky(
            AssetSubscription::NextCallback inNext,
            AssetSubscription::ErrorCallback inError = nullptr,
            AssetSubscription::CompleteCallback inComplete = nullptr
        );

    protected:
        const Box::Sky* m_asset;
        AssetObservable m_assetObservable;
    };
}