#include "Runtime/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const std::string& inId)
            : m_id(inId)
        {}

        View::View(const std::string& inId, const std::string inSource)
            : m_id(inId),
            m_callbacks({})
        {
            if (inSource.empty())
            {
                return;
            }

            if (!m_document.load_file(inSource.c_str()))
            {
                throw std::runtime_error("Failed to read " + inSource);
            }

            if (m_document.empty() || m_document.children().empty())
            {
                throw std::runtime_error("UI document " + inSource + " does not have any components");
            }

            validate(m_document.first_child());
        }

        std::string View::getId()
        {
            return m_id;
        }

        void View::show(
            const Vec2& inResolution,
            const Vec2& inPosition
        )
        {
            compile(
                m_document.first_child(),
                inResolution,
                inPosition
            );
        }

        bool View::hasCallback(const std::string& inId)
        {
            return m_callbacks.find(inId) != m_callbacks.end();
        }

        void View::addCallback(const ComponentCallbackMap& inCallbacks)
        {
            for (auto [id, callback] : inCallbacks)
            {
                addCallback(id, callback);
            }
        }

        void View::addCallback(const std::string& inId, ComponentCallback inCallback)
        {
            if (hasCallback(inId))
            {
                return;
            }

            m_callbacks.insert(
                std::make_pair(
                    inId,
                    inCallback
                )
            );
        }

        void View::removeCallback(const std::string& inId)
        {
            if (!hasCallback(inId))
            {
                return;
            }

            m_callbacks.erase(inId);
        }

        void View::execCallback(const std::string& inId, pugi::xml_node& outNode)
        {
            if (!hasCallback(inId))
            {
                return;
            }

            m_callbacks.at(inId)(outNode);
        }

        void View::validate(const pugi::xml_node& inNode)
        {
            bool isRoot  = inNode.parent() == inNode.root();
            bool isAlone = isRoot && inNode.next_sibling() == nullptr;

            if (!isRoot || !isAlone)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (TAG_ID.compare(inNode.name()) != 0)
            {
                throw std::runtime_error("UI document root element must be a " + TAG_ID);
            }
        }

        void View::compile(
            pugi::xml_node& outNode,
            const Vec2& inResolution,
            const Vec2& inPosition
        )
        {
            ImGuiWindowFlags viewFlags = ImGuiWindowFlags_NoNav |
                                         ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoBackground;

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

                compileChildren(outNode);
            ImGui::End();
        }
    }
}