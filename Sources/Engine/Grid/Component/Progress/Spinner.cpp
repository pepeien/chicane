#include "Chicane/Grid/Component/Progress/Spinner.reflected.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Grid
    {
        ProgressSpinner::ProgressSpinner(const XmlNode& inNode)
            : Container(inNode),
              bIsSpinning(true)
        {
            load(
                "Assets/Engine/UI/Components/Progress/Spinner.grid",
                "Assets/Engine/UI/Components/Progress/Spinner.decal"
            );

            watchAttribute(
                IS_SPINNING_ATTRIBUTE_NAME,
                [this](const String& inValue) { bIsSpinning = Xml::parseBool(parseText(inValue).trim(), true); }
            );
        }
    }
}
