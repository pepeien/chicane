#include "Chicane/Renderer/Backend/OpenGL/Frame.hpp"

#include <stdexcept>

namespace Chicane
{
    namespace Renderer
    {
        void OpenGLFrame::addObject(const String& inId, std::uint32_t inObject)
        {
            if (objects.find(inId) != objects.end())
            {
                throw std::runtime_error("Object " + inId + " already exists");
            }

            objects.insert(std::make_pair(inId, inObject));
        }

        std::uint32_t OpenGLFrame::getObject(const String& inId) const
        {
            return objects.at(inId);
        }

        void OpenGLFrame::removeObject(const String& inId)
        {
            objects.erase(inId);
        }
    }
}
