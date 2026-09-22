#include "Chicane/Core/Xml/Node/Type.hpp"
#include "Chicane/Core/Xml/Node/Type.reflected.hpp"

namespace Chicane
{
    String toString(XmlNodeType inValue)
    {
        switch (inValue)
        {
        case XmlNodeType::Null:
            return "Null";

        case XmlNodeType::Element:
            return "Element";

        case XmlNodeType::Pcdata:
            return "Pcdata";

        case XmlNodeType::Cdata:
            return "Cdata";

        case XmlNodeType::Comment:
            return "Comment";

        case XmlNodeType::Other:
            return "Other";

        default:
            return "";
        }
    }
}
