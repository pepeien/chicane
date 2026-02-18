#include "Chicane/Core/String.hpp"

#include <algorithm>
#include <cstring>

namespace Chicane
{
    String formatSplittedBlock(const String& inValue, const String inDelimeter)
    {
        if (inValue.isEmpty())
        {
            return "";
        }

        String block = inValue;

        if (block.startsWith(inDelimeter))
        {
            block.erase(0, 1);
        }

        if (block.endsWith(inDelimeter))
        {
            block.popBack();
        }

        return block.trim();
    }

    bool String::isEmpty() const
    {
        return trim().toStandard().empty();
    }

    bool String::isNaN() const
    {
        if (isEmpty())
        {
            return false;
        }

        return std::find_if(
                   m_value.begin(),
                   m_value.end(),
                   [](unsigned char c) { return !std::isdigit(c) && c != '.' && c != ','; }
               ) != m_value.end();
    }

    bool String::equals(const String& inValue) const
    {
        return std::strcmp(toChar(), inValue.toChar()) == 0;
    }

    bool String::equals(char inValue) const
    {
        return std::strcmp(toChar(), std::string(1, inValue).c_str()) == 0;
    }

    bool String::contains(const String& inValue) const
    {
        return m_value.find(inValue.toChar()) != std::string::npos;
    }

    bool String::contains(char inValue) const
    {
        return m_value.find(inValue) != std::string::npos;
    }

    bool String::startsWith(const String& inValue) const
    {
        if (isEmpty() || inValue.isEmpty())
        {
            return false;
        }

        if (inValue.size() > size())
        {
            return false;
        }

        return std::strcmp(m_value.substr(0, inValue.size()).c_str(), inValue.toChar()) == 0;
    }

    bool String::startsWith(char inValue) const
    {
        if (isEmpty())
        {
            return false;
        }

        return m_value.at(0) == inValue;
    }

    bool String::endsWith(const String& inValue) const
    {
        if (isEmpty() || inValue.isEmpty())
        {
            return false;
        }

        if (inValue.size() > size())
        {
            return false;
        }

        return std::strcmp(m_value.substr(size() - inValue.size()).c_str(), inValue.toChar()) == 0;
    }

    bool String::endsWith(char inValue) const
    {
        if (isEmpty())
        {
            return false;
        }

        return m_value.at(size() - 1) == inValue;
    }

    bool String::toBool() const
    {
        return equals("true") || equals("1");
    }

    const std::string& String::toStandard() const
    {
        return m_value;
    }

    const char* String::toChar() const
    {
        return m_value.c_str();
    }

    int String::compare(const String& inValue) const
    {
        return std::strcmp(toChar(), inValue.toChar());
    }

    String String::toUpper() const
    {
        std::string result = m_value;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);

        return result;
    }

    String String::toLower() const
    {
        std::string result = m_value;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);

        return result;
    }

    std::vector<char32_t> String::toUnicode() const
    {
        std::vector<char32_t> result;

        for (size_t i = 0; i < size();)
        {
            if (at(i) == '&' && i + 3 < size() && at(i + 1) == '#')
            {
                size_t j   = i + 2;
                bool   hex = false;

                if (j < size() && (at(j) == 'x' || at(j) == 'X'))
                {
                    hex = true;
                    j++;
                }

                size_t start = j;

                while (j < size() && at(j) != ';')
                {
                    j++;
                }

                if (j < size() && at(j) == ';')
                {
                    try
                    {
                        std::string num       = toStandard().substr(start, j - start);
                        char32_t    codepoint = (char32_t)std::stoul(num, nullptr, hex ? 16 : 10);

                        result.push_back(codepoint);

                        i = j + 1;

                        continue;
                    }
                    catch (...)
                    {
                        // fallthrough to UTF-8 decode
                    }
                }
            }

            unsigned char raw   = (unsigned char)at(i);
            char32_t      code  = 0;
            size_t        extra = 0;

            if (raw <= 0x7F)
            {
                code  = raw;
                extra = 0;
            }
            else if ((raw & 0xE0) == 0xC0)
            {
                code  = raw & 0x1F;
                extra = 1;
            }
            else if ((raw & 0xF0) == 0xE0)
            {
                code  = raw & 0x0F;
                extra = 2;
            }
            else if ((raw & 0xF8) == 0xF0)
            {
                code  = raw & 0x07;
                extra = 3;
            }
            else
            {
                i++;

                continue;
            }

            if (i + extra >= size())
            {
                break;
            }

            for (size_t k = 1; k <= extra; k++)
            {
                code <<= 6;
                code |= (at(i + k) & 0x3F);
            }

            result.push_back(code);

            i += extra + 1;
        }

        return result;
    }

    char String::at(std::size_t inIndex) const
    {
        return m_value.at(inIndex);
    }

    String String::substr(std::size_t inStart, std::size_t inEnd) const
    {
        return m_value.substr(inStart, inEnd);
    }

    std::size_t String::firstOf(char inValue) const
    {
        return m_value.find_first_of(inValue);
    }

    std::size_t String::firstOf(const String& inValue) const
    {
        return m_value.find_first_of(inValue.toStandard());
    }

    std::size_t String::lastOf(char inValue) const
    {
        return m_value.find_last_of(inValue);
    }

    std::size_t String::lastOf(const String& inValue) const
    {
        return m_value.find_last_of(inValue.toStandard());
    }

    String String::filter(char inValue) const
    {
        return filter(String(1, inValue));
    }

    String String::filter(const String& inValue) const
    {
        if (size() == 0 || inValue.size() == 0)
        {
            return *this;
        }

        String result = "";

        for (const String& block : split(inValue))
        {
            result.append(block);
        }

        return result;
    }

    std::size_t String::find(char inValue) const
    {
        return find(String(1, inValue));
    }

    std::size_t String::find(const String& inValue) const
    {
        return m_value.find(inValue.toStandard());
    }

    String String::getBetween(char inOpening, char inClosing) const
    {
        const std::size_t start = firstOf(inOpening) + 1;
        const std::size_t end   = lastOf(inClosing);

        return substr(start, end - start).trim();
    }

    String String::getBetween(const String& inOpening, const String& inClosing) const
    {
        const std::size_t start = firstOf(inOpening) + 1;
        const std::size_t end   = lastOf(inClosing);

        return substr(start, end - start);
    }

    String String::trim() const
    {
        std::string result = m_value;
        result.erase(0, result.find_first_not_of(" \n\r\t"));
        result.erase(result.find_last_not_of(" \n\r\t") + 1);

        return result;
    }

    std::size_t String::size() const
    {
        return m_value.size();
    }

    std::string::iterator String::begin()
    {
        return m_value.begin();
    }

    std::string::const_iterator String::begin() const
    {
        return m_value.begin();
    }

    std::string::const_iterator String::cbegin()
    {
        return m_value.cbegin();
    }

    std::string::const_iterator String::cbegin() const
    {
        return m_value.cbegin();
    }

    std::string::iterator String::end()
    {
        return m_value.end();
    }

    std::string::const_iterator String::end() const
    {
        return m_value.end();
    }

    std::string::const_iterator String::cend()
    {
        return m_value.end();
    }

    std::string::const_iterator String::cend() const
    {
        return m_value.cend();
    }

    char& String::front()
    {
        return m_value.front();
    }

    const char& String::front() const
    {
        return m_value.front();
    }

    char& String::back()
    {
        return m_value.back();
    }

    const char& String::back() const
    {
        return m_value.back();
    }

    void String::append(const String& inValue)
    {
        m_value.append(inValue.toChar());
    }

    void String::append(char inValue)
    {
        m_value.push_back(inValue);
    }

    void String::erase(char inValue)
    {
        m_value.erase(std::remove(m_value.begin(), m_value.end(), inValue), m_value.cend());
    }

    void String::erase(std::string::const_iterator inStart, std::string::const_iterator inEnd)
    {
        m_value.erase(inStart, inEnd);
    }

    void String::erase(std::uint32_t inStart, std::uint32_t inEnd)
    {
        m_value.erase(inStart, inEnd);
    }

    void String::popFront()
    {
        if (size() == 0)
        {
            return;
        }

        erase(0, 1);
    }

    void String::popBack()
    {
        if (size() == 0)
        {
            return;
        }

        m_value.pop_back();
    }
}