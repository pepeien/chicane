#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Transformable/2D.hpp"

#include "Chicane/Drift/Animator.hpp"
#include "Chicane/Drift/Clip.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/Keyframe.hpp"
#include "Chicane/Grid/Style/Transition.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Animatable : public Transformable2D
        {
        public:
            using Properties = std::unordered_map<String, std::vector<float>>;

        public:
            Animatable();

        protected:
            inline virtual const StyleKeyframe::List* findKeyframes(const String& inName) const { return nullptr; }
            inline virtual Drift::Clip makeAnimationClip(const StyleKeyframe::List& inKeyframes) const { return {}; }

        protected:
            void snapshotAnimationVisual(const Style& inStyle);
            void restoreAnimationVisual(
                Style& outStyle, const std::unordered_map<String, std::vector<float>>& inVisual
            );
            void tickAnimation(Style& outStyle, float inDeltaTime);

            Properties extractAnimatedProperties(const Style& inStyle) const;
            std::vector<float> extractAnimatedProperty(const Style& inStyle, const String& inName) const;
            void applyAnimatedProperty(Style& outStyle, const String& inName, const std::vector<float>& inValue);
            const StyleTransition* findTransition(const Style& inStyle, const String& inName) const;

        protected:
            Drift::Animator m_animator;
            Properties      m_animationVisual;
            Properties      m_animationTargets;
            String          m_animationClip;
            float           m_animationDelta;
            bool            m_bIsAnimationReady;
        };
    }
}
