#include "Preview/Scene.hpp"

#include <iostream>
#include <stdexcept>

#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Core/Image.hpp>
#include <Chicane/Core/Math/Vec.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Renderer/Instance.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Preview/Service.hpp>

namespace
{
    static constexpr int           PREVIEW_SIZE    = Chicane::Box::AssetPreview::SIZE;
    static constexpr std::uint32_t WARMUP_FRAMES   = 16;
    static constexpr std::uint32_t CAPTURE_RETRIES = 48;
}

PreviewScene::PreviewScene(const std::vector<Chicane::FileSystem::Path>& inAssets)
    : Scene(),
      m_assets(inAssets),
      m_index(0),
      m_frames(0),
      m_phase(PreviewScenePhase::Load),
      m_bCameraActive(false)
{}

PreviewScene::~PreviewScene()
{
    Chicane::PreviewService::sInstance().clear(*this);
}

void PreviewScene::onLoad()
{
    Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
    preview.openStage(*this);
    preview.spawnCamera(*this);
}

void PreviewScene::onTick(float)
{
    if (m_phase.load(std::memory_order_acquire) != PreviewScenePhase::Load)
    {
        return;
    }

    if (m_index >= m_assets.size())
    {
        m_phase.store(PreviewScenePhase::Done, std::memory_order_release);

        return;
    }

    try
    {
        loadCurrent();
        m_frames.store(0, std::memory_order_relaxed);
        m_phase.store(PreviewScenePhase::Warmup, std::memory_order_release);
    }
    catch (const std::exception& exception)
    {
        const Chicane::FileSystem::Path& asset = m_assets.at(m_index);
        Chicane::PreviewService::sInstance().clear(*this);

        if (Chicane::Box::embedPreview(asset))
        {
            std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
        }
        else
        {
            std::cerr << "Failed to generate a preview for [" << asset.toString() << "]: " << exception.what()
                      << std::endl;
        }

        m_index++;
        m_phase.store(
            m_index >= m_assets.size() ? PreviewScenePhase::Done : PreviewScenePhase::Load,
            std::memory_order_release
        );
    }
}

void PreviewScene::onFrame()
{
    if (m_phase.load(std::memory_order_acquire) != PreviewScenePhase::Warmup)
    {
        if (m_phase.load(std::memory_order_acquire) == PreviewScenePhase::Done)
        {
            if (Chicane::Window* window = Chicane::Window::sGetCurrent())
            {
                window->close();
            }
        }

        return;
    }

    sEnsurePresentable();

    const std::uint32_t frames = m_frames.fetch_add(1, std::memory_order_relaxed) + 1;
    if (frames < WARMUP_FRAMES)
    {
        return;
    }

    Chicane::Renderer::Instance*    renderer = Chicane::Instance::sInstance().getRenderer();
    std::unique_ptr<Chicane::Image> image    = renderer ? renderer->captureScreen() : nullptr;
    if (!image && frames < WARMUP_FRAMES + CAPTURE_RETRIES)
    {
        return;
    }

    const Chicane::FileSystem::Path asset = m_assets.at(m_index);

    try
    {
        if (image && Chicane::Box::AssetPreview::sBake(asset, Chicane::Box::getTypeFromExtension(asset), *image))
        {
            std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
        }
        else if (Chicane::Box::embedPreview(asset))
        {
            std::cout << "Generated a preview for [" << asset.toString() << "]" << std::endl;
        }
        else if (!image)
        {
            std::cerr << "Failed to generate a preview for [" << asset.toString() << "]: screen capture unavailable"
                      << std::endl;
        }
        else
        {
            std::cerr << "Failed to generate a preview for [" << asset.toString() << "]: bake and CPU fallback failed"
                      << std::endl;
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Failed to generate a preview for [" << asset.toString() << "]: " << exception.what() << std::endl;
    }

    m_index++;
    m_phase.store(
        m_index >= m_assets.size() ? PreviewScenePhase::Done : PreviewScenePhase::Load,
        std::memory_order_release
    );
}

void PreviewScene::sEnsurePresentable()
{
    Chicane::Window* window = Chicane::Window::sGetCurrent();
    if (!window)
    {
        return;
    }

    if (window->isMinimized())
    {
        window->restore();
    }

    const Chicane::Vec<2, std::uint32_t> size = window->getSize();
    if (size.x == 0 || size.y == 0)
    {
        window->setSize(PREVIEW_SIZE, PREVIEW_SIZE);
    }
}

void PreviewScene::loadCurrent()
{
    Chicane::PreviewService& preview = Chicane::PreviewService::sInstance();
    if (!m_bCameraActive)
    {
        preview.activateCamera(*this);
        m_bCameraActive = true;
    }

    preview.show(*this, m_assets.at(m_index), true);
}
