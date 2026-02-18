#pragma once

#include <functional>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        template <typename T>
        struct CHICANE_GRID StyleProperty
        {
        public:
            using Parser = std::function<T(const String&)>;

        public:
            template <typename... A>
            inline StyleProperty(A... args)
                : m_raw(""),
                  m_parsed(args...)
            {}

        public:
            inline friend bool operator==(const StyleProperty& inA, const StyleProperty& inB)
            {
                return inA.get() == inB.get();
            }

            inline friend bool operator==(const StyleProperty& inA, const T& inB) { return inA.get() == inB; }

        public:
            inline bool isRaw(const String& inValue) const { return m_raw.equals(inValue); }
            inline const String& getRaw() const { return m_raw; }
            inline void setRaw(const String& inValue) { m_raw = inValue; }

            inline T get() const { return m_parsed; }
            inline void set(const StyleProperty<T>& inProperty) { m_parsed = inProperty.get(); }
            inline void set(T inValue) { m_parsed = inValue; }

            inline void refresh()
            {
                if (m_raw.isEmpty() || !m_parser)
                {
                    return;
                }

                set(m_parser(m_raw));
            }

            inline void parseWith(std::function<T(const String&)> inParser) { m_parser = inParser; }

        private:
            String m_raw;
            T      m_parsed;
            Parser m_parser;
        };
    }
}