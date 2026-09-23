#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_CORE String
    {
    public:
        static constexpr inline std::size_t npos = std::string::npos;

    public:
        static inline const String& empty()
        {
            static const String result;

            return result;
        }

    public:
        template <typename... T>
        static inline String sprint(const String& inValue, T... inParams)
        {
            const int bufferSize = static_cast<int>(inValue.size()) * 50;

            char*     buffer = new char[bufferSize];

            int       size = std::snprintf(buffer, bufferSize, inValue.toChar(), inParams...);

            if (size < 0 || size > bufferSize)
            {
                delete[] buffer;

                throw std::runtime_error("Error while trying to sprint " + inValue.toStandard());
            }

            std::string result = std::string(buffer);

            delete[] buffer;

            return result;
        }

    public:
        inline String() = default;

        inline String(const char* value)
            : m_value(value ? value : "")
        {}

        inline String(char value)
            : m_value(1, value)
        {}

        inline String(size_t count, char ch)
            : m_value(count, ch)
        {}

        inline String(const std::string& value)
            : m_value(value)
        {}

        inline String(std::string&& value) noexcept
            : m_value(std::move(value))
        {}

        inline String(const std::filesystem::path& p)
            : m_value(p.string())
        {}

        template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
        inline String(InputIt first, InputIt last)
            : m_value(first, last)
        {}

        inline String(const String&)     = default;
        inline String(String&&) noexcept = default;

    public:
        inline String& operator=(const String&)     = default;
        inline String& operator=(String&&) noexcept = default;

        inline friend String operator+(String lhs, const String& rhs)
        {
            lhs += rhs;

            return lhs;
        }

        inline friend String operator+(String lhs, const char* rhs)
        {
            lhs += rhs;

            return lhs;
        }

        inline friend String operator+(const char* lhs, const String& rhs)
        {
            String result(lhs);
            result += rhs;

            return result;
        }

        inline String& operator+=(const String& rhs)
        {
            m_value += rhs.m_value;

            return *this;
        }

        inline String& operator+=(const char* rhs)
        {
            m_value += rhs;

            return *this;
        }

        inline String& operator+=(char rhs)
        {
            m_value += rhs;

            return *this;
        }

        inline friend std::ostream& operator<<(std::ostream& os, const String& str)
        {
            os << str.m_value;

            return os;
        }

        inline bool operator<(const String& rhs) const { return m_value < rhs.m_value; }

        inline friend bool operator==(const String& inA, const String& inB) { return inA.equals(inB); }

        inline operator std::string() const { return m_value; }

    public:
        CH_FUNCTION()
        bool isEmpty() const;

        CH_FUNCTION()
        bool isNaN() const;

        CH_FUNCTION()
        bool contains(const String& inValue) const;

        CH_FUNCTION()
        bool contains(char inValue) const;

        CH_FUNCTION()
        bool startsWith(const String& inValue) const;

        CH_FUNCTION()
        bool startsWith(char inValue) const;

        CH_FUNCTION()
        bool endsWith(const String& inValue) const;

        CH_FUNCTION()
        bool endsWith(char inValue) const;

        CH_FUNCTION()
        String toUpper() const;

        CH_FUNCTION()
        String toLower() const;

        CH_FUNCTION()
        String substr(std::size_t inStart, std::size_t inEnd = npos) const;

        CH_FUNCTION()
        std::size_t firstOf(char inValue, std::size_t inLocation = 0L) const;

        CH_FUNCTION()
        std::size_t firstOf(const String& inValue, std::size_t inLocation = 0L) const;

        CH_FUNCTION()
        std::size_t lastOf(char inValue, std::size_t inLocation = npos) const;

        CH_FUNCTION()
        std::size_t lastOf(const String& inValue, std::size_t inLocation = npos) const;

        CH_FUNCTION()
        String filter(char inValue) const;

        CH_FUNCTION()
        String filter(const String& inValue) const;

        CH_FUNCTION()
        std::size_t find(char inValue) const;

        CH_FUNCTION()
        std::size_t find(const String& inValue) const;

        CH_FUNCTION()
        String getBetween(char inOpening, char inClosing) const;

        CH_FUNCTION()
        String getBetween(const String& inOpening, const String& inClosing) const;

        CH_FUNCTION()
        String trim() const;

        CH_FUNCTION()
        std::size_t size() const;

    public:
        template <typename... Args>
        inline bool equals(Args... inDelimeters) const
        {
            std::vector<String> delimeters;
            (delimeters.emplace_back(std::forward<Args>(inDelimeters)), ...);

            if (size() == 0 || delimeters.empty())
            {
                return false;
            }

            for (const String& delimeter : delimeters)
            {
                if (delimeter.size() == 0)
                {
                    continue;
                }

                if (std::strcmp(toChar(), delimeter.toChar()) == 0)
                {
                    return true;
                }
            }

            return false;
        }

        template <typename... Args>
        inline bool startsWithChars(Args... inValues) const
        {
            static_assert((std::is_same_v<Args, char> && ...), "startsWithChars only accepts char arguments");

            std::vector<char> values;
            (values.emplace_back(std::forward<Args>(inValues)), ...);

            if (size() == 0 || values.empty())
            {
                return npos;
            }

            for (char value : values)
            {
                if (!startsWith(value))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        bool toBool() const;
        const std::string& toStandard() const;
        const char* toChar() const;

        int compare(const String& inValue) const;

        std::vector<char32_t> toUnicode() const;

        char at(std::size_t inIndex) const;

        template <typename... Args>
        std::size_t firstOfChars(Args... inValues)
        {
            static_assert((std::is_same_v<Args, char> && ...), "firstOfChars only accepts char arguments");

            std::vector<char> values;
            (values.emplace_back(std::forward<Args>(inValues)), ...);

            if (size() == 0 || values.empty())
            {
                return npos;
            }

            std::size_t result = npos;

            for (char value : values)
            {
                result = std::min(firstOf(value), result);
            }

            return result;
        }

        template <typename... Args>
        inline std::vector<String> split(Args... inDelimeters) const
        {
            std::vector<String> delimeters;
            (delimeters.emplace_back(std::forward<Args>(inDelimeters)), ...);

            if (size() == 0 || delimeters.empty())
            {
                return {};
            }

            std::vector<String> result = {};

            String              value = *this;

            size_t              position = 0;

            String              block = "";

            for (const String& delimeter : delimeters)
            {
                if (delimeter.size() == 0)
                {
                    continue;
                }

                position = 0;

                block = "";

                while ((position = value.find(delimeter)) != std::string::npos)
                {
                    block = value.substr(0, position);

                    result.push_back(block);

                    value.erase(0, position + delimeter.size());
                }
            }

            if (value.size() > 0)
            {
                result.push_back(value);
            }

            return result;
        }

        std::string::iterator begin();
        std::string::const_iterator begin() const;

        std::string::const_iterator cbegin();
        std::string::const_iterator cbegin() const;

        std::string::iterator end();
        std::string::const_iterator end() const;

        std::string::const_iterator cend();
        std::string::const_iterator cend() const;

        char& front();
        const char& front() const;

        char& back();
        const char& back() const;

        void append(const String& inValue);
        void append(char inValue);

        void erase(char inValue);
        void erase(std::string::const_iterator inStart, std::string::const_iterator inEnd);
        void erase(std::uint32_t inStart, std::uint32_t inEnd);

        template <typename... Args>
        String regexReplace(Args... args)
        {
            return std::regex_replace(m_value, args...);
        }

        void popFront();
        void popBack();

    protected:
        std::string m_value;
    };
}

namespace std
{
    template <>
    struct hash<Chicane::String>
    {
        size_t operator()(const Chicane::String& inValue) const { return hash<string>()(inValue.toStandard()); }
    };
}