#include "Editor/View.hpp"

#include "Runtime/Core.hpp"

#include <windows.h>
#include <tchar.h>

namespace Chicane
{
    namespace Editor
    {
        EditorView::EditorView()
            : Grid::View("home"),
            m_activeMenu(Menu::None)
        {}

        void EditorView::show(
            const Vec2& inResolution,
            const Vec2& inPosition
        )
        {
            ImGuiWindowFlags viewFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoNav |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                                         ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

            ImGui::Begin(
                m_id.c_str(),
                nullptr,
                viewFlags
            );
                ImGui::SetWindowSize(
                    ImVec2(
                        static_cast<float>(inResolution.x),
                        static_cast<float>(inResolution.y)
                    )
                );
                ImGui::SetWindowPos(
                    ImVec2(
                        static_cast<float>(inPosition.x),
                        static_cast<float>(inPosition.y)
                    )
                );

                ImGui::BeginMenuBar();
                    if (ImGui::Button("Grid"))
                    {
                        onBoxButtonClick();
                    }
                ImGui::EndMenuBar();

                if (m_activeMenu == Menu::Grid)
                {
                    ImGuiWindow* currentWindow = ImGui::GetCurrentWindow();
 
                    float width  = Grid::calculateSizeFromViewportWidth(20.0f);
                    float height = Grid::calculateSizeFromViewportHeight(100.0f);

                    ImGui::SetNextWindowPos(
                        ImVec2(
                            currentWindow->InnerRect.Max.x - width,
                            currentWindow->InnerRect.Min.y
                        )
                    );
                    ImGui::SetNextWindowSize(
                        ImVec2(
                            width,
                            height - 19.0f
                        )
                    );
                    ImGui::Begin(
                        "Tool",
                        nullptr,
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoNav |
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoMove
                    );
                        if (ImGui::Button("Open File"))
                        {
                            std::vector<FileSystem::FileFormat> fileFormats;
                            fileFormats.push_back(
                                {
                                    "Texture",
                                    "png"
                                }
                            );
                            fileFormats.push_back(
                                {
                                    "Texture",
                                    "tga"
                                }
                            );
                            fileFormats.push_back(
                                {
                                    "OBJ File",
                                    "obj"
                                }
                            );

                            FileSystem::FileResult selectedFile = FileSystem::openFileDialog(
                                "Asset File",
                                fileFormats
                            );

                            LOG_INFO(selectedFile.path + " => " + selectedFile.extension);
                        }
                    ImGui::End();
                }

            ImGui::End();
        }

        void EditorView::onBoxButtonClick()
        {
            m_activeMenu = Menu::Grid;
        }
    }
}