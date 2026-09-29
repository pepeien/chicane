#include "Chicane/Grid/Component/Logger.reflected.hpp"

namespace Chicane
{
    namespace Grid
    {
        Logger::Logger(const XmlNode& inNode)
            : Container(inNode),
              logs({})
        {
            load("Assets/Engine/UI/Components/Logger/Index.grid", "Assets/Engine/UI/Components/Logger/Index.decal");
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
