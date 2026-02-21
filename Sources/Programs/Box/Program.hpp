#pragma once

#include <Chicane/Core/Program.hpp>

class Program : public Chicane::Program
{
private:
    static constexpr inline const char* ID_OPTION_NAME        = "id";
    static constexpr inline const char* ID_OPTION_DESCRIPTION = "Asset ID";

    static constexpr inline const char* ASSET_OPTION_NAME        = "asset";
    static constexpr inline const char* ASSET_OPTION_DESCRIPTION = "Asset description";

    static constexpr inline const char* OUTPUT_OPTION_NAME        = "output";
    static constexpr inline const char* OUTPUT_OPTION_DESCRIPTION = "Output folder";

public:
    Program();

protected:
    void onExec(const Chicane::ProgramParam& inParam) override;
};