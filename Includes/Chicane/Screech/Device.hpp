#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Device/Format.hpp"
#include "Chicane/Screech/Device/Id.hpp"
#include "Chicane/Screech/Device/Type.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_device_info`
        struct CHICANE_SCREECH Device
        {
        public:
            using Formats = std::vector<DeviceFormat>;

        public:
            Device(void* inData, DeviceType inType);
            Device();

        public:
            void* getInstance() const;

        private:
            void*      m_instance;

            bool       m_bIsSystemDefault;

            String     m_name;

            DeviceId   m_id;
            DeviceType m_type;

            Formats    m_formats;
        };
    }
}