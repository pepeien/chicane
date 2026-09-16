#pragma once

#include <atomic>
#include <vector>

#include "Chicane/Core/Transformable.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Scene;
    class Component;

    CH_TYPE(Manual)
    class CHICANE_RUNTIME Object : public Transformable
    {
        friend Scene;
        friend Component;

    public:
        Object();
        virtual ~Object();

    protected:
        void onRefresh() override;

    protected:
        inline virtual void onLoad() { return; }
        inline virtual void onUnload() { return; }
        inline virtual void onTick(float inDeltaTime) { return; }
        inline virtual void onPropertyEdited(const String& inName) { (void)inName; }

    public:
        CH_FUNCTION()
        bool canTick() const;

        CH_FUNCTION()
        const String& getId() const;

        CH_FUNCTION()
        String getTypeName() const;

        CH_FUNCTION()
        bool isTransient() const;

    public:
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        void setId(const String& inId);

        void setIsTransient(bool inValue);

        void notifyPropertyEdited(const String& inName);

        const std::vector<Component*>& getAttachments() const;

    protected:
        void applyLookAt(const String& inTarget);

    protected:
        template <typename T = Scene>
        T* getScene() const
        {
            if (!m_scene)
            {
                return nullptr;
            }

            return static_cast<T*>(m_scene);
        }

    private:
        void setScene(Scene* inScene);
        void markSpatialDirty();
        bool consumeSpatialDirty();
        void addAttachment(Component* inComponent);
        void removeAttachment(Component* inComponent);

    protected:
        bool                    m_bCanTick;
        bool                    m_bCanCollide;
        bool                    m_bIsTransient;

        String                  m_id;

        std::vector<Component*> m_attachments;

    private:
        Scene*            m_scene;
        std::atomic<bool> m_bIsSpatialDirty;
    };
}
