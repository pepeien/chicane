#include "Chicane/Grid/Component/Loading.hpp"

namespace Chicane
{
    namespace Grid
    {
        Loading::Loading(std::vector<String>& inStack, const String& inPath)
            : stack(inStack)
        {
            stack.push_back(inPath);
        }

        Loading::~Loading()
        {
            stack.pop_back();
        }
    }
}
