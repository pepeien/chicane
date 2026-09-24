#pragma once

#include <array>
#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Sampler.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER LScene : public Layer
        {
        public:
            LScene();

        public:
            void onInit() override;
            void onDestruction() override;

            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

            RHI::Buffer                                  modelVertexBuffer;
            RHI::Buffer                                  modelIndexBuffer;
            RHI::Image                                   shadowImage;
            std::array<RHI::Image, SHADOW_CASCADE_COUNT> shadowLayerViews = {};
            RHI::Sampler                                 shadowSampler;
            RHI::Image                                   skyImage;
            RHI::Sampler                                 skySampler;
            RHI::Image                                   skyPanoramaImage;
            RHI::Sampler                                 skyPanoramaSampler;

        private:
            void buildModelBuffers();
            void destroyModelBuffers();
            void buildShadowImage();
            void destroyShadowImage();
            void buildSkyImage();
            void destroySkyImage();
            void buildSkyPanoramaImage();
            void destroySkyPanoramaImage();
            void buildLayers();
        };
    }
}
