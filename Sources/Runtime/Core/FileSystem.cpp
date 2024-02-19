#include "Runtime/Core/FileSystem.hpp"

#include "Runtime/Core/Helper.hpp"

#ifdef OS_WINDOWS
    #include <windows.h>
    #include <tchar.h>
#endif

namespace Chicane
{
    namespace FileSystem
    {
        FileResult openFileDialog(
            const std::string& inTitle,
            const std::vector<FileFormat>& inFileFormats
        )
        {
            #ifdef OS_WINDOWS
                std::string fileFormatExtensions = "";

                for (FileFormat fileFormat : inFileFormats)
                {
                    fileFormatExtensions.append("*." + fileFormat.extension + ";");
                }

                std::string fileFilter = "All Files (" + fileFormatExtensions + ")";
                fileFilter.push_back('\0');
                fileFilter += fileFormatExtensions;
                fileFilter.push_back('\0');

                for (FileFormat fileFormat : inFileFormats)
                {
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

        std::vector<char> readFile(const std::string& inFilepath)
        {
            std::ifstream file(inFilepath, std::ios::ate | std::ios::binary);

            if (file.is_open() == false)
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

        stbi_uc* readImageFromFile(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::string& inFilepath
        )
        {
            stbi_uc* result = stbi_load(
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

        stbi_uc* readImageFromMemory(
            int& outWidth,
            int& outHeight,
            int& outChannel,
            const std::vector<unsigned char>& inData
        )
        {
            stbi_uc* result = stbi_load_from_memory(
                &inData[0],
                static_cast<int>(inData.size()),
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
    }
}