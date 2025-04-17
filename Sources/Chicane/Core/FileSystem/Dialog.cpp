#include "Core/FileSystem/Dialog.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        std::vector<Dialog::Result> Dialog::Result::fromRaw(const char* const* inData)
        {
            std::vector<Result> result = {};

            const char* const* currentData = inData;

            while (true)
            {
                if (!currentData || !*currentData)
                {
                    break;
                }

                Result block = {};
                block.path = *currentData;

                result.push_back(block);

                currentData++;
            }

            return result;
        }
    }
}