#include "Chicane/Grid/Loader.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, Component::Compiler> m_components = {
            { Button::TAG_ID,      [](const pugi::xml_node& inNode) { return new Button(inNode); } },
            { List::TAG_ID,        [](const pugi::xml_node& inNode) { return new List(inNode); } },
            { Container::TAG_ID,   [](const pugi::xml_node& inNode) { return new Container(inNode); } },
            { Popup::TAG_ID,       [](const pugi::xml_node& inNode) { return new Popup(inNode); } },
            { ProgressBar::TAG_ID, [](const pugi::xml_node& inNode) { return new ProgressBar(inNode); } },
            { Text::TAG_ID,        [](const pugi::xml_node& inNode) { return new Text(inNode); } },
            { TextInput::TAG_ID,   [](const pugi::xml_node& inNode) { return new TextInput(inNode); } }
        };

        std::unordered_map<std::string, View*> m_views          = { };
        View*                                  m_activeView     = nullptr;
        std::unique_ptr<Observable<View*>>     m_viewObservable = std::make_unique<Observable<View*>>();

        Component* createComponent(const pugi::xml_node& inNode)
        {
            const std::string& tag = inNode.name();

            if (m_components.find(tag) == m_components.end())
            {
                return nullptr;
            }

            return m_components.at(tag)(inNode);
        }

        void registerComponent(const std::string& inTag, Component::Compiler inCompiler)
        {
            if (m_components.find(inTag) != m_components.end())
            {
                return;
            }

            m_components.insert(
                std::make_pair(
                    inTag,
                    inCompiler
                )
            );
        }

        bool hasActiveView()
        {
            return m_activeView != nullptr;
        }

        View* getActiveView()
        {
            return m_activeView;
        }

        void activateView(const std::string& inPath)
        {
            if (!hasView(inPath))
            {
                Log::warning("The view [" + inPath + "] was not registered.");

                return;
            }

            m_activeView = getView(inPath);

            m_viewObservable->next(m_activeView);
        }

        Subscription<View*>* watchActiveView(
            std::function<void (View*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        )
        {
            Subscription<View*>* susbcription = m_viewObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            susbcription->next(m_activeView);

            return susbcription;
        }

        bool hasView(const std::string& inPath)
        {
            return m_views.find(inPath) != m_views.end();
        }

        View* getView(const std::string& inPath)
        {
            if (!hasView(inPath))
            {
                return nullptr;
            }

            return m_views.at(inPath);
        }

        void registerView(View* inView)
        {
            if (!inView)
            {
                Log::warning("The view is null and cannot be registered.");

                return;
            }

            if (hasView(inView->getPath()))
            {
                Log::warning("The view [" + inView->getPath() + "] was already registered.");

                return;
            }

            m_views.insert(
                std::make_pair(
                    inView->getPath(),
                    inView
                )
            );
        }
    }
}