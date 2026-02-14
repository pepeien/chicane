#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleRuleset
        {
        public:
            using Selectors  = std::vector<String>;
            using Properties = std::unordered_map<String, String>;
            using List       = std::vector<StyleRuleset>;

        public:
            static String normalizeData(const String& inValue);

        public:
            StyleRuleset();

        public:
            inline friend bool operator==(const StyleRuleset& inA, const StyleRuleset& inB)
            {
                if (inA.selectors.size() != inB.selectors.size())
                {
                    return false;
                }

                for (std::uint32_t i = 0; i < inA.selectors.size(); i++)
                {
                    if (!inA.selectors.at(i).equals(inB.selectors.at(i)))
                    {
                        return false;
                    }
                }

                return true;
            }

        public:
            bool isEmpty() const;

            void addSelectors(const String& inValue);

            void addProperties(const String& inValue);
            void addProperties(const Properties& inProperties);

        public:
            Selectors  selectors;
            Properties properties;
        };
    }
}