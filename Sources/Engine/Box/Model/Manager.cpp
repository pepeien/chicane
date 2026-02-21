#include "Chicane/Box/Model/Manager.hpp"

#include "Chicane/Box/Model/Wavefront.hpp"

namespace Chicane
{
    namespace Box
    {
        ModelManager::ModelManager()
            : Manager()
        {}

        void ModelManager::onLoad(const String& inId, const Model& inData)
        {
            ModelParsed::Map data = {};
            switch (inData.getVendor())
            {
            case ModelVendor::Wavefront:
                data = ModelWavefront::parse(inData.getData());

                break;

            default:
                throw std::runtime_error("Failed to import Model due to invalid type");
            }

            Children children = {};
            for (const auto& [name, model] : data)
            {
                String id = inId;
                id.append("_");
                id.append(name);

                add(id, model);

                children.push_back(id);
            }

            m_groups[inId] = children;
        }

        ModelManager::Children ModelManager::getChildren(const String& inId) const
        {
            if (m_groups.find(inId) == m_groups.end())
            {
                return {};
            }

            return m_groups.at(inId);
        }
    }
}