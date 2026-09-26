#pragma once

#include <atomic>
#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Serializable.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Transformable.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Scene;
    class Component;

    CH_TYPE(Manual)
    class CHICANE_RUNTIME Object : public Transformable, public Serializable
    {
        friend Scene;
        friend Component;

    public:
        // Attributes
        static constexpr inline const char* ID_ATTRIBUTE_NAME                   = "id";
        static constexpr inline const char* RELATIVE_TRANSLATION_ATTRIBUTE_NAME = "relativeTranslation";
        static constexpr inline const char* RELATIVE_ROTATION_ATTRIBUTE_NAME    = "relativeRotation";
        static constexpr inline const char* RELATIVE_SCALE_ATTRIBUTE_NAME       = "relativeScale";
        static constexpr inline const char* ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME = "absoluteTranslation";
        static constexpr inline const char* ABSOLUTE_ROTATION_ATTRIBUTE_NAME    = "absoluteRotation";
        static constexpr inline const char* ABSOLUTE_SCALE_ATTRIBUTE_NAME       = "absoluteScale";

    public:
        Object();
        virtual ~Object();

    protected:
        void onRefresh() override;
        void onAttributeChange(const String& inName, const String& inValue) override;

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
        bool applySerializedField(const String& inName, const String& inValue);

        const std::vector<Component*>& getAttachments() const;

    protected:
        void bindAttributes();

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
