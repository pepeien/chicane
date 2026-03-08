#include "Chicane/Core/FileSystem/File/Dialog.hpp"

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
        FileDialog::FileDialog()
            : Dialog()
        {}

        void FileDialog::open(const Callback& inCallback) const
        {
            String filepaths = "";

#if IS_WINDOWS
            std::wstring wInTitle  = std::wstring(title.begin(), title.end());
            std::wstring wLocation = std::wstring(location.begin(), location.end());

            IFileOpenDialog* dialog = nullptr;
            if (SUCCEEDED(CoCreateInstance(
                    CLSID_FileOpenDialog,
                    nullptr,
                    CLSCTX_ALL,
                    IID_IFileOpenDialog,
                    reinterpret_cast<void**>(&dialog)
                )))
            {
                DWORD options;
                dialog->GetOptions(&options);
                dialog->SetOptions(
                    options | FOS_NOCHANGEDIR | FOS_FILEMUSTEXIST | (bCanSelectMany ? FOS_ALLOWMULTISELECT : 0)
                );

                dialog->SetTitle(wInTitle.c_str());

                IShellItem* locationFolder = NULL;
                if (SUCCEEDED(SHCreateItemFromParsingName(wLocation.c_str(), NULL, IID_PPV_ARGS(&locationFolder))))
                {
                    dialog->SetFolder(locationFolder);

                    locationFolder->Release();
                }

                std::vector<COMDLG_FILTERSPEC> specs;
                std::vector<std::wstring>      specNames;
                std::vector<std::wstring>      specPatterns;

                if (!filters.empty())
                {
                    String allExtensions = "";
                    for (const FileFilter& filter : filters)
                    {
                        if (filter.extensions.empty())
                        {
                            continue;
                        }

                        allExtensions.append(getExtensionsFilter(filter, ";"));
                        allExtensions.append(';');
                    }
                    allExtensions.popBack();

                    specNames.push_back(std::wstring(allExtensions.begin(), allExtensions.end()));
                    specNames.back() = L"All Files (" + specNames.back() + L")";
                    specPatterns.push_back(std::wstring(allExtensions.begin(), allExtensions.end()));

                    std::uint32_t filterCount = 0U;
                    for (const FileFilter& filter : filters)
                    {
                        if (filter.extensions.empty())
                        {
                            continue;
                        }

                        const String filterTitle = filter.title.isEmpty()
                                                       ? String("File filter " + std::to_string(filterCount))
                                                       : filter.title;

                        String extensions = getExtensionsFilter(filter, ";");
                        String fullTitle  = filterTitle.trim() + " (" + extensions + ")";

                        specNames.push_back(std::wstring(fullTitle.begin(), fullTitle.end()));
                        specPatterns.push_back(std::wstring(extensions.begin(), extensions.end()));

                        filterCount++;
                    }

                    for (std::size_t i = 0; i < specNames.size(); ++i)
                    {
                        specs.push_back({specNames[i].c_str(), specPatterns[i].c_str()});
                    }

                    dialog->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
                }

                if (SUCCEEDED(dialog->Show(nullptr)))
                {
                    if (bCanSelectMany)
                    {
                        IShellItemArray* items = nullptr;
                        dialog->GetResults(&items);

                        DWORD count = 0;
                        items->GetCount(&count);

                        for (DWORD i = 0; i < count; ++i)
                        {
                            IShellItem* item = nullptr;
                            items->GetItemAt(i, &item);

                            PWSTR path = nullptr;
                            item->GetDisplayName(SIGDN_FILESYSPATH, &path);

                            std::wstring ws(path);
                            filepaths.append(String(ws.begin(), ws.end()));
                            filepaths.append('\0');

                            CoTaskMemFree(path);
                            item->Release();
                        }

                        items->Release();
                    }
                    else
                    {
                        IShellItem* item = nullptr;
                        dialog->GetResult(&item);

                        PWSTR path = nullptr;
                        item->GetDisplayName(SIGDN_FILESYSPATH, &path);

                        std::wstring ws(path);
                        filepaths = String(ws.begin(), ws.end());

                        CoTaskMemFree(path);

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

        void FileDialog::addFilter(const String& inTitle, const FileFilter::Extensions& inExtensions)
        {
            FileFilter filter;
            filter.title      = inTitle;
            filter.extensions = inExtensions;

            filters.push_back(filter);
        }
    }
}