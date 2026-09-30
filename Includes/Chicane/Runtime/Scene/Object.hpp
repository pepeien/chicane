#pragma once

#include <atomic>
#include <vector>

#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Serializable.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Transformable.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Scene;

    CH_TYPE(Manual)
    class CHICANE_RUNTIME Object : public Transformable, public Serializable
    {
        friend Scene;

    public:
        using SpatialTransform::setRelativeTranslation;

    public:
        static constexpr inline const char* ID_ATTRIBUTE_NAME                   = "id";
        static constexpr inline const char* RELATIVE_TRANSLATION_ATTRIBUTE_NAME = "relativeTranslation";
        static constexpr inline const char* RELATIVE_ROTATION_ATTRIBUTE_NAME    = "relativeRotation";
        static constexpr inline const char* RELATIVE_SCALE_ATTRIBUTE_NAME       = "relativeScale";
        static constexpr inline const char* ABSOLUTE_TRANSLATION_ATTRIBUTE_NAME = "absoluteTranslation";
        static constexpr inline const char* ABSOLUTE_ROTATION_ATTRIBUTE_NAME    = "absoluteRotation";
        static constexpr inline const char* ABSOLUTE_SCALE_ATTRIBUTE_NAME       = "absoluteScale";

        static constexpr inline float       DEFAULT_BOUNDS_SIZE = 1.0f;

    public:
        Object();
        virtual ~Object();

    protected:
        void beginRefresh() override;
        void endRefresh() override;
        void onRefresh() override;
        void onAttributeChange(const String& inName, const String& inValue) override;

    protected:
        inline virtual void onLoad() { return; }
        inline virtual void onUnload() { return; }
        inline virtual void onTick(float inDeltaTime) { return; }
        inline virtual void onPropertyEdited(const String& inName) { return; }
        inline virtual void onAttachment(Object* inParent) { return; }

    public:
        CH_FUNCTION()
        bool canTick() const;

        CH_FUNCTION()
        const String& getId() const;

        CH_FUNCTION()
        void setId(const String& inId);

        CH_FUNCTION()
        String getTypeName() const;

        CH_FUNCTION()
        bool isTransient() const;

        CH_FUNCTION()
        bool isAttached() const;

        CH_FUNCTION()
        bool isDescendantOf(const Object* inAncestor) const;

        CH_FUNCTION()
        Object* getParent() const;

        CH_FUNCTION()
        const std::vector<Object*>& getAttachments() const;

        CH_FUNCTION()
        void attachTo(Object* inParent);

        CH_FUNCTION()
        void detach();

        CH_FUNCTION()
        const Vec3& getTranslation() const;

        CH_FUNCTION()
        void setTranslation(const Vec3& inValue);

        CH_FUNCTION()
        void setRelativeTranslation(const Vec3& inValue);

        CH_FUNCTION()
        void lookAt(const Vec3& inTarget);

        CH_FUNCTION()
        const Vec3& getCenter() const;

        CH_FUNCTION()
        const Vec3& getTop() const;

        CH_FUNCTION()
        const Vec3& getBottom() const;

        CH_FUNCTION()
        const Vec3& getSize() const;

    public:
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        void setIsTransient(bool inValue);

        void notifyPropertyEdited(const String& inName);
        bool applySerializedField(const String& inName, const String& inValue);

    protected:
        void bindAttributes();
        void applyDefaultBounds();

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

        void addAttachment(Object* inObject);
        void removeAttachment(Object* inObject);
        bool isAncestorOf(const Object* inObject) const;

    protected:
        bool                 m_bCanTick;
        bool                 m_bCanCollide;
        bool                 m_bIsTransient;

        String               m_id;

        Object*              m_parent;
        EventSubscription<>  m_parentSubscription;
        std::vector<Object*> m_attachments;

    private:
        Scene*            m_scene;
        std::atomic<bool> m_bIsSpatialDirty;
    };
}
