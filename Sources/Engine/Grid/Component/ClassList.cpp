#include "Chicane/Grid/Component/ClassList.reflected.hpp"

#include <iterator>
#include <utility>

namespace Chicane
{
    namespace Grid
    {
        String ClassList::sNormalize(const String& inValue)
        {
            String token = inValue.trim();
            if (token.startsWith(SELECTOR))
            {
                token = token.substr(1);
            }

            return token;
        }

        void ClassList::sInsertTokens(Source& outSource, const String& inValue)
        {
            String token;
            bool   bInReference = false;

            const auto flush = [&]()
            {
                const String normalized = sNormalize(token);
                if (!normalized.isEmpty())
                {
                    outSource.insert(normalized);
                }

                token = String::sEmpty();
            };

            for (std::size_t i = 0; i < inValue.size(); i++)
            {
                const char c = inValue.at(i);

                if (!bInReference && c == '{' && i + 1 < inValue.size() && inValue.at(i + 1) == '{')
                {
                    bInReference = true;
                    token.append(c);
                    continue;
                }

                if (bInReference && c == '}' && i + 1 < inValue.size() && inValue.at(i + 1) == '}')
                {
                    token.append(c);
                    token.append(inValue.at(++i));
                    bInReference = false;
                    continue;
                }

                if (!bInReference && c == SEPARATOR)
                {
                    flush();
                    continue;
                }

                token.append(c);
            }

            flush();
        }

        ClassList::ClassList()
            : Changeable(),
              m_source({})
        {}

        ClassList::ClassList(const String& inValue)
            : ClassList()
        {
            sInsertTokens(m_source, inValue);
        }

        ClassList::ClassList(const ClassList& inOther)
            : Changeable(),
              m_source(inOther.m_source)
        {}

        ClassList::ClassList(ClassList&& inOther) noexcept
            : Changeable(),
              m_source(std::move(inOther.m_source))
        {}

        ClassList& ClassList::operator=(const ClassList& inOther)
        {
            commit(inOther.m_source);

            return *this;
        }

        ClassList& ClassList::operator=(ClassList&& inOther) noexcept
        {
            commit(std::move(inOther.m_source));

            return *this;
        }

        ClassList& ClassList::operator=(const String& inValue)
        {
            setValue(inValue);

            return *this;
        }

        ClassList& ClassList::operator+=(const String& inValue)
        {
            add(inValue);

            return *this;
        }

        ClassList::operator String() const
        {
            return toString();
        }

        void ClassList::add(const String& inValue)
        {
            Source next = m_source;
            sInsertTokens(next, inValue);
            commit(std::move(next));
        }

        void ClassList::remove(const String& inValue)
        {
            Source next = m_source;
            for (const String& part : inValue.split(SEPARATOR))
            {
                const String token = sNormalize(part);
                if (!token.isEmpty())
                {
                    next.erase(token);
                }
            }

            commit(std::move(next));
        }

        bool ClassList::toggle(const String& inValue)
        {
            if (contains(inValue))
            {
                remove(inValue);

                return false;
            }

            add(inValue);

            return true;
        }

        bool ClassList::toggle(const String& inValue, bool inForce)
        {
            if (inForce)
            {
                add(inValue);

                return contains(inValue);
            }

            remove(inValue);

            return false;
        }

        bool ClassList::contains(const String& inValue) const
        {
            const String token = sNormalize(inValue);
            if (token.isEmpty())
            {
                return false;
            }

            return m_source.find(token) != m_source.end();
        }

        bool ClassList::replace(const String& inOld, const String& inNew)
        {
            const String oldToken = sNormalize(inOld);
            if (oldToken.isEmpty() || m_source.find(oldToken) == m_source.end())
            {
                return false;
            }

            Source next = m_source;
            next.erase(oldToken);

            const String newToken = sNormalize(inNew);
            if (!newToken.isEmpty())
            {
                next.insert(newToken);
            }

            commit(std::move(next));

            return true;
        }

        String ClassList::item(std::size_t inIndex) const
        {
            if (inIndex >= m_source.size())
            {
                return String::sEmpty();
            }

            auto found = m_source.begin();
            std::advance(found, static_cast<std::ptrdiff_t>(inIndex));

            return *found;
        }

        std::size_t ClassList::length() const
        {
            return m_source.size();
        }

        String ClassList::value() const
        {
            String result;
            for (const String& token : m_source)
            {
                if (!result.isEmpty())
                {
                    result.append(SEPARATOR);
                }

                result.append(token);
            }

            return result;
        }

        void ClassList::setValue(const String& inValue)
        {
            Source next;
            sInsertTokens(next, inValue);
            commit(std::move(next));
        }

        String ClassList::toString() const
        {
            return value();
        }

        std::size_t ClassList::size() const
        {
            return m_source.size();
        }

        bool ClassList::isEmpty() const
        {
            return m_source.empty();
        }

        ClassList::Iterator ClassList::begin()
        {
            return m_source.begin();
        }

        ClassList::Iterator ClassList::end()
        {
            return m_source.end();
        }

        ClassList::ConstIterator ClassList::begin() const
        {
            return m_source.begin();
        }

        ClassList::ConstIterator ClassList::end() const
        {
            return m_source.end();
        }

        void ClassList::commit(Source inSource)
        {
            if (m_source == inSource)
            {
                return;
            }

            m_source = std::move(inSource);
            emmitChanges();
        }
    }
}
