#include "Chicane/Grid/Style/Import.hpp"

#include <unordered_map>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const std::unordered_map<String, StyleImportType> TYPE_KEYS = {
            {"Font",    StyleImportType::Font   },
            {"Style",   StyleImportType::Style  },
            {"Texture", StyleImportType::Texture},
        };

        StyleImport::StyleImport(const String& inValue)
            : StyleImport()
        {
            parse(inValue);
        }

        StyleImport::StyleImport()
            : type(StyleImportType::Undefined),
              location({})
        {}

        void StyleImport::parse(const String& inValue)
        {
            if (!inValue.startsWith(Style::IMPORT_KEYWORD))
            {
                return;
            }

            const std::vector<String> params =
                inValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING).split(FUNCTION_PARAMS_SEPARATOR);

            if (params.size() < 2)
            {
                return;
            }

            setType(params.at(0));
            setLocation(params.at(1));
        }

        void StyleImport::setType(const String& inValue)
        {
            const String value = inValue.trim();

            if (value.isEmpty())
            {
                type = StyleImportType::Undefined;

                return;
            }

            type = TYPE_KEYS.find(value) == TYPE_KEYS.end() ? StyleImportType::Undefined : TYPE_KEYS.at(value);
        }

        void StyleImport::setLocation(const String& inValue)
        {
            location.parse(inValue.trim());
        }
    }
}