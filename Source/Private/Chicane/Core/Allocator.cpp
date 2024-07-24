#include "Chicane/Core/Allocator.hpp"

namespace Chicane
{
    namespace Allocator
    {
        std::unique_ptr<CubeMap::Manager> m_cubemapManager = std::make_unique<CubeMap::Manager>();
        std::unique_ptr<Model::Manager> m_modelManager     = std::make_unique<Model::Manager>();
        std::unique_ptr<Texture::Manager> m_textureManager = std::make_unique<Texture::Manager>();

        CubeMap::Manager* getCubemapManager()
        {
            return m_cubemapManager.get();
        }

        Model::Manager* getModelManager()
        {
            return m_modelManager.get();
        }

        Texture::Manager* getTextureManager()
        {
            return m_textureManager.get();
        }

        void loadMesh(const Box::Instance& inInstance)
        {
            for (Box::Entry entry : inInstance.entries)
            {
                if (entry.referenceName.empty())
                {
                    continue;
                }

                switch (entry.type)
                {
                case Box::EntryType::Model:
                    m_modelManager->addDuplicate(entry.referenceName);

                    break;

                default:
                    break;
                }
            }
        }

        void loadModel(const Box::Instance& inInstance)
        {
            if (m_modelManager->contains(inInstance.name))
            {
                m_modelManager->addDuplicate(inInstance.name);

                return;
            }

            m_modelManager->add(
                inInstance.name,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        void loadTexture(const Box::Instance& inInstance)
        {
            if (m_textureManager->contains(inInstance.name))
            {
                return;
            }

            m_textureManager->add(
                inInstance.name,
                Box::read(inInstance.filepath).entries[0]
            );
        }

        void load(const std::string& inFilePath)
        {
            Box::Instance headerOnlyInstance = Box::readHeader(inFilePath);

            load(headerOnlyInstance);
        }
        
        void load(const Box::Instance& inInstance)
        {
            switch (inInstance.type)
            {
            case Box::Type::Texture:
                loadTexture(inInstance);

                return;

            case Box::Type::Model:
                loadModel(inInstance);

                return;
            
            case Box::Type::Mesh:
                loadMesh(Box::read(inInstance.filepath));

                return;

            default:
                return;
            }
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}