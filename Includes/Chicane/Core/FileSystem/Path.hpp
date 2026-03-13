#pragma once

#include <filesystem>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        CH_TYPE(Manual)
        struct CHICANE_CORE Path
        {
        public:
            Path() = default;

            Path(const std::filesystem::path& inPath)
                : m_path(inPath)
            {}

            Path(const String& inPath)
                : m_path(inPath.toStandard())
            {}

            Path(const char* inPath)
                : m_path(inPath)
            {}

        public:
            // Conversion
            operator std::filesystem::path() const { return m_path; }
            operator String() const { return String(m_path.string()); }
            operator std::string() const { return m_path.string(); }

            // Comparison
            bool operator==(const Path& inOther) const { return m_path == inOther.m_path; }
            bool operator!=(const Path& inOther) const { return m_path != inOther.m_path; }

            // Concatenation
            Path operator/(const Path& inOther) const { return Path(m_path / inOther.m_path); }
            Path& operator/=(const Path& inOther)
            {
                m_path /= inOther.m_path;
                return *this;
            }

        public:
            // Components
            Path filename() const { return Path(m_path.filename()); }
            Path stem() const { return Path(m_path.stem()); }
            Path extension() const { return Path(m_path.extension()); }
            Path parent() const { return Path(m_path.parent_path()); }
            Path absolute() const { return Path(std::filesystem::absolute(m_path)); }
            Path canonical() const { return Path(std::filesystem::canonical(m_path)); }
            Path lexicallyNormal() const { return Path(m_path.lexically_normal()); }

            // Queries
            bool isEmpty() const { return m_path.empty(); }
            bool exists() const { return std::filesystem::exists(m_path); }
            bool isFile() const { return std::filesystem::is_regular_file(m_path); }
            bool isDirectory() const { return std::filesystem::is_directory(m_path); }
            bool isAbsolute() const { return m_path.is_absolute(); }
            bool isRelative() const { return m_path.is_relative(); }
            bool hasExtension() const { return m_path.has_extension(); }
            bool hasFilename() const { return m_path.has_filename(); }

            // Conversion
            String toString() const { return String(m_path.string()); }
            const std::filesystem::path::value_type* toChar() const { return m_path.c_str(); }
            const std::filesystem::path& toStandard() const { return m_path; }

            // Mutation
            Path& replaceExtension(const String& inExt)
            {
                m_path.replace_extension(inExt.toStandard());

                return *this;
            }

            Path withExtension(const String& inExt) const
            {
                Path copy = *this;
                copy.m_path.replace_extension(inExt.toStandard());

                return copy;
            }

            Path relativeTo(const Path& inBase) const { return Path(std::filesystem::relative(m_path, inBase.m_path)); }

        private:
            std::filesystem::path m_path;
        };
    }
}

namespace std
{
    template <>
    struct hash<Chicane::FileSystem::Path>
    {
        size_t operator()(const Chicane::FileSystem::Path& inValue) const
        {
            return hash<filesystem::path>()(inValue.toStandard());
        }
    };
}