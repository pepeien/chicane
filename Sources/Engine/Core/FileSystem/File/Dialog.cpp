#include "Chicane/Core/FileSystem/File/Dialog.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#if IS_WINDOWS
    #include <windows.h>
    #include <tchar.h>
    #include <shellapi.h>
#endif

namespace Chicane
{
    namespace FileSystem
    {
        FileDialog::FileDialog()
            : Dialog()
        {}

        void FileDialog::open(const Callback& inCallback) const
        {
            String filepaths = "";

#if IS_WINDOWS
/*
            String command = "";

            if (filters.empty())
            {
                command = "All files";
            }
            else
            {
                String extensions = "";
                for (const FileFilter& filter : filters)
                {
                    if (filter.extensions.empty())
                    {
                        continue;
                    }

                    extensions.append(getExtensionsFilter(filter, ";"));
                    extensions.append(';');
                }
                extensions.popBack();

                command = "All Files (" + extensions + ")\0" + extensions + "\0";
            }

            std::uint32_t filterCount = 0U;
            for (const FileFilter& filter : filters)
            {
                if (filter.extensions.empty())
                {
                    continue;
                }

                const String title =
                    filter.title.isEmpty() ? String("File filter " + std::to_string(filterCount)) : filter.title;
                String extensions = getExtensionsFilter(filter, ";");

                command.append(title.trim() + " (" + extensions + ")\0" + extensions + "\0");

                filterCount++;
            }
            command.append('\0');

            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner   = NULL;

            wchar_t* filepath = new wchar_t();
            ZeroMemory(filepath, sizeof(filepath));

            ofn.lpstrFile = filepath;
            ofn.nMaxFile  = MAX_PATH;
            ofn.Flags     = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

            std::wstring sFileFilter = std::wstring(filters.begin(), filters.end());
            ofn.lpstrFilter          = sFileFilter.c_str();

            std::wstring sInTitle = std::wstring(ititle.begin(), title.end());
            ofn.lpstrTitle        = sInTitle.c_str();

            if (GetOpenFileName(&ofn))
            {
                std::wstring filePathWString(ofn.lpstrFile);

                filepaths = String(filePathWString.begin(), filePathWString.end());

                delete filepath;
            }
*/
#elif IS_LINUX
            String command = "zenity";

            if (!title.isEmpty())
            {
                command.append(" --title='" + title.trim() + "'");
            }

            command.append(" --file-selection");

            if (bCanSelectMany)
            {
                command.append(" --multiple");
                command.append(" --separator=','");
            }

            if (filters.empty())
            {
                command.append(" --file-filter='All files | *'");
            }
            else
            {
                String extensionsDisplay = "";
                String extensionsFilter  = "";
                for (const FileFilter& filter : filters)
                {
                    if (filter.extensions.empty())
                    {
                        continue;
                    }

                    extensionsDisplay.append(getExtensionsFilter(filter, ","));
                    extensionsDisplay.append(',');

                    extensionsFilter.append(getExtensionsFilter(filter, " "));
                    extensionsFilter.append(" ");
                }
                extensionsDisplay.popBack();
                extensionsFilter.popBack();

                command.append(" --file-filter='All files (" + extensionsDisplay + ") | " + extensionsFilter + "'");
            }

            std::uint32_t filterCount = 0U;
            for (const FileFilter& filter : filters)
            {
                if (filter.extensions.empty())
                {
                    continue;
                }

                const String title =
                    filter.title.isEmpty() ? String("File filter " + std::to_string(filterCount)) : filter.title;
                const String extensionsDisplay = getExtensionsFilter(filter, ",");
                const String extensionsFilter  = getExtensionsFilter(filter, " ");
                command.append(
                    " --file-filter='" + title.trim() + " (" + extensionsDisplay + ") | " + extensionsFilter + "'"
                );

                filterCount++;
            }

            char buffer[256];

            FILE* pipe = popen(command.toChar(), "r");

            if (!pipe)
            {
                throw std::runtime_error("Failed to open file dialog [" + title.trim() + "]");
            }

            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                filepaths += buffer;
            }

            pclose(pipe);
#endif

            if (!inCallback)
            {
                return;
            }

            if (!filepaths.isEmpty() && filepaths.back() == '\n')
            {
                filepaths.popBack();
            }

            Item::List result;
            for (const String& filepath : filepaths.split(','))
            {
                result.push_back(Item(ItemType::File, FileSystem::Path(filepath.toStandard())));
            }

            inCallback(result);
        }

        String FileDialog::getExtensionsFilter(const FileFilter& inFilter, const String& inSeparator) const
        {
            if (inFilter.extensions.empty())
            {
                return "";
            }

            String result = "";

            for (const FileSystem::Path& extension : inFilter.extensions)
            {
                const String value = extension.string();

                if (value.isEmpty())
                {
                    continue;
                }

                result.append("*" + value.trim() + inSeparator);
            }
            result.popBack();

            return result.trim();
        }
    }
}