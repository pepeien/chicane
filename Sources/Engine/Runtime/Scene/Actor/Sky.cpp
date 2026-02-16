#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor(),
          m_asset(nullptr)
    {}

    const Box::Sky* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky* inSky)
    {
        m_asset = inSky;
    }
}