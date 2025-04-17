#pragma once

#include "Core.hpp"
#include "Box/Asset/Sky.hpp"
#include "Runtime/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    class CHICANE ASky : public Actor
    {
    public:
        ASky();
        virtual ~ASky() = default;

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