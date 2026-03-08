#include "Chicane/Core/FileSystem/Folder/Dialog.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#if IS_WINDOWS
    #include <windows.h>
    #include <tchar.h>
    #include <shellapi.h>
    #include <shobjidl.h>
#endif

namespace Chicane
{
    namespace FileSystem
    {
        FolderDialog::FolderDialog()
            : Dialog()
        {}

        void FolderDialog::open(const Callback& inCallback) const
        {
            String filepath = "";

#if IS_WINDOWS
            std::wstring wTitle    = std::wstring(title.begin(), title.end());
            std::wstring wLocation = std::wstring(location.begin(), location.end());

            IFileOpenDialog* dialog = nullptr;
            if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog))))
            {
                DWORD options = 0;
                dialog->GetOptions(&options);
                dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR);

                dialog->SetTitle(wTitle.c_str());

                IShellItem* locationFolder = NULL;
                if (SUCCEEDED(SHCreateItemFromParsingName(wLocation.c_str(), NULL, IID_PPV_ARGS(&locationFolder))))
                {
                    dialog->SetFolder(locationFolder);

                    locationFolder->Release();
                }

                if (SUCCEEDED(dialog->Show(nullptr)))
                {
                    IShellItem* item = nullptr;
                    if (SUCCEEDED(dialog->GetResult(&item)))
                    {
                        PWSTR path = nullptr;
                        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                        {
                            std::wstring wPath(path);

                            filepath = String(wPath.begin(), wPath.end());

                            CoTaskMemFree(path);
                        }

                        item->Release();
                    }
                }

                dialog->Release();
            }
#elif IS_LINUX
            String command = "zenity";

            if (!title.isEmpty())
            {
                command.append(" --title='" + title.trim() + "'");
            }

            if (!location.isEmpty())
            {
                command.append(" --filename='" + location.trim() + "'");
            }

            command.append(" --file-selection");
            command.append(" --directory");

            char buffer[256];

            FILE* pipe = popen(command.toChar(), "r");

            if (!pipe)
            {
                throw std::runtime_error("Failed to open folder dialog [" + title.trim() + "]");
            }

            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                filepath += buffer;
            }

            pclose(pipe);
#endif

            if (!inCallback)
            {
                return;
            }

            if (!filepath.isEmpty() && filepath.back() == '\n')
            {
                filepath.popBack();
            }

            inCallback(ls(filepath.toStandard()));
        }
    }
}