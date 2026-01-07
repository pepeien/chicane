#include "Chicane/Screech/Device.hpp"

#include <miniaudio.h>

namespace Chicane
{
    namespace Screech
    {
        Device::Device(void* inData, DeviceType inType)
            : Device()
        {
            if (!inData)
            {
                return;
            }

            ma_device_info* data = (ma_device_info*)(inData);

            m_bIsSystemDefault = data->isDefault;
            m_id               = DeviceId((void*) &data->id);
            m_name             = data->name;

            for (std::uint32_t i = 0; i <= data->nativeDataFormatCount; i++)
            {
                DeviceFormat format = {};
                format.format     = (SoundFormat) data->nativeDataFormats[i].format;
                format.channels   = data->nativeDataFormats[i].channels;
                format.sampleRate = data->nativeDataFormats[i].sampleRate;
                format.flags      = data->nativeDataFormats[i].flags;

                m_formats.push_back(format);
            }

            m_type = inType;
        }

        Device::Device()
            : m_bIsSystemDefault(false),
            m_id(),
            m_name(""),
            m_formats(),
            m_type(DeviceType::Unknown)
        {}
    }
}