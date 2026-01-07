#pragma once

#include "Chicane/Screech/Essential.hpp"
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

        private:
            bool       m_bIsSystemDefault;

            DeviceId   m_id;

            String     m_name;

            Formats    m_formats;

            DeviceType m_type;
        };
    }
}