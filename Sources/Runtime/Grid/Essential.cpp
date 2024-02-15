#include "Runtime/Grid/Essential.hpp"

#include "Runtime/Grid/Maps.hpp"
#include "Runtime/Game/State.hpp"
#include "Runtime/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, View*> m_views;
        View* m_activeView = nullptr;

        bool endsWith(const std::string& inTarget, const std::string& inEnding)
        {
            return std::string(inTarget.end() - inEnding.size(), inTarget.end()).compare(inEnding) == 0;
        }
        
        std::string getTag(const pugi::xml_node& inNode)
        {
            return inNode.name();
        }
        
        float getSizeFromPixel(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }
        
            std::string rawValue = inAttribute.as_string();
        
            if (!endsWith(rawValue, "px"))
            {
                return 0.0f;
            }
        
            return std::stof(std::string(rawValue.begin(), rawValue.end() - 2));
        }
        
        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }
        
            std::string rawValue = inAttribute.as_string();
        
            if (!endsWith(rawValue, "vh"))
            {
                return 0.0f;
            }
        
            float valueInVh = std::stof(std::string(rawValue.begin(), rawValue.end() - 2)) / 100;
        
            return State::getResolution().y * valueInVh;
        }
        
        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }
        
            std::string rawValue = inAttribute.as_string();
        
            if (!endsWith(rawValue, "vw"))
            {
                return 0.0f;
            }
        
            float valueInVw = std::stof(std::string(rawValue.begin(), rawValue.end() - 2)) / 100;
        
            return State::getResolution().x * valueInVw;
        }
        
        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        )
        {
            pugi::xml_attribute attributeValue = getAttribute(inAttributeName, inNode);
        
            if (attributeValue.empty())
            {
                return 0.0f;
            }
        
            std::string rawValue = attributeValue.as_string();
        
            if (endsWith(rawValue, "px"))
            {
                return getSizeFromPixel(attributeValue);
            }
        
            if (endsWith(rawValue, "vh"))
            {
                return getSizeFromViewportHeight(attributeValue);
            }
        
            if (endsWith(rawValue, "vw"))
            {
                return getSizeFromViewportWidth(attributeValue);
            }
        
            return 0.0f;
        }
        
        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        )
        {
            return inNode.attribute(inName.c_str());
        }

        void addView(std::vector<View*> inViews)
        {
            for (View* view : inViews)
            {
                addView(view);
            }
        }

        void addView(View* inView)
        {
            if (m_views.find(inView->getId()) != m_views.end())
            {
                return;
            }

            m_views.insert(
                std::make_pair(
                    inView->getId(),
                    inView
                )
            );
        }

        View* getActiveView()
        {
            return m_activeView;
        }
        
        void setActiveView(const std::string& inViewID)
        {
            m_activeView = m_views.at(inViewID);
        }
        
        void compileChildren(pugi::xml_node& outNode)
        {
            for (pugi::xml_node& child : outNode.children())
            {
                compileChild(child);
            }
        }
        
        void compileChild(pugi::xml_node& outNode)
        {
            if (outNode.name() == nullptr)
            {
                return;
            }

            std::string tagName = std::string(outNode.name());

            if (Components.find(tagName) == Components.end())
            {
                return;
            }

            std::string onTickSignature = getAttribute(ON_TICK_ATTRIBUTE, outNode).as_string();

            if (
                !onTickSignature.empty() &&
                m_activeView->hasCallback(onTickSignature)
            )
            {
                m_activeView->execCallback(onTickSignature, outNode);
            }

            Components.at(tagName)(outNode);
        }
    }
}