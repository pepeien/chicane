#include "Chicane/Core/String.hpp"

namespace Chicane
{
    bool String::isEmpty() const
    {
        const std::string& result = trim().toStandard();

        return result.empty();
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
        return strcmp(toChar(), inValue.toChar()) == 0;
    }

    bool String::equals(char inValue) const
    {
        return strcmp(toChar(), std::string(1, inValue).c_str()) == 0;
    }
    
    bool String::contains(const String& inValue) const
    {
        return contains(inValue.toChar());
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

        return strcmp(m_value.substr(0, inValue.size()).c_str(), inValue.toChar()) == 0;
    }

    bool String::startsWith(char inValue) const
    {
        if (isEmpty())
        {
            return false;
        }

        return *m_value.substr(0, 1).c_str() == inValue;
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

        return strcmp(m_value.substr(size() - inValue.size()).c_str(), inValue.toChar()) == 0;
    }

    bool String::endsWith(char inValue) const
    {
        if (isEmpty())
        {
            return false;
        }

        return *m_value.substr(size() - 1).c_str() == inValue;
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

    String String::toUpper() const
    {
        std::string result = m_value;
        std::transform(
            result.begin(),
            result.end(),
            result.begin(),
            ::toupper
        );

        return result;

    }

    String String::toLower() const
    {
        std::string result = m_value;
        std::transform(
            result.begin(),
            result.end(),
            result.begin(),
            ::tolower
        );

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

    std::vector<String> String::split(char inDelimeter) const
    {
        return split(String(1, inDelimeter));
    }

    std::vector<String> String::split(const String& inDelimeter) const
    {
        if (isEmpty())
        {
            return {};
        }

        int start = 0;

        std::vector<String> result = {};

        for (int i = 0; i < size(); i += inDelimeter.size())
        {
            if (!substr(i, inDelimeter.size()).equals(inDelimeter.toChar()))
            {
                continue;
            }

            String block = m_value.substr(start, i - start);

            if (block.startsWith(inDelimeter))
            {
                block.erase(0, 1);
            }

            if (block.endsWith(inDelimeter))
            {
                block.popBack();
            }

            block = block.trim();

            start = i;

            if (block.isEmpty())
            {
                continue;
            }

            result.push_back(block);
        }

        const String block = m_value.substr(start);

        if (!block.isEmpty())
        {
            result.push_back(block);
        }

        return result;
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

    void String::erase(std::string::const_iterator inStart, std::string::const_iterator inEnd)
    {
        m_value.erase(inStart, inEnd);
    }

    void String::erase(std::uint32_t inStart, std::uint32_t inEnd)
    {
        m_value.erase(inStart, inEnd);
    }

    void String::popBack()
    {
        if (isEmpty())
        {
            return;
        }

        m_value.pop_back();
    }
}