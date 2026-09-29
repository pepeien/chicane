#include "Chicane/Core/FileSystem.hpp"

#include <fstream>
#include <system_error>

#if IS_WINDOWS
    #include <windows.h>
#endif

#include "Chicane/Core/FileSystem/Listing/Service.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        Path executableDirectory()
        {
            static const Path directory = []() -> Path
            {
#if IS_WINDOWS
                wchar_t     buffer[MAX_PATH] = {};
                const DWORD length           = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
                if (length == 0 || length >= MAX_PATH)
                {
                    return Path(std::filesystem::current_path());
                }

                return Path(std::filesystem::path(buffer).parent_path());
#else
                std::error_code             error;
                const std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", error);
                if (error)
                {
                    return Path(std::filesystem::current_path());
                }

                return Path(exe.parent_path());
#endif
            }();

            return directory;
        }

        Path resolve(const Path& inPath, const Path& inBase)
        {
            if (inPath.isEmpty())
            {
                return inPath;
            }

            const auto existing = [](const Path& inCandidate) -> Path
            {
                if (inCandidate.isEmpty() || !std::filesystem::exists(inCandidate.toStandard()))
                {
                    return {};
                }

                std::error_code             error;
                const std::filesystem::path canonical =
                    std::filesystem::weakly_canonical(inCandidate.toStandard(), error);
                if (error)
                {
                    return inCandidate.lexicallyNormal();
                }

                return Path(canonical);
            };

            if (inPath.isAbsolute())
            {
                const Path found = existing(inPath);
                return found.isEmpty() ? inPath : found;
            }

            if (const Path found = existing(inPath); !found.isEmpty())
            {
                return found;
            }

            const String pathText      = inPath.toString();
            const bool   bRootRelative = pathText.startsWith("Assets/") || pathText.startsWith("Assets\\");

            const auto searchAncestors = [&](Path inStart) -> Path
            {
                Path probe = inStart;
                for (int depth = 0; depth < 8; ++depth)
                {
                    if (probe.isEmpty())
                    {
                        break;
                    }

                    if (const Path found = existing(probe / inPath); !found.isEmpty())
                    {
                        return found;
                    }

                    const Path parent = probe.parent();
                    if (parent.isEmpty() || parent == probe)
                    {
                        break;
                    }

                    probe = parent;
                }

                return {};
            };

            if (bRootRelative)
            {
                if (const Path found = searchAncestors(Path(std::filesystem::current_path())); !found.isEmpty())
                {
                    return found;
                }

                if (const Path found = searchAncestors(executableDirectory()); !found.isEmpty())
                {
                    return found;
                }

                return inPath.lexicallyNormal();
            }

            Path            base = inBase;
            std::error_code fileError;
            if (!base.isEmpty() &&
                (base.hasExtension() || std::filesystem::is_regular_file(base.toStandard(), fileError)))
            {
                base = base.parent();
            }

            if (!base.isEmpty())
            {
                if (const Path found = existing(base / inPath); !found.isEmpty())
                {
                    return found;
                }
            }

            if (const Path found = existing(executableDirectory() / inPath); !found.isEmpty())
            {
                return found;
            }

            if (!base.isEmpty())
            {
                return (base / inPath).lexicallyNormal();
            }

            return inPath.lexicallyNormal();
        }

        Path rootRelative(const Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return inPath;
            }

            std::error_code       error;
            std::filesystem::path absolute = std::filesystem::absolute(inPath.toStandard(), error);
            if (error)
            {
                absolute = inPath.toStandard();
            }

            const std::filesystem::path canonical = std::filesystem::weakly_canonical(absolute, error);
            if (!error)
            {
                absolute = canonical;
            }

            const std::filesystem::path relative =
                std::filesystem::relative(absolute, executableDirectory().toStandard(), error);
            if (!error && !relative.empty() && relative.begin() != relative.end() && *relative.begin() != "..")
            {
                return Path(String(relative.generic_string()));
            }

            std::filesystem::path fromAssets;
            bool                  found = false;
            for (const std::filesystem::path& part : absolute)
            {
                if (!found)
                {
                    if (part != "Assets")
                    {
                        continue;
                    }

                    found = true;
                }

                fromAssets /= part;
            }

            if (found)
            {
                return Path(String(fromAssets.generic_string()));
            }

            if (!error && !relative.empty())
            {
                return Path(String(relative.generic_string()));
            }

            return Path(String(absolute.generic_string()));
        }

        bool exists(const Path& inPath, const Path& inBase)
        {
            if (inPath.isEmpty())
            {
                return false;
            }

            const Path resolved = resolve(inPath, inBase);
            return !resolved.isEmpty() && std::filesystem::exists(resolved.toStandard());
        }

        std::vector<Item> ls(const Path& inDir, std::uint32_t inDepth)
        {
            if (inDir.isEmpty())
            {
                return {};
            }

            const std::uint32_t depth = inDepth == 0 ? 1 : inDepth;

            std::vector<Item> result;

            for (const auto& entry : std::filesystem::directory_iterator(inDir))
            {
                Item item(entry.is_directory() ? ItemType::Folder : ItemType::File, entry.path());
                if (item.type == ItemType::Folder && depth > 1)
                {
                    item.children = ls(item.path, depth - 1);
                }

                result.push_back(item);
            }

            return result;
        }

        void requestLs(const Path& inDir)
        {
            if (inDir.isEmpty())
            {
                return;
            }

            ListingService::sInstance().enqueue(inDir);
        }

        void pumpLs(std::vector<Listing>& outReady)
        {
            ListingService::sInstance().drain(outReady);
        }

        String readStringUnsigned(const Path& inFilepath)
        {
            const std::vector<unsigned char> raw = readUnsigned(inFilepath);

            if (raw.empty())
            {
                return "";
            }

            return String(raw.begin(), raw.end());
        }

        std::vector<unsigned char> readUnsigned(const Path& inFilepath)
        {
            std::ifstream file(resolve(inFilepath).toStandard(), std::ios::binary | std::ios::ate);

            if (!file)
            {
                throw std::runtime_error("Failed to open the file [" + inFilepath.toString() + "]");
            }

            std::streamsize size = file.tellg();
            file.seekg(0);

            std::vector<unsigned char> result(size);

            if (!file.read(reinterpret_cast<char*>(result.data()), size))
            {
                throw std::runtime_error("Failed to read the file [" + inFilepath.toString() + "]");
            }

            return result;
        }

        String readString(const Path& inFilepath)
        {
            const std::vector<char> raw = read(inFilepath);

            if (raw.empty())
            {
                return "";
            }

            return String(raw.begin(), raw.end());
        }

        std::vector<char> read(const Path& inFilepath)
        {
            const Path path = resolve(inFilepath);

            std::basic_ifstream<char> file(path.toString(), std::ios::ate | std::ios::binary);

            if (!file)
            {
                throw std::runtime_error("Failed to open the file [" + inFilepath.toString() + "]");
            }

            size_t            fileSize = (size_t)file.tellg();
            std::vector<char> result(fileSize);

            file.seekg(0);
            file.read(result.data(), fileSize);
            file.close();

            return result;
        }

        static void writeBytes(const char* inData, std::size_t inSize, const Path& inFilepath)
        {
            if (inData == nullptr || inSize == 0)
            {
                return;
            }

            const Path target = inFilepath.isAbsolute() ? inFilepath : resolve(inFilepath);
            if (target.isEmpty())
            {
                throw std::runtime_error("Failed to write the file [" + inFilepath.toString() + "]");
            }

            const Path tempPath(target.toString() + ".tmp");

            {
                std::ofstream file(tempPath.toStandard(), std::ios::binary | std::ios::trunc);
                if (!file)
                {
                    throw std::runtime_error("Failed to write the file [" + target.toString() + "]");
                }

                file.write(inData, static_cast<std::streamsize>(inSize));
                file.flush();
                if (!file)
                {
                    file.close();
                    std::error_code removeError;
                    std::filesystem::remove(tempPath.toStandard(), removeError);

                    throw std::runtime_error("Failed to write the file [" + target.toString() + "]");
                }
            }

            std::error_code error;
            std::filesystem::remove(target.toStandard(), error);
            error.clear();
            std::filesystem::rename(tempPath.toStandard(), target.toStandard(), error);
            if (error)
            {
                std::filesystem::copy_file(
                    tempPath.toStandard(),
                    target.toStandard(),
                    std::filesystem::copy_options::overwrite_existing,
                    error
                );
                std::error_code removeError;
                std::filesystem::remove(tempPath.toStandard(), removeError);
                if (error)
                {
                    throw std::runtime_error("Failed to write the file [" + target.toString() + "]");
                }
            }
        }

        void write(const String& inData, const Path& inFilepath)
        {
            if (inData.isEmpty())
            {
                return;
            }

            const std::string& bytes = inData.toStandard();
            writeBytes(bytes.data(), bytes.size(), inFilepath);
        }

        void write(const std::vector<unsigned char>& inData, const Path& inFilepath)
        {
            if (inData.empty())
            {
                return;
            }

            writeBytes(reinterpret_cast<const char*>(inData.data()), inData.size(), inFilepath);
        }

        void write(const std::vector<char>& inData, const Path& inFilepath)
        {
            if (inData.empty())
            {
                return;
            }

            writeBytes(inData.data(), inData.size(), inFilepath);
        }
    }
}