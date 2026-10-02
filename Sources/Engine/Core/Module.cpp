#include "Chicane/Core/Module.hpp"

#include <unordered_map>

#include <SDL3/SDL_loadso.h>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Module/Entry.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Module
    {
        static std::unordered_map<String, Entry>& entries()
        {
            static std::unordered_map<String, Entry> instance;

            return instance;
        }

        static String sharedExtension()
        {
#if IS_WINDOWS
            return ".dll";
#else
            return ".so";
#endif
        }

        static String keyFor(const FileSystem::Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return {};
            }

            return inPath.lexicallyNormal().toString();
        }

        static FileSystem::Path resolvePath(const FileSystem::Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return {};
            }

            FileSystem::Path path = inPath;
            if (!path.hasExtension())
            {
                path = FileSystem::Path(path.toString() + sharedExtension());
            }

            const FileSystem::Path resolved = FileSystem::resolve(path, FileSystem::executableDirectory());
            if (!resolved.isEmpty())
            {
                return resolved.lexicallyNormal();
            }

            return (FileSystem::executableDirectory() / path).lexicallyNormal();
        }

        bool isLoaded(const FileSystem::Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return false;
            }

            const FileSystem::Path path = resolvePath(inPath);
            const String           key  = keyFor(path);

            return entries().find(key) != entries().end();
        }

        bool load(const FileSystem::Path& inPath)
        {
            if (inPath.isEmpty())
            {
                return false;
            }

            const FileSystem::Path path = resolvePath(inPath);
            const String           key  = keyFor(path);

            if (entries().find(key) != entries().end())
            {
                return true;
            }

            SDL_SharedObject* handle = SDL_LoadObject(path.toString().toChar());
            if (!handle)
            {
                Log::warning("Failed to load module [%s]: %s", path.toString().toChar(), SDL_GetError());

                return false;
            }

            auto* init = reinterpret_cast<InitFn>(SDL_LoadFunction(handle, "ChicaneModuleInit"));
            if (!init)
            {
                Log::warning("Module [%s] is missing ChicaneModuleInit: %s", path.toString().toChar(), SDL_GetError());
                SDL_UnloadObject(handle);

                return false;
            }

            if (!init())
            {
                Log::warning("Module [%s] ChicaneModuleInit failed", path.toString().toChar());
                SDL_UnloadObject(handle);

                return false;
            }

            Entry entry;
            entry.handle   = handle;
            entry.shutdown = reinterpret_cast<ShutdownFn>(SDL_LoadFunction(handle, "ChicaneModuleShutdown"));

            entries().emplace(key, entry);

            Log::info("Loaded module [%s]", path.toString().toChar());

            return true;
        }

        void unloadAll()
        {
            for (auto& [path, entry] : entries())
            {
                if (entry.shutdown)
                {
                    entry.shutdown();
                }

                if (entry.handle)
                {
                    SDL_UnloadObject(static_cast<SDL_SharedObject*>(entry.handle));
                    entry.handle = nullptr;
                }
            }

            entries().clear();
        }
    }
}
