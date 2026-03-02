#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"
#include "Chicane/Core/FileSystem/File/Filter.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FileDialog : Dialog
        {
        public:
            FileDialog();

        public:
            void open(const Callback& inCallback) const override;

        public:
            void addFilter(const String& inTitle, const FileFilter::Extensions& inExtensions);

        private:
            String getExtensionsFilter(const FileFilter& inFilter, const String& inSeparator) const;

        public:
            FileFilter::List filters = {};
        };
    }
}