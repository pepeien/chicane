#include "Chicane/Box/Model/Manager.hpp"

#include "Chicane/Box/Model/Wavefront.hpp"

#include "Chicane/Core/Log.hpp"

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

            for (const auto& [name, model] : data)
            {
                String id = inId;
                id.append("_");
                id.append(name);

                Log::info(id);

                add(id, model);
            }
        }
    }
}