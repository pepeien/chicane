#include "Chicane/Grid/Component/Progress/Bar.reflected.hpp"

#include <algorithm>
#include <cstdlib>

namespace Chicane
{
    namespace Grid
    {
        ProgressBar::ProgressBar(const XmlNode& inNode)
            : Container(inNode),
              percentage(0.0f)
        {
            load(
                "Assets/Engine/UI/Components/Progress/Bar/Index.grid",
                "Assets/Engine/UI/Components/Progress/Bar/Index.decal"
            );
        }

        void ProgressBar::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            if (!isReference(getAttribute(PERCENTAGE_ATTRIBUTE_NAME)) && !hasFlag(ComponentDirty::LaidOut))
            {
                return;
            }

            refreshPercentage();
        }

        void ProgressBar::refreshPercentage()
        {
            const String value = parseText(getAttribute(PERCENTAGE_ATTRIBUTE_NAME)).trim();
            if (value.isEmpty())
            {
                percentage = 0.0f;

                return;
            }

            percentage = std::clamp(static_cast<float>(std::strtod(value.toChar(), nullptr)), 0.0f, 100.0f);
        }
    }
}
