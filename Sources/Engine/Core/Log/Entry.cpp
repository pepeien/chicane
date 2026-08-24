#include "Chicane/Core/Log/Entry.reflected.hpp"

namespace Chicane
{
    namespace Log
    {
        Entry::Entry(const String& inText, const String& inColor)
            : text(inText),
              color(inColor)
        {}

        Entry::Entry()
            : text(""),
              color("")
        {}

        bool Entry::isEmpty() const
        {
            return text.isEmpty();
        }
    }
}