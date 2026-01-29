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
            switch (inData.getVendor())
            {
            case ModelVendor::Wavefront:
                add(inId, ModelWavefront::parse(inData.getData()));

                break;

            default:
                throw std::runtime_error("Failed to import Model due to invalid type");
            }
        }
    }
}