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
            StyleProperty(A... args)
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
            bool isRaw(const String& inValue) const { return m_raw.equals(inValue); }
            const String& getRaw() const { return m_raw; }
            void setRaw(const String& inValue)
            {
                if (inValue.isEmpty())
                {
                    return;
                }

                m_raw = inValue;
            }

            T get() const { return m_parsed; }
            void set(const StyleProperty<T>& inProperty) { m_parsed = inProperty.get(); }
            void set(T inValue) { m_parsed = inValue; }

            void refresh()
            {
                if (m_raw.isEmpty() || !m_parser)
                {
                    return;
                }

                set(m_parser(m_raw));
            }

            void parseWith(std::function<T(const String&)> inParser) { m_parser = inParser; }

        private:
            String m_raw;
            T      m_parsed;
            Parser m_parser;
        };
    }
}