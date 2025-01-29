#include "Chicane/Game/Transformable/Actor/Sky/Instance.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor()
    {}

    const Box::Sky::Instance* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky::Instance* inSky)
    {
        m_asset = inSky;
    }
}