#pragma once

#include "Base.hpp"

#include "Core/Controller.hpp"
#include "Core/Window.hpp"

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Debug.hpp"
#include "Descriptor.hpp"
#include "Device.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Instance.hpp"
#include "Queue.hpp"
#include "Shader.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "Sync.hpp"
#include "Vertex.hpp"

#include "CommandBuffer/Instance.hpp"
#include "CommandBuffer/Pool.hpp"
#include "CommandBuffer/Worker.hpp"

#include "CubeMap/Instance.hpp"
#include "CubeMap/Manager.hpp"

#include "Frame/Buffer.hpp"
#include "Frame/Instance.hpp"

#include "Level/Instance.hpp"
#include "Level/Actor/Instance.hpp"
#include "Level/Actor/Pawn.hpp"

#include "Mesh/Manager.hpp"
#include "Mesh/Instance.hpp"

#include "Model/Manager.hpp"
#include "Model/Instance.hpp"

#include "Texture/Manager.hpp"
#include "Texture/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Application
        {
        public:
            Application(
                const std::string& inWindowTitle,
                const Level::Instance& inLevel
            );
            ~Application();

        public:
            void run();

        private:
            // Render
            void drawSky(
                const vk::CommandBuffer& inCommandBuffer,
                uint32_t inImageIndex
            );

            void prepareLevel(const vk::CommandBuffer& inCommandBuffer);
            void drawLevel(const vk::CommandBuffer& inCommandBuffer);
            void drawScene(
                const vk::CommandBuffer& inCommandBuffer,
                uint32_t inImageIndex
            );

            void render();

            // Stats
            void calculateFrameRate();

            // Window
            void initSDL();
            void buildWindow();
            void onWindowEvent(const SDL_WindowEvent& inEvent);
            void onKeyboardInput(const SDL_KeyboardEvent& inEvent);

            // Vulkan
            void buildInstance();
            void destroyInstance();

            void buildDebugMessenger();
            void destroyDebugMessenger();

            void buildSurface();
            void destroySurface();

            void buildQueues();

            void buildDevices();
            void destroyDevices();

            void buildSwapChain();
            void rebuildSwapChain();
            void destroySwapChain();

            void initializeDescriptors();
            void buildSkyDescriptorSetLayouts();
            void buildSceneDescriptorSetLayouts();
            void buildDescriptorSetLayouts();

            void buildSkyGraphicsPipeline();
            void buildSceneGraphicsPipeline();
            void buildGraphicsPipelines();
            void destroyGraphicsPipelines();

            void buildFramebuffers();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();
            void buildFramesCommandBuffers();

            void includeCubeMaps();
            void buildCubeMaps();

            void includeModels();
            void buildModels();

            void includeTextures();
            void buildTextures();

            void includeAssets();
            void buildAssets();
            void destroyAssets();

            void prepareFrame(Frame::Instance& outFrame);
            void buildFrameResources();

            void buildMaterialResources();

            void prepareCamera(Frame::Instance& outFrame);
            void prepareActors(Frame::Instance& outFrame);

        private:
            // Stats
            Frame::Stats m_frameStats;

            // Instance
            vk::Instance m_instance;
            vk::DispatchLoaderDynamic m_dldi;
            vk::DebugUtilsMessengerEXT m_debugMessenger;

            // Surface
            vk::SurfaceKHR m_surface;

            // Devices
            vk::PhysicalDevice m_physicalDevice;
            vk::Device m_logicalDevice;

            // Queues
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;

            // Swap Chain
            SwapChain::Bundle m_swapChain;

            // Graphics Pipline
            std::unordered_map<
                GraphicsPipeline::Type,
                std::unique_ptr<GraphicsPipeline::Instance>
            > m_graphicPipelines;

            std::unordered_map<
                GraphicsPipeline::Type,
                Descriptor::Bundle
            > m_frameDescriptors;

            std::unordered_map<
                GraphicsPipeline::Type,
                Descriptor::Bundle
            > m_materialDescriptors;
    
            // Command
            vk::CommandPool m_mainCommandPool;
            vk::CommandBuffer m_mainCommandBuffer;

            // Frame
            int m_maxInFlightFramesCount;
            int m_currentImageIndex;

            // Mesh
            Buffer::Instance m_meshVertexBuffer;
            Buffer::Instance m_meshIndexBuffer;

            std::unique_ptr<CubeMap::Manager::Instance> m_cubeMapManager;
            std::unique_ptr<Model::Manager::Instance> m_modelManager;
            std::unique_ptr<Texture::Manager::Instance> m_textureManager;

            // Level
            Level::Instance m_level;

            // Camera
            std::unique_ptr<Camera::Instance> m_camera;

            // Window
            Window::Instance m_window;
        };
    }
}