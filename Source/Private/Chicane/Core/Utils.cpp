#include "Chicane/Core/Utils.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Utils
    {
        std::string trim(const std::string& inTarget)
        {
            std::string result = inTarget;
            result.erase(result.find_last_not_of(' ') + 1);
            result.erase(0, result.find_first_not_of(' '));

            return result;
        }

        std::vector<std::string> split(const std::string& inTarget, char inDelimeter)
        {
            std::stringstream stream(inTarget);

            std::vector<std::string> result {};
            std::string item                = "";

            while (getline(stream, item, inDelimeter))
            {
                result.push_back(item);
            }

            return result;
        }
    }
}