#include "Chicane/Smoke/Module.reflected.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        Module* Module::create(const XmlNode& inNode)
        {
            if (inNode.isEmpty() || !inNode.isElement())
            {
                return nullptr;
            }

            const String              tag = inNode.getName();
            const ReflectionTypeInfo* type =
                ReflectionTypeRegistry::getInstance().find(String("Chicane::Smoke::") + tag);

            if (!type)
            {
                type = ReflectionTypeRegistry::getInstance().find(tag);
            }

            if (!type && !tag.isEmpty())
            {
                const String pascal = tag.substr(0, 1).toUpper() + tag.substr(1);
                if (!pascal.equals(tag))
                {
                    type = ReflectionTypeRegistry::getInstance().find(String("Chicane::Smoke::") + pascal);
                }
            }

            if (!type)
            {
                return nullptr;
            }

            return type->create<Module>({inNode});
        }

        Module::Module()
            : Serializable(),
              m_tag(""),
              m_bIsRefreshing(false)
        {}

        Module::Module(const XmlNode& inNode)
            : Module()
        {
            parse(inNode);
        }

        Module::~Module() = default;

        void Module::tick(float, Particle::List&, const PlayInfo&)
        {}

        void Module::collect(const Particle::List&, const PlayInfo&, Particle::List&) const
        {}

        void Module::onAttributeRefresh()
        {}

        void Module::watchRefresh(const String& inName)
        {
            watchAttribute(
                inName,
                [this](const String&)
                {
                    if (m_bIsRefreshing)
                    {
                        return;
                    }

                    onAttributeRefresh();
                }
            );
        }

        void Module::onAttributeSync()
        {
            m_bIsRefreshing = true;
            Serializable::onAttributeSync();
            m_bIsRefreshing = false;

            m_tag = getSource().isEmpty() ? String::empty() : getSource().getName();
            onAttributeRefresh();
        }

        const String& Module::getTag() const
        {
            return m_tag;
        }
    }
}
