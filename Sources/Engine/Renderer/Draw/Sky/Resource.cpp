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

            m_draw.reference = inData.reference;
            m_draw.model     = inData.model;
            m_draw.textures  = inData.textures;
            m_draw.kind      = inData.kind;

            if (id > Draw::InvalidId)
            {
                m_draw.id = id;
                markAsDirty();

                return id;
            }

            m_draw.id = 1;
            markAsDirty();

            return m_draw.id;
        }

        void DrawSkyResource::reset()
        {
            if (!isVolatile() && !m_draw.bIsVolatile)
            {
                return;
            }

            m_draw = {};

            clearHashes();
            markAsDirty();
        }
    }
}