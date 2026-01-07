#pragma once

#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        class Context;
        class Device;

        class CHICANE_SCREECH Manager
        {
        public:
            Manager();

            virtual ~Manager();

        public:
            std::vector<Device> getDevices() const;

        private:
            void initContext();
            void destroyContext();

            void initEngine();
            void destroyEngine();
        };
    }
}