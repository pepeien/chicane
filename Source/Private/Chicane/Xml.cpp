#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace XML
    {
        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        )
        {
            return inNode.attribute(inName.c_str());
        }
    }
}