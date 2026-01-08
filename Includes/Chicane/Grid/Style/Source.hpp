#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSource
        {
        public:
            using Map  = std::unordered_map<String, String>;
            using List = std::vector<StyleSource>;

        public:
            StyleSource(const std::vector<String>& inSelectors, const Map& inProperties);
            StyleSource();

        public:
            bool isEmpty() const;

        public:
            std::vector<String> selectors;
            Map                 properties;
        };
    }
}