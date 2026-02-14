#pragma once

#include <vector>
#include <unordered_map>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Import.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFile
        {
        public:
            using Variables = std::unordered_map<String, String>;

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

        private:
            StyleImport::List extractImports(const String& inValue);

            Variables extractVariables(const String& inValue);

            StyleRuleset::List extractRulesets(const String& inValue, const String& inSelector = "");
            String stripNestedRulesets(const String& inValue);

        private:
            StyleImport::List  m_imports;
            Variables          m_variables;
            StyleRuleset::List m_rulesets;
        };
    }
}