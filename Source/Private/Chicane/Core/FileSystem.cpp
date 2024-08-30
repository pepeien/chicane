#include "Chicane/Core/FileSystem.hpp"


#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Chicane/Core/Helper.hpp"
#include "Chicane/Core/Log.hpp"

#ifdef OS_WINDOWS
    #include <windows.h>
    #include <tchar.h>
    #include <commdlg.h>
    #include <ShlObj.h>
#endif

namespace Chicane
{
    namespace FileSystem
    {
        DirectoryResult openDirectoryDialog()
        {
            #ifdef OS_WINDOWS
                char filepath[ MAX_PATH ];
                ZeroMemory(&filepath, sizeof( filepath ));

                BROWSEINFO browseInfo = { 0 };
                browseInfo.hwndOwner      = NULL;
                browseInfo.pidlRoot       = NULL;
                browseInfo.pszDisplayName = filepath;
                browseInfo.lpszTitle      = _T("Select a folder");
                browseInfo.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

                if (LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo))
                {
                    if (SHGetPathFromIDList(pidl, filepath))
                    {
                        std::string path = filepath;
                        path += "\\";

                        return {
                            path
                        };
                    }

                    CoTaskMemFree(pidl);
                }

                return {};
            #elif OS_LINUX
                return {};
            #endif
        }

        FileResult openFileDialog(
            const std::string& inTitle,
            const std::vector<FileFormat>& inFileFormats
        )
        {
            #ifdef OS_WINDOWS
                std::string fileFormatExtensions = "";

                for (int i = 0; i < inFileFormats.size(); i++)
                {
                    FileFormat fileFormat = inFileFormats[i];

                    if (i + 1 >= inFileFormats.size())
                    {
                        fileFormatExtensions.append("*." + fileFormat.extension);

                        break;
                    }

                    fileFormatExtensions.append("*." + fileFormat.extension + ";");
                }

                std::string fileFilter = "All Files (" + fileFormatExtensions + ")";
                fileFilter.push_back('\0');
                fileFilter += fileFormatExtensions;
                fileFilter.push_back('\0');

                for (int i = 0; i < inFileFormats.size(); i++)
                {
                    FileFormat fileFormat = inFileFormats[i];

                    fileFilter += fileFormat.title + " (*." + fileFormat.extension + ")";
                    fileFilter.push_back('\0');
                    fileFilter += "*." + fileFormat.extension + ";";
                    fileFilter.push_back('\0');
                }
                fileFilter.push_back('\0');

                char filepath[ MAX_PATH ];

                OPENFILENAME ofn;
                ZeroMemory(&filepath, sizeof( filepath ));
                ZeroMemory(&ofn,      sizeof( ofn ));
                ofn.lStructSize  = sizeof( ofn );
                ofn.hwndOwner    = NULL;
                ofn.lpstrFilter  = _T(fileFilter.c_str());
                ofn.lpstrFile    = filepath;
                ofn.nMaxFile     = MAX_PATH;
                ofn.lpstrTitle   = _T(inTitle.c_str());
                ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

                if (GetOpenFileName( &ofn ))
                {
                    std::vector<std::string> splittedFilepath = Helper::splitString(
                        filepath,
                        "."
                    );

                    return {
                        filepath,
                        splittedFilepath[splittedFilepath.size() - 1]
                    };
                }

                return {};
            #elif OS_LINUX
                return {};
            #endif
        }

        bool exists(const std::string& inPath)
        {
            if (inPath.empty())
            {
                return false;
            }

            return std::filesystem::exists(inPath);
        }

        std::vector<ListItem> ls(const std::string& inDir, std::uint32_t inDepth)
        {
            std::vector<ListItem> result {};

            if (inDepth > 1)
            {
                return result;
            }

            for (const auto& entry : std::filesystem::directory_iterator(inDir))
            {
                const auto& path = entry.path();

                ListItem item {};
                item.type      = entry.is_directory() ? ListType::Folder : ListType::File;
                item.name      = path.filename().string();
                item.extension = path.extension().string();
                item.path      = path.string();

                if (item.type == ListType::Folder)
                {
                    item.childCount = ls(item.path, 1).size();
                }

                result.push_back(item);
            }

            return result;
        }

        std::vector<char> readFile(const std::string& inFilepath)
        {
            std::ifstream file(inFilepath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open file -> " + inFilepath);
            }
    
            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);
    
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
    
            return buffer;
        }

        ImageData* readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            int& outFormat,
            const std::string& inFilepath
        )
        {
            outFormat = STBI_rgb_alpha;

            ImageData* result = stbi_load(
                inFilepath.c_str(),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (result == nullptr)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }

        ImageData* readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::vector<unsigned char>& inData
        )
        {
            ImageData* result = stbi_load_from_memory(
                &inData[0],
                static_cast<int>(inData.size()),
                &outWidth,
                &outHeight,
                &outChannel,
                STBI_rgb_alpha
            );

            if (!result)
            {
                throw std::runtime_error(stbi_failure_reason());
            }

            return result;
        }
    }
}