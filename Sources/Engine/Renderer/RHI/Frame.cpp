#include "Chicane/Renderer/RHI/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            void Frame::addBindGroup(const String& inId, BindGroup inGroup)
            {
                m_bindGroups[inId] = inGroup;
            }

            BindGroup Frame::getBindGroup(const String& inId) const
            {
                auto found = m_bindGroups.find(inId);
                if (found == m_bindGroups.end())
                {
                    return {};
                }

                return found->second;
            }
        }
    }
}
