#pragma once

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Program.hpp>

class Program : public Chicane::Program
{
private:
    static constexpr inline const char* ID_OPTION_NAME        = "id";
    static constexpr inline const char* ID_OPTION_DESCRIPTION = "Asset ID";

    static constexpr inline const char* TYPE_OPTION_NAME        = "type";
    static constexpr inline const char* TYPE_OPTION_DESCRIPTION = "Asset type";

    static constexpr inline const char* OUTPUT_OPTION_NAME        = "output";
    static constexpr inline const char* OUTPUT_OPTION_DESCRIPTION = "Output file location";

public:
    Program();

protected:
    void onExec(const Chicane::ProgramParam& inParam) override;

private:
    void createFont(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );

    void createMesh(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );

    void createModel(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );

    void createSky(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );

    void createTexture(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );

    void createSound(
        const Chicane::String&                    inId,
        const Chicane::ProgramParam::Positionals& inSources,
        const Chicane::FileSystem::Path&          inOutput
    );
};