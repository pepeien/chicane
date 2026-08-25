#include "Chicane/Grid/Component/ProgressBar.reflected.hpp"

namespace Chicane
{
    namespace Grid
    {
        static int switchfer = 1;

        ProgressBar::ProgressBar(const pugi::xml_node& inNode)
            : Container(inNode),
              percentage(0.0f)
        {
            load("Assets/Engine/UI/Components/ProgressBar.grid", "Assets/Engine/UI/Components/ProgressBar.decal");
        }

        void ProgressBar::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshPercentage();
        }

        void ProgressBar::refreshPercentage()
        {
            const String value = parseText(getAttribute(PERCENTAGE_ATTRIBUTE_NAME));
            if (value.isEmpty())
            {
                percentage = 0.0f;

                return;
            }

            percentage = std::clamp(static_cast<float>(std::strtod(value.toChar(), nullptr)), 0.0f, 100.0f);
        }
    }
}