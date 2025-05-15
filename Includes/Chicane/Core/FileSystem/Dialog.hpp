#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Dialog
        {
        public:
            typedef SDL_DialogFileCallback Callback;

        public:
            struct CHICANE_CORE Result
            {
            public:
                static std::vector<Result> fromRaw(const char* const* inData);

            public:
                std::filesystem::path path;
            };

        public:
            std::string title         = "Dialog";
            std::string location      = "";

            bool        canSelectMany = false;
        };
    }
}