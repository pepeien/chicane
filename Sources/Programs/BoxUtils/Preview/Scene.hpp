#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Preview/Scene/Phase.hpp"

class PreviewScene : public Chicane::Scene
{
public:
    explicit PreviewScene(const std::vector<Chicane::FileSystem::Path>& inAssets);
    ~PreviewScene() override;

public:
    void onLoad() override;
    void onTick(float inDeltaTime) override;
    void onFrame();

private:
    static void sEnsurePresentable();
    void loadCurrent();

private:
    std::vector<Chicane::FileSystem::Path> m_assets;
    std::size_t                            m_index;
    std::atomic<std::uint32_t>             m_frames;
    std::atomic<PreviewScenePhase>         m_phase;
    bool                                   m_bCameraActive;
};
