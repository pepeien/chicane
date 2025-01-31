#include "Chicane/Renderer/Vulkan/Layer/Grid.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LGrid::LGrid()
            : Layer::Instance("UI"),
            m_window(Application::getWindow()),
            m_view(nullptr),
            m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

            loadEvents();
        }

        LGrid::~LGrid()
        {
            if (is(Layer::Status::Offline))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();

            m_internals.logicalDevice.destroyRenderPass(m_renderPass);
            m_internals.logicalDevice.destroyDescriptorPool(m_descriptorPool);
        }

        void LGrid::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initDescriptorPool();
            initRenderpass();
            initFramebuffers();
            initImgui();

            setStatus(Layer::Status::Running);
        }

        void LGrid::destroy()
        {
            setStatus(Layer::Status::Idle);
        }

        void LGrid::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFramebuffers();

            m_view->rebuild();

            setStatus(Layer::Status::Running);
        }

        void LGrid::onEvent(const SDL_Event& inEvent)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            ImGui_ImplSDL3_ProcessEvent(&inEvent);
        }

        void LGrid::setup(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            m_view->show(
                m_window->getDrawableSize(),
                m_window->getPosition()
            );
        }

        void LGrid::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Renderer::Data* data             = (Renderer::Data*) outData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance& frame           = data->frame;

            vk::RenderPassBeginInfo beginInfo = {};
            beginInfo.renderPass          = m_renderPass;
            beginInfo.framebuffer         = frame.getFramebuffer(m_id);
            beginInfo.renderArea.offset.x = 0;
            beginInfo.renderArea.offset.y = 0;
            beginInfo.renderArea.extent   = data->swapChainExtent;
            beginInfo.clearValueCount     = static_cast<std::uint32_t>(m_clearValues.size());
            beginInfo.pClearValues        = m_clearValues.data();

            ImGui::Render();
        	ImGui::UpdatePlatformWindows();
	    	ImGui::RenderPlatformWindowsDefault();
    
            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
            commandBuffer.endRenderPass();

            ImGui::EndFrame();
        }

        void LGrid::loadEvents()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            setStatus(Layer::Status::Idle);

            Application::watchView(
                [&](Grid::View* inView)
                {
                    m_view = inView;

                    if (!inView)
                    {
                        return;
                    }

                    if (!is(Layer::Status::Idle))
                    {
                        return;
                    }

                    setStatus(Layer::Status::Initialized);

                    build();
                }
            );
        }

        void LGrid::initDescriptorPool()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Vulkan::Descriptor::PoolCreateInfo createInfo;
            createInfo.flags   = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
            createInfo.maxSets = 1000;
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eSampler, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eSampledImage, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eStorageImage, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eUniformTexelBuffer, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eStorageTexelBuffer, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eStorageBuffer, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eUniformBufferDynamic, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eStorageBufferDynamic, .descriptorCount = createInfo.maxSets }
            );
            createInfo.sizes.push_back(
                { .type = vk::DescriptorType::eInputAttachment, .descriptorCount = createInfo.maxSets }
            );

            Vulkan::Descriptor::initPool(
                m_descriptorPool,
                m_internals.logicalDevice,
                createInfo
            );
        }

        void LGrid::initRenderpass()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Vulkan::GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type          = GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            vk::AttachmentDescription attachment = Vulkan::GraphicsPipeline::createColorAttachment(colorAttachment);

            m_renderPass = Vulkan::GraphicsPipeline::createRendepass(
                { attachment },
                m_internals.logicalDevice,
                true,
                false
            );
        }

        void LGrid::initFramebuffers()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            Vulkan::Frame::Buffer::CreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_renderPass;
            framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                framebufferCreateInfo.attachments.clear();
                framebufferCreateInfo.attachments.push_back(frame.colorImage.view);

                Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
            }
        }

        void LGrid::setupImgui()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.IniFilename = nullptr;

            ImVec2 padding = ImVec2(0.0f, 0.0f);
            ImVec2 minSize = ImVec2(1.0f, 1.0f);

            ImGuiStyle& style = ImGui::GetStyle();
            style.DisplayWindowPadding   = padding;
            style.DisplaySafeAreaPadding = padding;

            style.ChildBorderSize = 0.0f;

            style.WindowPadding    = padding;
            style.WindowBorderSize = 0.0f;
            style.WindowRounding   = 0.0f;
            style.WindowMinSize    = minSize;

            style.FramePadding    = padding;
            style.FrameBorderSize = 0.0f;

            style.SeparatorTextPadding    = padding;
            style.SeparatorTextBorderSize = 0.0f;

            style.CellPadding = padding;
        
            style.ButtonTextAlign = padding;

            style.TabBorderSize = 0.0f;

            style.GrabMinSize = 0.0f;

            style.ItemInnerSpacing = padding;
            style.ItemSpacing      = padding;

            ImGui_ImplSDL3_InitForVulkan(m_window->instance);
        }

        void LGrid::initImgui()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            setupImgui();

            Vulkan::Queue::FamilyIndices familyIndices;
            Vulkan::Queue::findFamilyInidices(
                familyIndices,
                m_internals.physicalDevice,
                m_internals.sufrace
            );

            ImGui_ImplVulkan_InitInfo imguiInitInfo = {};
            imguiInitInfo.Instance       = m_internals.instance;
            imguiInitInfo.PhysicalDevice = m_internals.physicalDevice;
            imguiInitInfo.Device         = m_internals.logicalDevice;
            imguiInitInfo.QueueFamily    = familyIndices.graphicsFamily.value();
            imguiInitInfo.Queue          = m_internals.graphicsQueue;
            imguiInitInfo.PipelineCache  = VK_NULL_HANDLE;
            imguiInitInfo.DescriptorPool = m_descriptorPool;
            imguiInitInfo.MinImageCount  = m_internals.imageCount;
            imguiInitInfo.ImageCount     = m_internals.imageCount;
            imguiInitInfo.RenderPass     = m_renderPass;

            ImGui_ImplVulkan_Init(&imguiInitInfo);
        }
    }
}