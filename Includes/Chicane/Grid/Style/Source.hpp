#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        using StyleProperties = std::unordered_map<String, String>;

        struct CHICANE_GRID StyleSource
        {
        public:
            using List = std::vector<StyleSource>;

        public:
            StyleSource(const std::vector<String>& inSelectors, const StyleProperties& inProperties);
            StyleSource();

        public:
            bool isEmpty() const;

        public:
            std::vector<String> selectors;
            StyleProperties     properties;
        };
    }
}