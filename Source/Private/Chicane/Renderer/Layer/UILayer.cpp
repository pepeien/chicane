#include "Chicane/Renderer/Layer/UILayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    UILayer::UILayer()
        : Layer("UI"),
        m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
        m_clearValues({})
    {
        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

        loadEvents();
    }

    UILayer::~UILayer()
    {
        m_internals.logicalDevice.waitIdle();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        m_internals.logicalDevice.destroyRenderPass(m_renderPass);
        m_internals.logicalDevice.destroyDescriptorPool(m_descriptorPool);
    }

    void UILayer::build()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        initDescriptorPool();
        initRenderpass();
        initFramebuffers();
        initImgui();

        setStatus(Status::Running);
    }

    void UILayer::destroy()
    {
        setStatus(Status::Idle);
    }

    void UILayer::rebuild()
    {
        if (!is(Status::Idle))
        {
            return;
        }

        initFramebuffers();

        Application::getView()->rebuild();

        setStatus(Status::Running);
    }

    void UILayer::onEvent(const SDL_Event& inEvent)
    {
        if (!is(Status::Running))
        {
            return;
        }

        ImGui_ImplSDL3_ProcessEvent(&inEvent);
    }

    void UILayer::setup(Vulkan::Frame::Instance& outFrame)
    {
        if (!is(Status::Running))
        {
            return;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        Window* window = Application::getWindow();

        Application::getView()->show(
            window->getDrawableSize(),
            window->getPosition()
        );

        ImGui::Render();

    	ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
    }

    void UILayer::render(
        Vulkan::Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!is(Status::Running))
        {
            return;
        }

        vk::RenderPassBeginInfo renderPassBeginInfo {};
        renderPassBeginInfo.renderPass          = m_renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<std::uint32_t>(m_clearValues.size());
        renderPassBeginInfo.pClearValues        = m_clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), inCommandBuffer);
        inCommandBuffer.endRenderPass();

        ImGui::EndFrame();
    }

    void UILayer::loadEvents()
    {
        if (!is(Status::Idle))
        {
            return;
        }

        Application::watchView(
            [this](Grid::View* inView)
            {
                if (!inView)
                {
                    return;
                }

                if (!is(Status::Idle))
                {
                    return;
                }

                setStatus(Status::Initialized);

                build();
            }
        );
    }

    void UILayer::initDescriptorPool()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::Descriptor::PoolCreateInfo createInfo;
        createInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        createInfo.size  = 1000;
        createInfo.types.push_back(vk::DescriptorType::eSampler);
        createInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        createInfo.types.push_back(vk::DescriptorType::eSampledImage);
        createInfo.types.push_back(vk::DescriptorType::eStorageImage);
        createInfo.types.push_back(vk::DescriptorType::eUniformTexelBuffer);
        createInfo.types.push_back(vk::DescriptorType::eStorageTexelBuffer);
        createInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        createInfo.types.push_back(vk::DescriptorType::eStorageBuffer);
        createInfo.types.push_back(vk::DescriptorType::eUniformBufferDynamic);
        createInfo.types.push_back(vk::DescriptorType::eStorageBufferDynamic);
        createInfo.types.push_back(vk::DescriptorType::eInputAttachment);

        Vulkan::Descriptor::initPool(
            m_descriptorPool,
            m_internals.logicalDevice,
            createInfo
        );
    }

    void UILayer::initRenderpass()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_internals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentDescription attachment = Vulkan::GraphicsPipeline::createColorAttachment(colorAttachment);

        m_renderPass = Vulkan::GraphicsPipeline::createRendepass(
            false,
            { attachment },
            m_internals.logicalDevice
        );
    }

    void UILayer::initFramebuffers()
    {
        if (is(Status::Running))
        {
            return;
        }

        Vulkan::Frame::Buffer::CreateInfo framebufferCreateInfo {};
        framebufferCreateInfo.id              = m_id;
        framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
        framebufferCreateInfo.renderPass      = m_renderPass;
        framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
        {
            framebufferCreateInfo.attachments.clear();
            framebufferCreateInfo.attachments.push_back(frame.imageView);

            Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void UILayer::initImgui()
    {
        if (!is(Status::Initialized))
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

        ImGui_ImplSDL3_InitForVulkan(Application::getWindow()->instance);

        Vulkan::Queue::FamilyIndices familyIndices;
        Vulkan::Queue::findFamilyInidices(
            familyIndices,
            m_internals.physicalDevice,
            m_internals.sufrace
        );

        ImGui_ImplVulkan_InitInfo imguiInitInfo {};
        imguiInitInfo.Instance        = m_internals.instance;
        imguiInitInfo.PhysicalDevice  = m_internals.physicalDevice;
        imguiInitInfo.Device          = m_internals.logicalDevice;
        imguiInitInfo.QueueFamily     = familyIndices.graphicsFamily.value();
        imguiInitInfo.Queue           = m_internals.graphicsQueue;
        imguiInitInfo.PipelineCache   = VK_NULL_HANDLE;
        imguiInitInfo.DescriptorPool  = m_descriptorPool;
        imguiInitInfo.Allocator       = nullptr;
        imguiInitInfo.MinImageCount   = m_internals.imageCount;
        imguiInitInfo.ImageCount      = m_internals.imageCount;
        imguiInitInfo.RenderPass      = m_renderPass;

        ImGui_ImplVulkan_Init(&imguiInitInfo);
    }
}