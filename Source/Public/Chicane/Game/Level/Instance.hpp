#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/CameraComponent.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class Level
    {
    public:
        Level();
        virtual ~Level();

    public:
        virtual void onActivation() { return; }
        virtual void onTick(float inDeltaTime);

    public:
        void activate();

        bool hasActors() const;
        const std::vector<Actor*>& getActors() const;
        template<class T>
        std::vector<T*> getActors() const
        {
            std::vector<T*> result {};

            for (Actor* actor : getActors())
            {
                if (typeid(*actor) != typeid(T))
                {
                    continue;
                }

                result.push_back(static_cast<T*>(actor));
            }

            return result;
        }
        void addActor(Actor* inActor);
        Subscription<Actor*>* watchActors(
            std::function<void (Actor*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        bool hasComponents() const;
        const std::vector<Component*>& getComponents() const;
        template<class T>
        std::vector<T*> getComponents() const
        {
            std::vector<T*> result {};

            for (Component* component : getComponents())
            {
                if (typeid(*component) != typeid(T))
                {
                    continue;
                }

                result.push_back(static_cast<T*>(component));
            }

            return result;
        }
        void addComponent(Component* inComponent);
        void removeComponent(Component* inComponent);
        Subscription<const std::vector<Component*>&>* watchComponents(
            std::function<void (const std::vector<Component*>&)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        bool hasCamera();
        CameraComponent* getCamera();
        Subscription<CameraComponent*>* watchCamera(
            std::function<void (CameraComponent*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

    private:
        std::vector<Actor*>                                         m_actors;
        std::unique_ptr<Observable<Actor*>>                         m_actorObservable;

        std::vector<Component*>                                     m_components;
        std::unique_ptr<Observable<const std::vector<Component*>&>> m_componentObservable;

        std::unique_ptr<Observable<CameraComponent*>>               m_cameraObservable;
    };
}