#include "Application.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Application::Application(
            const std::string& inWindowTitle,
            const Level::Instance& inLevel
        )
            : m_frameStats({}),
            m_swapChain({}),
            m_maxInFlightFramesCount(0),
            m_currentImageIndex(0),
            m_meshVertexBuffer({}),
            m_meshIndexBuffer({}),
            m_cubeMapManager(std::make_unique<CubeMap::Manager::Instance>()),
            m_modelManager(std::make_unique<Model::Manager::Instance>()),
            m_textureManager(std::make_unique<Texture::Manager::Instance>()),
            m_level(inLevel),
            m_camera(std::make_unique<Camera::Instance>()),
            m_window({ nullptr, inWindowTitle, 0, 0 })
        {
            // Assets pre load
            includeAssets();

            // Window
            initSDL();
            buildWindow();

            // Vulkan
            initializeDescriptors();

            buildInstance();
            buildDebugMessenger();
            buildSurface();
            buildDevices();
            buildQueues();
            buildSwapChain();
            buildDescriptorSetLayouts();
            buildGraphicsPipelines();
            buildFramebuffers();
            buildCommandPool();
            buildMainCommandBuffer();
            buildFramesCommandBuffers();
            buildFrameResources();
            buildMaterialResources();

            // Assets building
            buildAssets();
        }

        Application::~Application()
        {
            // Vulkan
            m_logicalDevice.waitIdle();

            destroyCommandPool();
            destroyGraphicsPipelines();
            destroySwapChain();

            m_camera.reset();

            for (auto [type, instance] : m_frameDescriptors)
            {
                m_logicalDevice.destroyDescriptorSetLayout(
                    instance.setLayout
                );
            }

            for (auto [type, instance] : m_materialDescriptors)
            {
                m_logicalDevice.destroyDescriptorSetLayout(
                    instance.setLayout
                );
                m_logicalDevice.destroyDescriptorPool(
                    instance.pool
                );
            }

            Buffer::destroy(
                m_logicalDevice,
                m_meshVertexBuffer
            );
            Buffer::destroy(
                m_logicalDevice,
                m_meshIndexBuffer
            );

            destroyAssets();
            destroyDevices();
            destroySurface();

            if (IS_DEBUGGING)
            {
                destroyDebugMessenger();
            }

            destroyInstance();

            // Window
            SDL_DestroyWindow(m_window.instance);
            SDL_Quit();
        }

        void Application::run()
        {
            bool shouldClose = false;

            SDL_Event event;

            while (shouldClose == false)
            {
                while(SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                    case SDL_QUIT:
                        shouldClose = true;

                        break;
                    
                    case SDL_WINDOWEVENT:
                        onWindowEvent(event.window);

                        break;
                    
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        onKeyboardInput(event.key);

                        break;
                    
                    default:
                        break;
                    }
                }

                render();
                calculateFrameRate();
            }
        }

        void Application::drawSky(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        )
        {
            vk::ClearValue clearColor;
            clearColor.color = vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f);

            std::vector<vk::ClearValue> clearValues = {{ clearColor }};

            vk::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass          = m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->renderPass;
            renderPassBeginInfo.framebuffer         = m_swapChain.frames[inImageIndex].framebuffers.at(GraphicsPipeline::Type::SKY);
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
            renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues        = clearValues.data();

            inCommandBuffer.beginRenderPass(
                &renderPassBeginInfo,
                vk::SubpassContents::eInline
            );

            inCommandBuffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->instance
            );
        
            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_graphicPipelines.at(
                    GraphicsPipeline::Type::SKY
                )->layout,
                0,
                m_swapChain.frames[inImageIndex].descriptorSets.at(
                    GraphicsPipeline::Type::SKY
                ),
                nullptr
            );

            m_cubeMapManager->bind(
                "Sky",
                inCommandBuffer,
                m_graphicPipelines.at(
                    GraphicsPipeline::Type::SKY
                )->layout
            );

            m_cubeMapManager->draw(
                "Sky",
                inCommandBuffer
            );

            inCommandBuffer.endRenderPass();
        }

        void Application::prepareLevel(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Buffer vertexBuffers[] = { m_meshVertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            inCommandBuffer.bindVertexBuffers(
                0,
                1,
                vertexBuffers,
                offsets
            );

            inCommandBuffer.bindIndexBuffer(
                m_meshIndexBuffer.instance,
                0,
                vk::IndexType::eUint32
            );
        }

        void Application::drawLevel(const vk::CommandBuffer& inCommandBuffer)
        {
            prepareLevel(inCommandBuffer);

            auto actors  = m_level.getActors();
            auto meshIds = m_modelManager->getRegisteredIds();
        
            std::unordered_map<std::string, uint32_t> usedMeshes;

            for (std::string& meshId : meshIds)
            {
                int referenceCount = std::count_if(
                    actors.begin(),
                    actors.end(),
                    [meshId](const Level::Actor::Pawn& inActor) {
                        return inActor.mesh.id == meshId;
                    }
                );

                usedMeshes.insert(std::make_pair(meshId, referenceCount));
            }

            uint32_t drawCount = 0;

            for (auto& actor : actors)
            {
                m_textureManager->bind(
                    actor.texture.id,
                    inCommandBuffer,
                    m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->layout
                );

                m_modelManager->draw(
                    actor.mesh.id,
                    inCommandBuffer,
                    usedMeshes[actor.mesh.id],
                    drawCount
                );

                drawCount += usedMeshes[actor.mesh.id];
            }
        }

        void Application::drawScene(
            const vk::CommandBuffer& inCommandBuffer,
            uint32_t inImageIndex
        )
        {
            vk::ClearValue clearColor;
            clearColor.color = vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f);

            vk::ClearValue clearDepth;
            clearDepth.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

            std::vector<vk::ClearValue> clearValues = {{ clearColor, clearDepth }};

            vk::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass          = m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->renderPass;
            renderPassBeginInfo.framebuffer         = m_swapChain.frames[inImageIndex].framebuffers.at(GraphicsPipeline::Type::SCENE);
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent   = m_swapChain.extent;
            renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues        = clearValues.data();

            inCommandBuffer.beginRenderPass(
                &renderPassBeginInfo,
                vk::SubpassContents::eInline
            );

            inCommandBuffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->instance
            );

            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->layout,
                0,
                m_swapChain.frames[inImageIndex].descriptorSets.at(GraphicsPipeline::Type::SCENE),
                nullptr
            );

            drawLevel(inCommandBuffer);

            inCommandBuffer.endRenderPass();
        }

        void Application::render()
        {
            m_logicalDevice.waitIdle();

            Frame::Instance& currentFrame = m_swapChain.frames[m_currentImageIndex];

            if (
                m_logicalDevice.waitForFences(
                    1,
                    &currentFrame.renderFence,
                    VK_TRUE,
                    UINT64_MAX
                ) != vk::Result::eSuccess
            )
            {
                LOG_WARNING("Error while waiting the fences");
            }

            if (
                m_logicalDevice.resetFences(
                    1,
                    &currentFrame.renderFence
                ) != vk::Result::eSuccess
            )
            {
                LOG_WARNING("Error while resetting the fences");
            }

            uint32_t imageIndex = 0;

            try
            {
                imageIndex = m_logicalDevice.acquireNextImageKHR(
                    m_swapChain.instance,
                    UINT64_MAX,
                    currentFrame.presentSemaphore,
                    nullptr
                ).value;
            }
            catch(vk::OutOfDateKHRError e)
            {
                rebuildSwapChain();

                return;
            }
            catch(vk::IncompatibleDisplayKHRError e)
            {
                rebuildSwapChain();

                return;
            }

            vk::CommandBuffer& commandBuffer = currentFrame.commandBuffer;

            commandBuffer.reset();

            prepareFrame(m_swapChain.frames[imageIndex]);

            vk::CommandBufferBeginInfo beginInfo = {};
            commandBuffer.begin(beginInfo);

            drawSky(commandBuffer,   imageIndex);
            drawScene(commandBuffer, imageIndex);

            commandBuffer.end();

            vk::Semaphore waitSemaphores[]      = { currentFrame.presentSemaphore };
            vk::Semaphore signalSemaphores[]    = { currentFrame.renderSemaphore };
            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

            vk::SubmitInfo submitInfo = {};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = waitSemaphores;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = signalSemaphores;
            submitInfo.pWaitDstStageMask    = waitStages;

            m_graphicsQueue.submit(
                submitInfo,
                currentFrame.renderFence
            );

            vk::SwapchainKHR swapChains[] = { m_swapChain.instance };

            vk::PresentInfoKHR presentInfo = {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = signalSemaphores;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = swapChains;
            presentInfo.pImageIndices      = &imageIndex;

            vk::Result present;

            try
            {
                present = m_presentQueue.presentKHR(presentInfo);
            }
            catch(vk::OutOfDateKHRError e)
            {
                present = vk::Result::eErrorOutOfDateKHR;
            }

            if (
                present == vk::Result::eErrorOutOfDateKHR ||
                present == vk::Result::eSuboptimalKHR
            )
            {
                rebuildSwapChain();

                return;
            }

            m_currentImageIndex = (m_currentImageIndex + 1) % m_maxInFlightFramesCount;
        }

        void Application::calculateFrameRate()
        {
            m_frameStats.currentTime = SDL_GetTicks64() / 1000;

            double delta = m_frameStats.currentTime - m_frameStats.lastTime;

            if (delta >= 1) {
                int framerate{ std::max(1, int(m_frameStats.count / delta)) };

                std::stringstream title;
                title << m_window.title << " - " << framerate << " FPS";

                SDL_SetWindowTitle(
                    m_window.instance,
                    title.str().c_str()
                );

                m_frameStats.lastTime  = m_frameStats.currentTime;
                m_frameStats.count     = -1;
                m_frameStats.time      = float(1000.0 / framerate);
            }

            m_frameStats.count++;
        }

        void Application::initSDL()
        {
            if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
            {
                throw std::runtime_error(SDL_GetError());
            }
        }

        void Application::buildWindow()
        {
            Window::init(m_window);
        }

        void Application::onWindowEvent(const SDL_WindowEvent& inEvent)
        {
            switch (inEvent.event)
            {
            case SDL_WINDOWEVENT_MINIMIZED:
                m_window.width   = 0;
                m_window.height  = 0;

                m_window.isMinimized = true;

                return;

            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_MAXIMIZED:
                m_window.isMinimized = false;

                rebuildSwapChain();

                return;
            
            default:
                return;
            }
        }

        void Application::onKeyboardInput(const SDL_KeyboardEvent& inEvent)
        {
            float cameraStepSize = 12.25;

            glm::vec3 nextPosition = m_camera->getPosition();

            switch (inEvent.keysym.scancode)
            {
            case SDL_SCANCODE_W:
                nextPosition.x += cameraStepSize;

                break;

            case SDL_SCANCODE_A:
                nextPosition.y += cameraStepSize;

                break;

            case SDL_SCANCODE_S:
                nextPosition.x -= cameraStepSize;   

                break;    

            case SDL_SCANCODE_D:
                nextPosition.y -= cameraStepSize;

                break;

            default:
                return;
            }

            m_camera->updatePosition(nextPosition);
        }

        void Application::buildInstance()
        {
            Instance::init(
                m_instance,
                m_dldi,
                m_window.instance
            );
        }

        void Application::destroyInstance()
        {
            m_instance.destroy();
        }

        void Application::buildDebugMessenger()
        {
            if (IS_DEBUGGING == false)
            {
                return;
            }

            Debug::initMessenger(
                m_debugMessenger,
                m_instance,
                m_dldi
            );
        }

        void Application::destroyDebugMessenger()
        {
            m_instance.destroyDebugUtilsMessengerEXT(
                m_debugMessenger,
                nullptr,
                m_dldi
            );
        }

        void Application::buildSurface()
        {
            Surface::init(
                m_surface,
                m_instance,
                m_window.instance
            );
        }

        void Application::destroySurface()
        {
            m_instance.destroySurfaceKHR(m_surface);
        }

        void Application::buildQueues()
        {
            Queue::initGraphicsQueue(
                m_graphicsQueue,
                m_physicalDevice,
                m_logicalDevice,
                m_surface
            );

            Queue::initPresentQueue(
                m_presentQueue,
                m_physicalDevice,
                m_logicalDevice,
                m_surface
            );
        }

        void Application::buildDevices()
        {
            Device::pickPhysicalDevice(
                m_physicalDevice,
                m_instance
            );
            Device::initLogicalDevice(
                m_logicalDevice,
                m_physicalDevice,
                m_surface
            );
        }
                    
        void Application::destroyDevices()
        {
            m_logicalDevice.destroy();
        }

        void Application::buildSwapChain()
        {
            SwapChain::init(
                m_swapChain,
                m_physicalDevice,
                m_logicalDevice,
                m_surface,
                m_window.width,
                m_window.height
            );

            m_camera->updateResolution(
                m_swapChain.extent.width,
                m_swapChain.extent.height
            );

            m_maxInFlightFramesCount = static_cast<int>(m_swapChain.frames.size());

            for (Frame::Instance& frame : m_swapChain.frames)
            {
                frame.physicalDevice = m_physicalDevice;
                frame.logicalDevice  = m_logicalDevice;
                frame.width          = m_swapChain.extent.width;
                frame.height         = m_swapChain.extent.height;

                frame.setupDepthBuffering();
            }
        }

        void Application::rebuildSwapChain()
        {
            if (m_window.isMinimized)
            {
                return;
            }

            SDL_Vulkan_GetDrawableSize(
                m_window.instance,
                &m_window.width,
                &m_window.height
            );

            if (m_window.width == 0 || m_window.height == 0)
            {
                return;
            }

            m_logicalDevice.waitIdle();

            destroySwapChain();
            buildSwapChain();

            buildFramebuffers();
            buildFrameResources();
            buildFramesCommandBuffers();
        }

        void Application::destroySwapChain()
        {
            for (Frame::Instance& frame : m_swapChain.frames)
            {
                frame.destroy();
            }

            m_logicalDevice.destroySwapchainKHR(m_swapChain.instance);

            for (auto [type, instance] : m_frameDescriptors)
            {
                m_logicalDevice.destroyDescriptorPool(
                    instance.pool
                );
            }
        }

        void Application::initializeDescriptors()
        {
            Descriptor::Bundle defaultDescriptor {};

            // Sky
            m_frameDescriptors.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SKY,
                    defaultDescriptor
                )
            );
            m_materialDescriptors.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SKY,
                    defaultDescriptor
                )
            );

            // Scene
            m_frameDescriptors.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SCENE,
                    defaultDescriptor
                )
            );
            m_materialDescriptors.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SCENE,
                    defaultDescriptor
                )
            );
        }

        void Application::buildSkyDescriptorSetLayouts()
        {
            // Frame
            Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 1;

            /// Camera
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Descriptor::initSetLayout(
                m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
                m_logicalDevice,
                frameLayoutBidings
            );

            // Material
            Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
            materialLayoutBidings.count = 1;

            /// Texture
            materialLayoutBidings.indices.push_back(0);
            materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            materialLayoutBidings.counts.push_back(1);
            materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(
                m_materialDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
                m_logicalDevice,
                materialLayoutBidings
            );
        }

        void Application::buildSceneDescriptorSetLayouts()
        {
            // Frame
            Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 2;

            /// Camera
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Model
            frameLayoutBidings.indices.push_back(1);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Descriptor::initSetLayout(
                m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                m_logicalDevice,
                frameLayoutBidings
            );

            // Material
            Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
            materialLayoutBidings.count = 1;

            /// Texture
            materialLayoutBidings.indices.push_back(0);
            materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            materialLayoutBidings.counts.push_back(1);
            materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(
                m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                m_logicalDevice,
                materialLayoutBidings
            );
        }

        void Application::buildDescriptorSetLayouts()
        {
            buildSkyDescriptorSetLayouts();
            buildSceneDescriptorSetLayouts();
        }

        void Application::buildSkyGraphicsPipeline()
        {
            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.canOverwrite         = false;
            createInfo.hasVertices          = false;
            createInfo.hasDepth             = false;
            createInfo.logicalDevice        = m_logicalDevice;
            createInfo.vertexShaderName     = "sky.vert.spv";
            createInfo.fragmentShaderName   = "sky.frag.spv";
            createInfo.swapChainExtent      = m_swapChain.extent;
            createInfo.swapChainImageFormat = m_swapChain.format;
            createInfo.descriptorSetLayouts = {
                m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
                m_materialDescriptors.at(GraphicsPipeline::Type::SKY).setLayout
            };

            m_graphicPipelines.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SKY,
                    std::make_unique<GraphicsPipeline::Instance>(createInfo)
                )
            );
        }

        void Application::buildSceneGraphicsPipeline()
        {
            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.canOverwrite          = true;
            createInfo.hasVertices           = true;
            createInfo.hasDepth              = true;
            createInfo.logicalDevice         = m_logicalDevice;
            createInfo.vertexShaderName      = "triangle.vert.spv";
            createInfo.fragmentShaderName    = "triangle.frag.spv";
            createInfo.bindingDescription    = Vertex::getBindingDescription();
            createInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
            createInfo.swapChainExtent       = m_swapChain.extent;
            createInfo.swapChainImageFormat  = m_swapChain.format;
            createInfo.depthFormat           = m_swapChain.frames[0].depthFormat;
            createInfo.descriptorSetLayouts  = {
                m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout
            };

            m_graphicPipelines.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SCENE,
                    std::make_unique<GraphicsPipeline::Instance>(createInfo)
                )
            );
        }

        void Application::buildGraphicsPipelines()
        {
            buildSkyGraphicsPipeline();
            buildSceneGraphicsPipeline();
        }

        void Application::destroyGraphicsPipelines()
        {
            for (auto& [type, instance] : m_graphicPipelines)
            {
                instance.reset();
            }
        }

        void Application::buildFramebuffers()
        {
            std::unordered_map<GraphicsPipeline::Type, vk::RenderPass> renderPasses;
            renderPasses.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SKY,
                    m_graphicPipelines.at(GraphicsPipeline::Type::SKY)->renderPass
                )
            );
            renderPasses.insert(
                std::make_pair(
                    GraphicsPipeline::Type::SCENE,
                    m_graphicPipelines.at(GraphicsPipeline::Type::SCENE)->renderPass
                )
            );

            Frame::Buffer::CreateInfo createInfo = {
                m_logicalDevice,
                renderPasses,
                m_swapChain.extent
            };

            Frame::Buffer::init(m_swapChain.frames, createInfo);
        }

        void Application::buildCommandPool()
        {
            CommandBuffer::Pool::init(
                m_mainCommandPool,
                m_logicalDevice,
                m_physicalDevice,
                m_surface
            );
        }

        void Application::destroyCommandPool()
        {
            m_logicalDevice.destroyCommandPool(m_mainCommandPool);
        }

        void Application::buildMainCommandBuffer()
        {
            CommandBuffer::Instance::CreateInfo createInfo = {
                m_logicalDevice,
                m_mainCommandPool
            };

            CommandBuffer::Instance::init(
                m_mainCommandBuffer,
                createInfo
            );
        }

        void Application::buildFramesCommandBuffers()
        {
            CommandBuffer::Instance::CreateInfo createInfo = {
                m_logicalDevice,
                m_mainCommandPool
            };

            Frame::Buffer::initCommand(
                m_swapChain.frames,
                createInfo
            );
        }

        void Application::prepareCamera(Frame::Instance& outFrame)
        {
            outFrame.cameraVectorUBO.instance = m_camera->getVectorUBO();
            memcpy(
                outFrame.cameraVectorUBO.writeLocation,
                &outFrame.cameraVectorUBO.instance,
                outFrame.cameraVectorUBO.allocationSize
            );

            outFrame.cameraMatrixUBO.instance = m_camera->getMatrixUBO();
            memcpy(
                outFrame.cameraMatrixUBO.writeLocation,
                &outFrame.cameraMatrixUBO.instance,
                outFrame.cameraMatrixUBO.allocationSize
            );
        }

        void Application::prepareActors(Frame::Instance& outFrame)
        {
            outFrame.updateModelTransforms(m_level.getActors());
            memcpy(
                outFrame.modelData.writeLocation,
                outFrame.modelData.transforms.data(),
                outFrame.modelData.allocationSize
            );
        }

        void Application::prepareFrame(Frame::Instance& outFrame)
        {
            prepareCamera(outFrame);
            prepareActors(outFrame);

            outFrame.updateDescriptorSets();
        }

        void Application::buildFrameResources()
        {
            // Sky
            Descriptor::PoolCreateInfo skyPoolCreateInfo;
            skyPoolCreateInfo.count = 2;
            skyPoolCreateInfo.size  = static_cast<uint32_t>(m_swapChain.frames.size());
            skyPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
            skyPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

            Descriptor::initPool(
                m_frameDescriptors.at(GraphicsPipeline::Type::SKY).pool,
                m_logicalDevice,
                skyPoolCreateInfo
            );

            // Scene
            Descriptor::PoolCreateInfo scenePoolCreateInfo;
            scenePoolCreateInfo.count = 2;
            scenePoolCreateInfo.size  = static_cast<uint32_t>(m_swapChain.frames.size());
            scenePoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
            scenePoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

            Descriptor::initPool(
                m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).pool,
                m_logicalDevice,
                scenePoolCreateInfo
            );

            for (Frame::Instance& frame : m_swapChain.frames)
            {
                // Sync
                frame.setupSync();

                // Data
                frame.setupCameraVectorUBO();
                frame.setupCameraMatrixUBO();
                frame.setupModelData(m_level.getActors());

                // Sets
                frame.addDescriptorSet(
                    GraphicsPipeline::Type::SKY,
                    m_frameDescriptors.at(GraphicsPipeline::Type::SKY).setLayout,
                    m_frameDescriptors.at(GraphicsPipeline::Type::SKY).pool
                );
                frame.addDescriptorSet(
                    GraphicsPipeline::Type::SCENE,
                    m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                    m_frameDescriptors.at(GraphicsPipeline::Type::SCENE).pool
                );
                frame.setupDescriptorSets();
            }
        }

        void Application::buildMaterialResources()
        {
            // Sky
            Descriptor::PoolCreateInfo skyPoolCreateInfo;
            skyPoolCreateInfo.count = 1;
            skyPoolCreateInfo.size  = 1;
            skyPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

            Descriptor::initPool(
                m_materialDescriptors.at(GraphicsPipeline::Type::SKY).pool,
                m_logicalDevice,
                skyPoolCreateInfo
            );

            // Scene
            Descriptor::PoolCreateInfo scenePoolCreateInfo;
            scenePoolCreateInfo.count = 1;
            scenePoolCreateInfo.size  = m_textureManager->getCount();
            scenePoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

            Descriptor::initPool(
                m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).pool,
                m_logicalDevice,
                scenePoolCreateInfo
            );
        }

        void Application::includeCubeMaps()
        {
            CubeMap::Data skyCubeMapData;
            skyCubeMapData.filepaths = {
                "SkyBox/Sunset/front.tga",
                "SkyBox/Sunset/back.tga",
                "SkyBox/Sunset/left.tga",
                "SkyBox/Sunset/right.tga",
                "SkyBox/Sunset/up.tga",
                "SkyBox/Sunset/down.tga"
            };

            m_cubeMapManager->add(
                "Sky",
                skyCubeMapData
            );
        }

        void Application::buildCubeMaps()
        {
            m_cubeMapManager->build(
                m_logicalDevice,
                m_physicalDevice,
                m_mainCommandBuffer,
                m_graphicsQueue,
                m_materialDescriptors.at(
                    GraphicsPipeline::Type::SKY
                ).setLayout,
                m_materialDescriptors.at(
                    GraphicsPipeline::Type::SKY
                ).pool
            );
        }

        void Application::includeModels()
        {
            m_modelManager->add(
                "floor",
                "floor.obj",
                Model::Vendor::Wavefront
            );
        }

        void Application::buildModels()
        {
            m_modelManager->build(
                m_meshVertexBuffer,
                m_meshIndexBuffer,
                m_logicalDevice,
                m_physicalDevice,
                m_graphicsQueue,
                m_mainCommandBuffer
            );
        }

        void Application::includeTextures()
        {
            Texture::Data grayTextureData;
            grayTextureData.filepath = "Base/gray.png";

            m_textureManager->add("gray", grayTextureData);

            Texture::Data gridTextureData;
            gridTextureData.filepath = "Base/grid.png";

            m_textureManager->add("grid", gridTextureData);

            Texture::Data uvTextureData;
            uvTextureData.filepath = "Base/uv.png";

            m_textureManager->add("uv", uvTextureData);

            Texture::Data missingTextureData;
            missingTextureData.filepath = "Base/missing.png";

            m_textureManager->add("missing", missingTextureData);
        }

        void Application::buildTextures()
        {
            m_textureManager->build(
                m_logicalDevice,
                m_physicalDevice,
                m_mainCommandBuffer,
                m_graphicsQueue,
                m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).setLayout,
                m_materialDescriptors.at(GraphicsPipeline::Type::SCENE).pool
            );
        }

        void Application::includeAssets()
        {
            includeCubeMaps();
            includeModels();
            includeTextures();
        }

        void Application::buildAssets()
        {
            buildCubeMaps();
            buildModels();
            buildTextures();
        }

        void Application::destroyAssets()
        {
            m_cubeMapManager.reset();
            m_modelManager.reset();
            m_textureManager.reset();
        }
    }
}