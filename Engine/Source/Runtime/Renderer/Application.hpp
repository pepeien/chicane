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
#include "Uniform.hpp"
#include "Vertex.hpp"

#include "CommandBuffer/Instance.hpp"
#include "CommandBuffer/Pool.hpp"
#include "CommandBuffer/Worker.hpp"

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
            void drawLevel(const vk::CommandBuffer& inCommandBuffer);
            void draw(
                const vk::CommandBuffer& inCommandBuffer,
                uint32_t inImageIndex
            );
            void render();
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

            void buildFrameDescriptorSetLayout();

            void buildMaterialDescriptorSetLayout();

            void buildGraphicsPipeline();
            void destroyGraphicsPipeline();

            void buildFramebuffers();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();
            void buildFramesCommandBuffers();

            void loadModels();
            void buildModels();

            void loadTextures();
            void buildTextures();
            void destroyTextures();

            void loadAssets();
            void buildAssets();
            void destroyAssets();

            void buildFrameResources();

            void buildMaterialResources();

            void prepareLevel(const vk::CommandBuffer& inCommandBuffer);
            void prepareCamera(Frame::Instance& outFrame);
            void prepareActors(Frame::Instance& outFrame);
            void prepareFrame(uint32_t inImageIndex);

        private:
            // Stats
            Frame::Stats m_frameStats;

            // Vulkan
            vk::Instance m_instance;
            vk::DispatchLoaderDynamic m_dldi;
            vk::DebugUtilsMessengerEXT m_debugMessenger;
            vk::SurfaceKHR m_surface;

            vk::PhysicalDevice m_physicalDevice;
            vk::Device m_logicalDevice;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentQueue;

            SwapChain::Bundle m_swapChain;

            GraphicsPipeline::Bundle m_graphicsPipeline;

            vk::CommandPool m_mainCommandPool;
            vk::CommandBuffer m_mainCommandBuffer;

            int m_maxInFlightFramesCount;
            int m_currentImageIndex;

            Descriptor::Bundle m_frameDescriptors;
            Descriptor::Bundle m_materialDescriptors;

            Buffer::Instance m_meshVertexBuffer;
            Buffer::Instance m_meshIndexBuffer;
        
            std::unique_ptr<Model::Manager::Instance> m_modelManager;
            std::unique_ptr<Texture::Manager::Instance> m_textureManager;

            // Context
            Level::Instance m_level;

            // Camera
            std::unique_ptr<Camera::Instance> m_camera;

            // Window
            Window::Instance m_window;
        };
    }
}