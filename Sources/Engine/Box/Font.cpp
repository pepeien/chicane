#include "Chicane/Box/Font.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

#include "Chicane/Box/Font/TrueType.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<FontVendor, String> EXTENSIONS = {
            {FontVendor::Undefined, "N/A"},
            {FontVendor::OpenType,  "OTF"},
            {FontVendor::TrueType,  "TTF"},
        };

        FontVendor Font::parseVendor(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return FontVendor::Undefined;
            }

            const String& value = inValue.trim().toUpper();

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!value.contains(extension))
                {
                    continue;
                }

                return type;
            }

            return FontVendor::Undefined;
        }

        const String& Font::getVendorExtension(FontVendor inValue)
        {
            const auto& found = EXTENSIONS.find(inValue);

            if (found == EXTENSIONS.end())
            {
                return EXTENSIONS.at(FontVendor::Undefined);
            }

            return found->second;
        }

        Font::Font(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(FontVendor::Undefined),
              m_raw({}),
              m_data({}),
              m_instances({})
        {
            fetchVendorFromXML();
            fetchDataFromXML();
        }

        FontVendor Font::getVendor() const
        {
            return m_vendor;
        }

        void Font::setVendor(const String& inValue)
        {
            setVendor(parseVendor(inValue));
        }

        void Font::setVendor(FontVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, getVendorExtension(m_vendor));
        }

        const FontFamily& Font::getData() const
        {
            return m_data;
        }

        const FontFamily& Font::getData(float inWeight) const
        {
            if (m_instances.empty())
            {
                return m_data;
            }

            if (!m_data.isVariable())
            {
                return m_data;
            }

            const float weight = std::clamp(inWeight, m_data.getWeightMin(), m_data.getWeightMax());
            const int   key    = static_cast<int>(std::round(weight));

            const auto found = m_instances.find(key);
            if (found != m_instances.end())
            {
                return found->second;
            }

            const FontFamily* nearest = &m_data;
            int               delta   = std::numeric_limits<int>::max();
            for (const auto& [instanceKey, instance] : m_instances)
            {
                const int distance = std::abs(instanceKey - key);
                if (distance < delta)
                {
                    delta   = distance;
                    nearest = &instance;
                }
            }

            return *nearest;
        }

        void Font::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Font source file was not found");
            }

            setVendor(inFilepath.extension().toString());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Font::setData(const FontRaw& inData)
        {
            if (!getXML().text().set(Base64::encode(inData).toChar()))
            {
                throw std::runtime_error("Failed to save the font [" + m_header.filepath.toString() + "] data");
            }

            m_raw  = inData;
            m_data = parseData(inData);
            rebuildInstances();
            notify(this);
        }

        std::vector<const FontFamily*> Font::getInstances() const
        {
            std::vector<const FontFamily*> result;
            result.reserve(m_instances.size());

            for (const auto& [weight, instance] : m_instances)
            {
                result.push_back(&instance);
            }

            if (result.empty())
            {
                result.push_back(&m_data);
            }

            return result;
        }

        void Font::fetchVendorFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_vendor = parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Font::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_raw  = Base64::decodeToUnsigned(getXML().text().as_string());
            m_data = parseData(m_raw);
            rebuildInstances();
        }

        void Font::rebuildInstances()
        {
            m_instances.clear();
            m_instances.emplace(static_cast<int>(std::round(m_data.getWeight())), m_data);

            if (!m_data.isVariable())
            {
                return;
            }

            const int min = static_cast<int>(std::ceil(m_data.getWeightMin()));
            const int max = static_cast<int>(std::floor(m_data.getWeightMax()));

            for (int weight = 100; weight <= 900; weight += 100)
            {
                if (weight < min || weight > max || m_instances.find(weight) != m_instances.end())
                {
                    continue;
                }

                m_instances.emplace(weight, parseData(m_raw, static_cast<float>(weight)));
            }
        }

        FontFamily Font::parseData(const FontRaw& inValue) const
        {
            return parseData(inValue, -1.0f);
        }

        FontFamily Font::parseData(const FontRaw& inValue, float inWeight) const
        {
            switch (m_vendor)
            {
            case FontVendor::OpenType:
            case FontVendor::TrueType:
                return FontTrueType::parse(getId(), inValue, inWeight);

            default:
                throw std::runtime_error("Failed to parse Font due to invalid vendor");
            }
        }
    }
}