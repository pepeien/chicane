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
            Box::Instance instance = Box::read(inInstance.filepath);

            for (Box::Entry entry : instance.entries)
            {
                if (entry.reference.empty())
                {
                    continue;
                }

                load(entry.reference);
            }
        }

        void loadCubemap(const Box::Instance& inInstance)
        {
            Box::Instance instance = Box::read(inInstance.filepath);

            if (instance.entries.size() < CUBEMAP_IMAGE_COUNT)
            {
                throw std::runtime_error("The [" + instance.name + "] cubemap isn't valid");
            }

            std::vector<Box::Entry> textures = {};

            for (Box::Entry entry : instance.entries)
            {
                if (entry.reference.empty())
                {
                    continue;
                }

                textures.push_back(Box::read(entry.reference).entries[0]);
            }

            m_cubemapManager->add(
                "Skybox",
                {
                    textures[0].data, // Positive X
                    textures[1].data, // Negative X
                    textures[2].data, // Positive Y
                    textures[3].data, // Negative Y
                    textures[4].data, // Positive Z
                    textures[5].data  // Negative Z
                }
            );
        }

        void loadModel(const Box::Instance& inInstance)
        {
            if (m_modelManager->contains(inInstance.name))
            {
                m_modelManager->use(inInstance.name);

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

        void load(const Box::Instance& inInstance)
        {
            switch (inInstance.type)
            {
            case Box::Type::Mesh:
                loadMesh(inInstance);

                return;

            case Box::Type::CubeMap:
                loadCubemap(inInstance);

                return;

            case Box::Type::Texture:
                loadTexture(inInstance);

                return;

            case Box::Type::Model:
                loadModel(inInstance);

                return;

            default:
                return;
            }
        }

        void load(const std::string& inFilePath)
        {
            Box::Instance headerOnlyInstance = Box::readHeader(inFilePath);

            load(headerOnlyInstance);
        }

        void reset()
        {
            m_cubemapManager.reset();
            m_textureManager.reset();
            m_modelManager.reset();
        }
    }
}