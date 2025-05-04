#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            typedef std::unordered_map<std::string, std::string> Properties;

            struct CHICANE Source
            {
            public:
                bool isEmpty() const;

            public:
                std::string selector;
                Properties  properties;
            };

            typedef std::vector<Source> Sources;
        }
    }
}