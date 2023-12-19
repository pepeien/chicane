#include "Manager.hpp"

namespace Engine
{
    namespace Texture
    {
        namespace Manager
        {
            Instance::~Instance()
            {
                for (auto [id, texture] : texturesMap)
                {
                    if (texture == nullptr)
                    {
                        continue;
                    }

                    delete texture;
                }
            }

            void Instance::addTexture(const std::string& inTextureId, const Texture::CreateInfo& inCreateInfo)
            {
                if (texturesMap.find(inTextureId) != texturesMap.end())
                {
                    throw std::runtime_error("A Texture identified by [" + inTextureId  + "] already exists");
                }

                Texture::Instance* newTexture = new Texture::Instance(inCreateInfo);

                texturesOrder.push_back(inTextureId);
                texturesMap.insert(std::make_pair(inTextureId, newTexture));
            }

            Texture::Instance* Instance::getTexture(const std::string& inTextureId)
            {
                auto foundTexture = texturesMap.find(inTextureId);

                if (foundTexture == texturesMap.end())
                {
                    throw std::runtime_error("Invalid Texture ID [" + inTextureId + "]");
                }

                return foundTexture->second;
            }

            uint32_t Instance::getCount()
            {
                return texturesMap.size();
            }
        }
	}
}