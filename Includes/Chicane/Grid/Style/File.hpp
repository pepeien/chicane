#pragma once

#include <vector>
#include <unordered_map>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Import.hpp"
#include "Chicane/Grid/Style/Keyframe.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFile
        {
        public:
            using Variables = std::unordered_map<String, String>;
            using Keyframes = std::unordered_map<String, StyleKeyframe::List>;

        public:
            static inline const StyleFile& empty()
            {
                static const StyleFile result;

                return result;
            }

        public:
            StyleFile();

        public:
            void parse(const FileSystem::Path& inLocation);
            void parse(const String& inValue);

            const StyleImport::List& getImports() const;
            void addImport(const String& inValue);
            void addImport(const StyleImport::List& inValue);
            void addImport(const StyleImport& inValue);

            const Variables& getVariables() const;
            const String& getVariable(const String& inName) const;
            void addVariable(const String& inValue);
            void addVariable(const Variables& inValue);
            void addVariable(const String& inName, const String& inValue);

            const StyleRuleset::List& getRulesets() const;
            void addRuleset(const String& inValue);
            void addRuleset(const StyleRuleset::List& inValue);
            void addRuleset(const StyleRuleset& inValue);

            bool hasKeyframes(const String& inName) const;
            const StyleKeyframe::List& getKeyframes(const String& inName) const;

        private:
            StyleImport::List extractImports(const String& inValue);

            Variables extractVariables(const String& inValue);

            StyleRuleset::List extractRulesets(const String& inValue, const String& inSelector = "");
            String stripNestedRulesets(const String& inValue);

            bool parseKeyframesSelector(const String& inSelector, String& outName, float& outOffset) const;

        private:
            StyleImport::List  m_imports;
            Variables          m_variables;
            StyleRuleset::List m_rulesets;
            Keyframes          m_keyframes;
        };
    }
}