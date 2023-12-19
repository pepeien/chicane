#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Engine
{
    namespace Texture
    {
		namespace Manager
		{
			class Instance
			{
			public:
				~Instance();

			public:
				void addTexture(const std::string& inTextureId, const Texture::CreateInfo& inCreateInfo);
				Texture::Instance* getTexture(const std::string& inTextureId); 
				uint32_t getCount();

			private:
				std::vector<std::string> texturesOrder;
				std::unordered_map<std::string, Texture::Instance*> texturesMap;
			};
		}
	}
}