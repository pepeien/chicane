#include "Chicane/Grid/Component/Svg/Scanner.hpp"

#include <cctype>
#include <cstdlib>

namespace Chicane
{
    namespace Grid
    {
        SvgScanner::SvgScanner()
            : p(nullptr),
              end(nullptr)
        {}

        SvgScanner::SvgScanner(const String& inValue)
            : p(inValue.toChar()),
              end(p + inValue.size())
        {}

        void SvgScanner::skip()
        {
            while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ','))
            {
                p++;
            }
        }

        bool SvgScanner::done()
        {
            skip();

            return p >= end;
        }

        bool SvgScanner::hasNumber()
        {
            skip();

            if (p >= end)
            {
                return false;
            }

            return *p == '+' || *p == '-' || *p == '.' || std::isdigit(static_cast<unsigned char>(*p));
        }

        float SvgScanner::number()
        {
            skip();

            char* next  = nullptr;
            float value = std::strtof(p, &next);
            p           = next ? next : p;

            return value;
        }

        bool SvgScanner::flag()
        {
            skip();

            if (p >= end)
            {
                return false;
            }

            const bool value = *p == '1';

            if (*p == '0' || *p == '1')
            {
                p++;
            }

            return value;
        }

        char SvgScanner::command()
        {
            skip();

            if (p >= end || !std::isalpha(static_cast<unsigned char>(*p)))
            {
                return 0;
            }

            return *p++;
        }
    }
}
