#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE Dialog
        {
        public:
            typedef SDL_DialogFileCallback Callback;

        public:
            struct CHICANE Result
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