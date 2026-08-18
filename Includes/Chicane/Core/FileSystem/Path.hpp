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
            inline Path() = default;

            inline Path(const std::filesystem::path& inPath)
                : m_path(inPath)
            {}

            inline Path(const String& inPath)
                : m_path(inPath.toStandard())
            {}

            inline Path(const char* inPath)
                : m_path(inPath)
            {}

        public:
            // Conversion
            inline operator std::filesystem::path() const { return m_path; }
            inline operator String() const { return String(m_path.string()); }
            inline operator std::string() const { return m_path.string(); }

            // Comparison
            inline bool operator==(const Path& inOther) const { return m_path == inOther.m_path; }
            inline bool operator!=(const Path& inOther) const { return m_path != inOther.m_path; }

            // Concatenation
            inline Path operator/(const Path& inOther) const { return Path(m_path / inOther.m_path); }
            inline Path& operator/=(const Path& inOther)
            {
                m_path /= inOther.m_path;
                return *this;
            }

        public:
            // Iterations
            inline std::filesystem::path::iterator begin() const { return m_path.begin(); }
            inline std::filesystem::path::iterator end() const { return m_path.end(); }

            // Components
            inline Path filename() const { return Path(m_path.filename()); }
            inline Path stem() const { return Path(m_path.stem()); }
            inline Path extension() const { return Path(m_path.extension()); }
            inline Path parent() const { return Path(m_path.parent_path()); }
            inline Path absolute() const { return Path(std::filesystem::absolute(m_path)); }
            inline Path canonical() const { return Path(std::filesystem::canonical(m_path)); }
            inline Path lexicallyNormal() const { return Path(m_path.lexically_normal()); }

            // Queries
            inline bool isEmpty() const { return m_path.empty(); }
            inline bool exists() const { return std::filesystem::exists(m_path); }
            inline bool isFile() const { return std::filesystem::is_regular_file(m_path); }
            inline bool isDirectory() const { return std::filesystem::is_directory(m_path); }
            inline bool isAbsolute() const { return m_path.is_absolute(); }
            inline bool isRelative() const { return m_path.is_relative(); }
            inline bool hasExtension() const { return m_path.has_extension(); }
            inline bool hasFilename() const { return m_path.has_filename(); }

            // Conversion
            inline String toString() const { return String(m_path.string()); }
            inline const std::filesystem::path::value_type* toChar() const { return m_path.c_str(); }
            inline const std::filesystem::path& toStandard() const { return m_path; }

            // Mutation
            inline Path& replaceExtension(const String& inExt)
            {
                m_path.replace_extension(inExt.toStandard());

                return *this;
            }

            inline Path withExtension(const String& inExt) const
            {
                Path copy = *this;
                copy.m_path.replace_extension(inExt.toStandard());

                return copy;
            }

            inline Path relativeTo(const Path& inBase) const
            {
                return Path(std::filesystem::relative(m_path, inBase.m_path));
            }

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
        inline size_t operator()(const Chicane::FileSystem::Path& inValue) const
        {
            return hash<filesystem::path>()(inValue.toStandard());
        }
    };
}