#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Debug.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Queue.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Surface.hpp"
#include "Renderer/SwapChain.hpp"
#include "Renderer/Sync.hpp"
#include "Renderer/Vertex.hpp"

#include "Renderer/CommandBuffer/Instance.hpp"
#include "Renderer/CommandBuffer/Pool.hpp"
#include "Renderer/CommandBuffer/Worker.hpp"

#include "Renderer/CubeMap/Instance.hpp"
#include "Renderer/CubeMap/Manager.hpp"

#include "Renderer/Frame/Buffer.hpp"
#include "Renderer/Frame/Instance.hpp"

#include "Renderer/Model/Manager.hpp"
#include "Renderer/Model/Instance.hpp"

#include "Renderer/Texture/Manager.hpp"
#include "Renderer/Texture/Instance.hpp"

namespace Engine
{
    // Core
    class Window;

    // Game
    class Actor;
    class Controller;
    class Level;

    class Renderer
    {
    public:
        Renderer(
            Window* inWindow,
            Level* inLevel
        );
        ~Renderer();

    public:
        void onEvent(const SDL_Event& inEvent);

        void render();

        void updateStats();

    private:
        // Render
        void updateViewport(
            const vk::CommandBuffer& inCommandBuffer
        );

        void prepareSky(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        );
        void drawSky(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        );

        void prepareScene(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        );
        void drawScene(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        );

        // Window
        void onKeyboardEvent(const SDL_KeyboardEvent& inEvent);
        void onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent);

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

        void prepareCamera(Frame::Instance& outFrame);
        void prepareActors(Frame::Instance& outFrame);
        void prepareFrame(Frame::Instance& outFrame);
        void buildFrameResources();

        void buildMaterialResources();

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
        int m_maxInFlightImageCount;
        int m_currentImageIndex;

        // Mesh
        Buffer::Instance m_meshVertexBuffer;
        Buffer::Instance m_meshIndexBuffer;

        std::unique_ptr<CubeMap::Manager::Instance> m_cubeMapManager;
        std::unique_ptr<Model::Manager::Instance> m_modelManager;
        std::unique_ptr<Texture::Manager::Instance> m_textureManager;

        // Level
        Level* m_level;

        // Camera
        std::unique_ptr<Camera::Instance> m_camera;

        // Window
        Window* m_window;
    };
}