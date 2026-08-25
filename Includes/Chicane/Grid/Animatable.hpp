#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Transformable/2D.hpp"

#include "Chicane/Drift/Animator.hpp"
#include "Chicane/Drift/Clip.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/Keyframe.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Animatable : public Transformable2D
        {
        public:
            Animatable();

        protected:
            inline virtual bool canPlayAnimation() const { return true; }

            inline virtual const StyleKeyframe::List* findKeyframes(const String& inName) const { return nullptr; }

            inline virtual Drift::Clip makeAnimationClip(const StyleKeyframe::List& inKeyframes) const { return {}; }

        protected:
            void tickAnimation(Style& outStyle, float inDeltaTime);

        protected:
            Drift::Animator m_animator;
            String          m_animationClip;
            float           m_animationDelta;
            bool            m_bIsAnimationReady;
        };
    }
}
