#pragma once

#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        class Device;

        class CHICANE_SCREECH Manager
        {
        public:
            Manager();

            virtual ~Manager();

        public:
            void* getContext();

            void* getEngine();

            std::vector<Device> getDevices() const;

        private:
            void initContext();
            void destroyContext();

            void initEngine();
            void destroyEngine();
        };
    }
}