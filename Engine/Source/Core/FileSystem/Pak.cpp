#include "Pak.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        namespace Pak
        {
            void write(const std::vector<std::string>& inInputPaths, const std::string& inOutputPath)
            {
                for (std::string path : inInputPaths)
                {
                    Log::info(path);
                }
            }
        }
    }
}