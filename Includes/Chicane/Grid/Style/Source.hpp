#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            using Properties = std::unordered_map<String, String>;

            struct CHICANE_GRID Source
            {
            public:
                using List = std::vector<Source>;

            public:
                Source(const std::vector<String>& inSelectors, const Properties& inProperties);
                Source();

            public:
                bool isEmpty() const;

            public:
                std::vector<String> selectors;
                Properties               properties;
            };
        }
    }
}