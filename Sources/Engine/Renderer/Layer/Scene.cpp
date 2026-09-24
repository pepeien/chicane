#include "Chicane/Renderer/Layer/Scene.hpp"

#include <algorithm>

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Layer/Scene/Foreground.hpp"
#include "Chicane/Renderer/Layer/Scene/Line.hpp"
#include "Chicane/Renderer/Layer/Scene/Mesh.hpp"
#include "Chicane/Renderer/Layer/Scene/Particle.hpp"
#include "Chicane/Renderer/Layer/Scene/Shadow.hpp"
#include "Chicane/Renderer/Layer/Scene/Sky.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LScene::LScene()
            : Layer(SCENE_LAYER_ID)
        {}

        void LScene::onInit()
        {
            buildModelBuffers();
            buildShadowImage();
            buildSkyImage();
            buildSkyPanoramaImage();
            buildLayers();
        }

        void LScene::onDestruction()
        {
            destroySkyPanoramaImage();
            destroySkyImage();
            destroyShadowImage();
            destroyModelBuffers();
        }

        void LScene::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType != DrawPolyType::e3D)
            {
                return;
            }

            RHI::Device* device = m_backend->getRHIDevice();
            device->updateBuffer(
                modelVertexBuffer,
                inResource.getVertices().data(),
                sizeof(Vertex) * inResource.getVertices().size()
            );
            device->updateBuffer(
                modelIndexBuffer,
                inResource.getIndices().data(),
                sizeof(Vertex::Index) * inResource.getIndices().size()
            );
        }

        void LScene::buildModelBuffers()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BufferCreateInfo vertex;
            vertex.size  = std::min(m_backend->getResourceBudget(Resource::SceneVertices), RESOURCE_MESH_INITIAL_BYTES);
            vertex.usage = RHI::BufferUsage::Vertex;
            vertex.bHasHostAccess = false;
            modelVertexBuffer     = device->createBuffer(vertex);

            RHI::BufferCreateInfo index;
            index.size  = std::min(m_backend->getResourceBudget(Resource::SceneIndices), RESOURCE_MESH_INITIAL_BYTES);
            index.usage = RHI::BufferUsage::Index;
            index.bHasHostAccess = false;
            modelIndexBuffer     = device->createBuffer(index);
        }

        void LScene::destroyModelBuffers()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            device->destroyBuffer(modelVertexBuffer);
            device->destroyBuffer(modelIndexBuffer);
            modelVertexBuffer = {};
            modelIndexBuffer  = {};
        }

        void LScene::buildShadowImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::ImageCreateInfo desc;
            desc.kind       = RHI::ImageKind::Depth2DArray;
            desc.format     = RHI::ImageFormat::Depth32F;
            desc.width      = SHADOW_MAP_WIDTH;
            desc.height     = SHADOW_MAP_HEIGHT;
            desc.layers     = SHADOW_CASCADE_COUNT;
            desc.bIsSampled = true;
            desc.bHasColor  = false;
            desc.bHasDepth  = true;
            shadowImage     = device->createImage(desc);

            RHI::SamplerCreateInfo sampler;
            sampler.minFilter   = RHI::SamplerFilter::Nearest;
            sampler.magFilter   = RHI::SamplerFilter::Nearest;
            sampler.address     = RHI::SamplerAddress::ClampToBorder;
            sampler.borderColor = Color::Rgba{255, 255, 255, 255};
            shadowSampler       = device->createSampler(sampler);

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; cascade++)
            {
                shadowLayerViews[cascade] = device->createImageView(shadowImage, cascade);
            }
        }

        void LScene::destroyShadowImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; cascade++)
            {
                device->destroyImage(shadowLayerViews[cascade]);
                shadowLayerViews[cascade] = {};
            }
            device->destroySampler(shadowSampler);
            device->destroyImage(shadowImage);
            shadowSampler = {};
            shadowImage   = {};
        }

        void LScene::buildSkyImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::ImageCreateInfo desc;
            desc.kind       = RHI::ImageKind::Cube;
            desc.format     = RHI::ImageFormat::RGBA16F;
            desc.width      = SKY_TEXTURE_SIZE;
            desc.height     = SKY_TEXTURE_SIZE;
            desc.layers     = 6;
            desc.mipLevels  = SKY_MIP_LEVELS;
            desc.bIsSampled = true;
            desc.bHasColor  = false;
            desc.bHasDepth  = false;
            skyImage        = device->createImage(desc);

            RHI::SamplerCreateInfo sampler;
            sampler.minFilter = RHI::SamplerFilter::Linear;
            sampler.magFilter = RHI::SamplerFilter::Linear;
            sampler.address   = RHI::SamplerAddress::ClampToEdge;
            sampler.bHasMip   = true;
            skySampler        = device->createSampler(sampler);
        }

        void LScene::destroySkyImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            device->destroySampler(skySampler);
            device->destroyImage(skyImage);
            skySampler = {};
            skyImage   = {};
        }

        void LScene::buildSkyPanoramaImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::ImageCreateInfo desc;
            desc.kind        = RHI::ImageKind::Color2D;
            desc.format      = RHI::ImageFormat::RGBA16F;
            desc.width       = SKY_PANORAMA_WIDTH;
            desc.height      = SKY_PANORAMA_HEIGHT;
            desc.layers      = 1;
            desc.mipLevels   = 1;
            desc.bIsSampled  = true;
            desc.bHasColor   = true;
            desc.bHasDepth   = false;
            skyPanoramaImage = device->createImage(desc);

            RHI::SamplerCreateInfo sampler;
            sampler.minFilter  = RHI::SamplerFilter::Linear;
            sampler.magFilter  = RHI::SamplerFilter::Linear;
            sampler.address    = RHI::SamplerAddress::Repeat;
            sampler.bHasMip    = false;
            skyPanoramaSampler = device->createSampler(sampler);
        }

        void LScene::destroySkyPanoramaImage()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            device->destroySampler(skyPanoramaSampler);
            device->destroyImage(skyPanoramaImage);
            skyPanoramaSampler = {};
            skyPanoramaImage   = {};
        }

        void LScene::buildLayers()
        {
            ListPush<Layer*> settings;
            settings.strategy = ListPushStrategy::Back;

            m_backend->addLayer<LSceneSky>(settings);
            m_backend->addLayer<LSceneShadow>(settings);
            m_backend->addLayer<LSceneMesh>(settings);
            m_backend->addLayer<LSceneParticle>(settings);
            m_backend->addLayer<LSceneLine>(settings);
            m_backend->addLayer<LSceneForeground>(settings);
        }
    }
}
