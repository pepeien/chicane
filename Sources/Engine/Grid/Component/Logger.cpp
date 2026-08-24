#include "Chicane/Grid/Component/Logger.reflected.hpp"

namespace Chicane
{
    namespace Grid
    {
        Logger::Logger(const pugi::xml_node& inNode)
            : Component(inNode),
              logs({})
        {
            load("Assets/Engine/UI/Components/Logger.grid", "Assets/Engine/UI/Components/Logger.decal");
        }

        void Logger::onTick(float inDeltaTime)
        {
            Log::List current = Log::getLogs();
            if (current.size() == logs.size() &&
                (current.empty() || logs.empty() || current.back().text.equals(logs.back().text)))
            {
                return;
            }

            logs = std::move(current);
        }
    }
}
