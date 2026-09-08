#include "Chicane/Grid/Component/Progress/Spinner.reflected.hpp"

namespace Chicane
{
    namespace Grid
    {
        ProgressSpinner::ProgressSpinner(const pugi::xml_node& inNode)
            : Container(inNode),
              isSpinning(true),
              spinState(STATE_SPINNING)
        {
            load(
                "Assets/Engine/UI/Components/Progress/Spinner.grid",
                "Assets/Engine/UI/Components/Progress/Spinner.decal"
            );
        }

        void ProgressSpinner::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshSpinning();
        }

        void ProgressSpinner::refreshSpinning()
        {
            isSpinning = parseSpinning(getAttribute(SPINNING_ATTRIBUTE_NAME), true);
            spinState  = isSpinning ? STATE_SPINNING : STATE_IDLE;
        }

        bool ProgressSpinner::parseSpinning(const String& inValue, bool inFallback) const
        {
            const String value = parseText(inValue).trim().toLower();
            if (value.isEmpty())
            {
                return inFallback;
            }

            if (value.equals("true", "1", "yes", "spinning"))
            {
                return true;
            }

            if (value.equals("false", "0", "no", "idle"))
            {
                return false;
            }

            return inFallback;
        }
    }
}
