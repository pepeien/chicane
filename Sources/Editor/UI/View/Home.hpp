#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Editor
    {
        class HomeView : public Grid::View
        {
        public:
            HomeView();

        private:
            Grid::Reference getFPS(const Grid::Component::Event& inEvent);
            Grid::Reference getFrametime(const Grid::Component::Event& inEvent);

            void showLog(const Log::Entry* inLog);
            void showActor(const Actor* inActor);
            void showDirectoryHistory(const std::string& inPath);
            void showDirectory(const FileSystem::Item& inList);

        private:
            void setupEvents();

            void setupUiTelemetry();
            void setupUiActor();
            void setupUiDirectory();
            void setupUiAssetCreator();
            void setupUiConsole();

            void updateOutliner();
            void updateDirHistory(std::string inPath);
            void updateDir(std::string inPath);

        private:
            // References
            std::vector<std::string>      m_rawDirectoryHistory;
            std::vector<Grid::Reference>  m_directoryHistory;
            std::vector<FileSystem::Item> m_rawDirectoryInfo;
            std::vector<Grid::Reference>  m_directoryInfo;

            std::vector<Grid::Reference>  m_actors;

            bool                          m_bIsConsoleOpen;
            std::vector<Grid::Reference>  m_consoleLogs;

            // UI
            Grid::Reference               m_uiDirectoryHistory;
            Grid::Reference               m_uiDirectoryInfo;

            Grid::Reference               m_uiActors;

            Grid::Reference               m_uiIsConsoleOpen;
            Grid::Reference               m_uiConsoleLogs;
        };
    }
}