#pragma once

#include "Chicane/Core/Transformable.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Scene;

    class CHICANE_RUNTIME Object : public Transformable
    {
        friend Scene;

    public:
        Object();

    protected:
        inline virtual void onLoad() { return; }
        inline virtual void onUnload() { return; }
        inline virtual void onTick(float inDeltaTime) { return; }

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        const String& getId() const;
        void setId(const String& inId);

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

    protected:
        bool   m_bCanTick;
        bool   m_bCanCollide;

        String m_id;

    private:
        Scene* m_scene;
    };
}