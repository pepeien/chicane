#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        using AssetObservable   = Observable<const Box::Sky::Instance*>;
        using AssetSubscription = Subscription<const Box::Sky::Instance*>;

    public:
        ASky();
        ASky(ASky&& other) = default;
        virtual ~ASky() = default;

        ASky& operator=(ASky&& other) = default;

    public:
        const Box::Sky::Instance* getSky() const;
        void setSky(const Box::Sky::Instance* inSky);
        AssetSubscription* watchSky(
            AssetSubscription::NextCallback inNext,
            AssetSubscription::ErrorCallback inError = nullptr,
            AssetSubscription::CompleteCallback inComplete = nullptr
        );

    protected:
        const Box::Sky::Instance*        m_asset;
        std::unique_ptr<AssetObservable> m_assetObservable;
    };
}