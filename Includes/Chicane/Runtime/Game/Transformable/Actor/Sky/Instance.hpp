#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ASky : public Actor
    {
    public:
        ASky();
        ASky(ASky&& other) = default;
        virtual ~ASky() = default;

        ASky& operator=(ASky&& other) = default;

    public:
        const Box::Sky::Instance* getSky() const;
        void setSky(const Box::Sky::Instance* inSky);
        Subscription<const Box::Sky::Instance*>* watchSky(
            std::function<void (const Box::Sky::Instance*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    protected:
        const Box::Sky::Instance*                              m_asset;
        std::unique_ptr<Observable<const Box::Sky::Instance*>> m_assetObservable;
    };
}