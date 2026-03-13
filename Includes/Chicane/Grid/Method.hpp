#pragma once

#include "Chicane/Core/Reflection/Method/Info.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Method
        {
        public:
            using Param  = std::any;
            using Params = std::vector<Param>;

        public:
            Method();

        public:
            bool isValid() const;

            void invoke(void* inInstance) const;

            void setInstance(const ReflectionMethodInfo* inValue);

            void addParam(Param inValue);

        private:
            const ReflectionMethodInfo* m_instance;
            Params                      m_params;
        };
    }
}