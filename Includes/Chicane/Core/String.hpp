#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    struct CHICANE_CORE String
    {
    public:
        static constexpr const std::size_t npos = std::string::npos;

    public:
        template<typename... T>
        static inline String sprint(const String& inValue, T... inParams)
        {
            const int bufferSize = static_cast<int>(inValue.size()) * 50;

            char* buffer = new char[bufferSize];

            int size = std::snprintf(
                buffer,
                bufferSize,
                inValue.toChar(),
                inParams...
            );

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
        template<typename... T>
        String(T... inParams)
            : m_value(inParams...)
        {}

        String(const String& inValue)
            : m_value(inValue.m_value)
        {}

        String(String&& inValue) noexcept
            : m_value(std::move(inValue.m_value))
        {}

        String()
            : m_value("")
        {}

    public:
        inline operator const char*() const
        {
            return m_value.c_str();
        }

        inline friend bool operator==(const String& inA, const String& inB)
        {
            return inA.equals(inB);
        }

        String& operator=(const String& other)
        {
            m_value = other.m_value;
            return *this;
        }

        String& operator=(String&& other) noexcept
        {
            m_value = std::move(other.m_value);
            return *this;
        }

        inline String& operator=(const std::string& rhs)
        {
            m_value = std::move(rhs);
            return *this;
        }

        inline String& operator=(const char* rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline String& operator+=(const String& rhs)
        {
            append(rhs);
            return *this;
        }

        inline String& operator+=(const std::string& rhs)
        {
            append(rhs);
            return *this;
        }

        inline String& operator+=(const char* rhs)
        {
            append(rhs);
            return *this;
        }

        inline friend String operator+(String lhs, const String& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        inline friend String operator+(String lhs, const std::string& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        inline friend String operator+(String lhs, const char* rhs)
        {
            lhs += rhs;
            return lhs;
        }

        inline bool operator<(const String& rhs) const
        {
            return m_value < rhs.m_value;
        }

    public:
        bool isEmpty() const;
        bool isNaN() const;

        bool equals(const String& inValue) const;
        bool equals(char inValue) const;

        bool contains(const String& inValue) const;
        bool contains(char inValue) const;

        bool startsWith(const String& inValue) const;
        bool startsWith(char inValue) const;

        bool endsWith(const String& inValue) const;
        bool endsWith(char inValue) const;

        bool toBool() const;
        const std::string& toStandard() const;
        const char* toChar() const;

        String toUpper() const;
        String toLower() const;

        char at(std::size_t inIndex) const;

        String substr(std::size_t inStart, std::size_t inEnd = npos) const;

        std::size_t firstOf(char inValue) const;
        std::size_t firstOf(const String& inValue) const;

        std::size_t lastOf(char inValue) const;
        std::size_t lastOf(const String& inValue) const;

        std::vector<String> split(char inDelimeter) const;
        std::vector<String> split(const String& inDelimeter) const;

        String trim() const;

        std::size_t size() const;

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

        void erase(std::string::const_iterator inStart, std::string::const_iterator inEnd);
        void erase(std::uint32_t inStart, std::uint32_t inEnd);

        void popBack();

    protected:
        std::string m_value;
    };
}

namespace std
{
    template<>
    struct hash<Chicane::String>
    {
        size_t operator()(const Chicane::String& s) const
        {
            return hash<string>()(s.toStandard());
        }
    };
}