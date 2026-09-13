#include "Chicane/Smoke/Module.reflected.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        Module* Module::create(const pugi::xml_node& inNode)
        {
            if (inNode.empty() || inNode.type() != pugi::node_element)
            {
                return nullptr;
            }

            const String              tag = inNode.name();
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
            : m_tag("")
        {}

        Module::Module(const pugi::xml_node& inNode)
            : m_tag(inNode.name())
        {}

        Module::~Module() = default;

        void Module::tick(float, Particle::List&, const PlayInfo&) {}

        void Module::collect(const Particle::List&, const PlayInfo&, Particle::List&) const {}

        const String& Module::getTag() const
        {
            return m_tag;
        }
    }
}
