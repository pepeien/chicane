#pragma once

#include <set>
#include <cstddef>

#include "Chicane/Core/Changeable.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        struct CHICANE_GRID ClassList : public Changeable
        {
            friend inline bool operator==(const ClassList& inLeft, const ClassList& inRight)
            {
                return inLeft.m_source == inRight.m_source;
            }

            friend inline bool operator!=(const ClassList& inLeft, const ClassList& inRight)
            {
                return !(inLeft == inRight);
            }

        public:
            using Source        = std::set<String>;
            using Iterator      = Source::iterator;
            using ConstIterator = Source::const_iterator;

        public:
            static constexpr inline const char SEPARATOR = ' ';
            static constexpr inline const char SELECTOR  = '.';

        private:
            static String sNormalize(const String& inValue);
            static void sInsertTokens(Source& outSource, const String& inValue);

        public:
            ClassList();
            explicit ClassList(const String& inValue);
            ClassList(const ClassList& inOther);
            ClassList(ClassList&& inOther) noexcept;

        public:
            ClassList& operator=(const ClassList& inOther);
            ClassList& operator=(ClassList&& inOther) noexcept;
            ClassList& operator=(const String& inValue);
            ClassList& operator+=(const String& inValue);

            operator String() const;

        public:
            CH_FUNCTION()
            void add(const String& inValue);

            template <typename T, typename... Args>
            inline void add(T inFirst, Args... inRest)
            {
                add(String(inFirst));
                (add(String(inRest)), ...);
            }

            CH_FUNCTION()
            void remove(const String& inValue);

            template <typename T, typename... Args>
            inline void remove(T inFirst, Args... inRest)
            {
                remove(String(inFirst));
                (remove(String(inRest)), ...);
            }

            CH_FUNCTION()
            bool toggle(const String& inValue);

            CH_FUNCTION()
            bool toggle(const String& inValue, bool inForce);

            CH_FUNCTION()
            bool contains(const String& inValue) const;

            CH_FUNCTION()
            bool replace(const String& inOld, const String& inNew);

            CH_FUNCTION()
            String item(std::size_t inIndex) const;

            CH_FUNCTION()
            std::size_t length() const;

            CH_FUNCTION()
            String value() const;

            CH_FUNCTION()
            void setValue(const String& inValue);

            CH_FUNCTION()
            String toString() const;

        public:
            std::size_t size() const;
            bool isEmpty() const;

            Iterator begin();
            Iterator end();
            ConstIterator begin() const;
            ConstIterator end() const;

        private:
            void commit(Source inSource);

        private:
            Source m_source;
        };
    }
}
