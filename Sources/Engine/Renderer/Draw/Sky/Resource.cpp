#include "Chicane/Renderer/Draw/Sky/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        bool DrawSkyResource::isEmpty() const
        {
            return m_draw.textures.empty() || m_draw.model.reference.isEmpty();
        }

        Draw::Id DrawSkyResource::findId(const Draw::Reference& inValue) const
        {
            if (inValue.isEmpty())
            {
                return Draw::InvalidId;
            }

            return m_draw.reference.equals(inValue) ? m_draw.id : Draw::InvalidId;
        }

        const DrawSky& DrawSkyResource::getDraw() const
        {
            return m_draw;
        }

        Draw::Id DrawSkyResource::add(const DrawSky& inData)
        {
            const Draw::Id id = findId(inData.reference);

            if (id > Draw::InvalidId)
            {
                return id;
            }

            m_draw.id        = 1;
            m_draw.reference = inData.reference;
            m_draw.model     = inData.model;
            m_draw.textures  = inData.textures;

            markAsDirty();

            return m_draw.id;
        }

        void DrawSkyResource::reset()
        {
            m_draw = {};

            clearHashes();
        }
    }
}