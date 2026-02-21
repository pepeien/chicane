#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Program/Param.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE Program
    {
    public:
        Program(const String& inName);

    protected:
        virtual void onExec(const ProgramParam& inParam) = 0;

    public:
        Program& addOption(const ProgramOptionSetting& inSetting);

        void exec(int inParamCount, char* inParamValue[]);

    protected:
        void showHelp() const;

    private:
        String       m_name;
        ProgramParam m_param;
    };
}