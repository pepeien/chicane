#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Cursor.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Drift/Easing/Curve.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Alignment.hpp"
#include "Chicane/Grid/Style/Animation.hpp"
#include "Chicane/Grid/Style/Background.hpp"
#include "Chicane/Grid/Style/Corners.hpp"
#include "Chicane/Grid/Style/Display.hpp"
#include "Chicane/Grid/Style/Filter.hpp"
#include "Chicane/Grid/Style/Flex.hpp"
#include "Chicane/Grid/Style/Font.hpp"
#include "Chicane/Grid/Style/Import.hpp"
#include "Chicane/Grid/Style/Overflow.hpp"
#include "Chicane/Grid/Style/Position.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Radius.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"
#include "Chicane/Grid/Style/Transition.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Component;

        struct CHICANE_GRID Style
        {
        public:
            using Properties = std::unordered_map<String, std::vector<float>>;

        public:
            // Extension
            static constexpr inline const char* FILE_EXTENSION_NAME = "decal";

            // Attribute
            static constexpr inline const char* ATTRIBUTE_NAME = "style";

            // Reference
            static constexpr inline const char* REFERENCE_KEYWORD = "ref";

            // Import
            static constexpr inline const char* IMPORT_KEYWORD = "@import";
            static constexpr inline const char* URL_KEYWORD    = "url";

            // Ruleset
            static constexpr inline const char  COMMAND_ENDING   = ';';
            static constexpr inline const char  VARIABLE_KEYWORD = '$';
            static constexpr inline const char  RULESET_OPENING  = '{';
            static constexpr inline const char  RULESET_CLOSING  = '}';

            // Separator
            static constexpr inline const char  ONELINE_SEPARATOR  = ' ';
            static constexpr inline const char  SELECTOR_SEPARATOR = ',';
            static constexpr inline const char  CLASS_SEPARATOR    = ' ';

            // Selectors
            static constexpr inline const char  ID_SELECTOR              = '#';
            static constexpr inline const char  CLASS_SELECTOR           = '.';
            static constexpr inline const char  INCLUSIVE_SELECTOR       = '*';
            static constexpr inline const char  SELECTOR_INHERITANCE     = '&';
            static constexpr inline const char  SELECTOR_SEPARATOR_COMMA = ',';
            static constexpr inline const char  SELECTOR_SEPARATOR_SPACE = ' ';
            static constexpr inline const char  PSEUDO_CLASS_SELECTOR    = ':';
            static constexpr inline const char* PSEUDO_CLASS_HOVER       = ":hover";
            static constexpr inline const char* PSEUDO_CLASS_FOCUS       = ":focus";

            // Display
            static constexpr inline const char* DISPLAY_ATTRIBUTE_NAME = "display";
            static constexpr inline const char* DISPLAY_TYPE_FLEX      = "flex";
            static constexpr inline const char* DISPLAY_TYPE_BLOCK     = "block";
            static constexpr inline const char* DISPLAY_TYPE_HIDDEN    = "hidden";
            static constexpr inline const char* DISPLAY_TYPE_NONE      = "none";

            // Overflow
            static constexpr inline const char* OVERFLOW_ATTRIBUTE_NAME   = "overflow";
            static constexpr inline const char* OVERFLOW_X_ATTRIBUTE_NAME = "overflow-x";
            static constexpr inline const char* OVERFLOW_Y_ATTRIBUTE_NAME = "overflow-y";
            static constexpr inline const char* OVERFLOW_TYPE_VISIBLE     = "visible";
            static constexpr inline const char* OVERFLOW_TYPE_HIDDEN      = "hidden";
            static constexpr inline const char* OVERFLOW_TYPE_SCROLL      = "scroll";
            static constexpr inline const char* OVERFLOW_TYPE_AUTO        = "auto";

            // Z-Index
            static constexpr inline const char* Z_INDEX_ATTRIBUTE_NAME = "z-index";

            // Cursor
            static constexpr inline const char* CURSOR_ATTRIBUTE_NAME   = "cursor";
            static constexpr inline const char* CURSOR_TYPE_AUTO        = "auto";
            static constexpr inline const char* CURSOR_TYPE_DEFAULT     = "default";
            static constexpr inline const char* CURSOR_TYPE_POINTER     = "pointer";
            static constexpr inline const char* CURSOR_TYPE_TEXT        = "text";
            static constexpr inline const char* CURSOR_TYPE_CROSSHAIR   = "crosshair";
            static constexpr inline const char* CURSOR_TYPE_MOVE        = "move";
            static constexpr inline const char* CURSOR_TYPE_NOT_ALLOWED = "not-allowed";
            static constexpr inline const char* CURSOR_TYPE_WAIT        = "wait";
            static constexpr inline const char* CURSOR_TYPE_PROGRESS    = "progress";
            static constexpr inline const char* CURSOR_TYPE_GRAB        = "grab";
            static constexpr inline const char* CURSOR_TYPE_GRABBING    = "grabbing";
            static constexpr inline const char* CURSOR_TYPE_NS_RESIZE   = "ns-resize";
            static constexpr inline const char* CURSOR_TYPE_EW_RESIZE   = "ew-resize";
            static constexpr inline const char* CURSOR_TYPE_NESW_RESIZE = "nesw-resize";
            static constexpr inline const char* CURSOR_TYPE_NWSE_RESIZE = "nwse-resize";
            static constexpr inline const char* CURSOR_TYPE_NONE        = "none";

            // Flex
            static constexpr inline const char* FLEX_DIRECTION_ATTRIBUTE_NAME = "flex-direction";
            static constexpr inline const char* FLEX_DIRECTION_TYPE_ROW       = "row";
            static constexpr inline const char* FLEX_DIRECTION_TYPE_COLUMN    = "column";

            static constexpr inline const char* FLEX_WRAP_ATTRIBUTE_NAME = "flex-wrap";
            static constexpr inline const char* FLEX_WRAP_TYPE_NOWRAP    = "nowrap";
            static constexpr inline const char* FLEX_WRAP_TYPE_WRAP      = "wrap";

            // Size
            static constexpr inline const char* WIDTH_ATTRIBUTE_NAME  = "width";
            static constexpr inline const char* HEIGHT_ATTRIBUTE_NAME = "height";

            // Position
            static constexpr inline const char* POSITION_ATTRIBUTE_NAME = "position";
            static constexpr inline const char* POSITION_TYPE_ABSOLUTE  = "absolute";
            static constexpr inline const char* POSITION_TYPE_RELATIVE  = "relative";

            // Background
            static constexpr inline const char* BACKGROUND_COLOR_ATTRIBUTE_NAME = "background-color";
            static constexpr inline const char* BACKGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_TRANSPARENT;

            static constexpr inline const char* BACKGROUND_IMAGE_ATTRIBUTE_NAME = "background-image";
            static constexpr inline const char* BACKGROUND_IMAGE_DEFAULT_COLOR  = "";

            // Color
            static constexpr inline const char  HEX_KEYWORD  = '#';
            static constexpr inline const char* RGB_KEYWORD  = "rgb";
            static constexpr inline const char* RGBA_KEYWORD = "rgba";

            static constexpr inline const char* FOREGROUND_COLOR_ATTRIBUTE_NAME = "color";
            static constexpr inline const char* FOREGROUND_COLOR_DEFAULT_COLOR  = Color::HEX_COLOR_WHITE;

            static constexpr inline const char* OPACITY_ATTRIBUTE_NAME = "opacity";
            static constexpr inline const float OPACITY_DEFAULT_VALUE  = 1.0f;

            // Filter
            static constexpr inline const char* FILTER_ATTRIBUTE_NAME = "filter";
            static constexpr inline const char* FILTER_TYPE_NONE      = "none";
            static constexpr inline const char* FILTER_BLUR_KEYWORD   = "blur";

            static constexpr inline const char* BACKDROP_FILTER_ATTRIBUTE_NAME = "backdrop-filter";

            // Corner
            static constexpr inline const char* CORNER_DEFAULT_VALUE = "0px";

            // Text
            static constexpr inline const char* FONT_FAMILY_ATTRIBUTE_NAME = "font-family";
            static constexpr inline const char* FONT_FAMILY_DEFAULT_VALUE  = "Lexend";

            static constexpr inline const char* FONT_SIZE_ATTRIBUTE_NAME = "font-size";
            static constexpr inline const char* FONT_SIZE_DEFAULT_VALUE  = "1em";

            static constexpr inline const char* FONT_WEIGHT_ATTRIBUTE_NAME = "font-weight";
            static constexpr inline const char* FONT_WEIGHT_DEFAULT_VALUE  = "normal";
            static constexpr inline const char* FONT_WEIGHT_TYPE_NORMAL    = "normal";
            static constexpr inline const char* FONT_WEIGHT_TYPE_BOLD      = "bold";
            static constexpr inline const char* FONT_WEIGHT_TYPE_BOLDER    = "bolder";
            static constexpr inline const char* FONT_WEIGHT_TYPE_LIGHTER   = "lighter";
            static constexpr inline const float FONT_WEIGHT_NORMAL_VALUE   = 400.0f;
            static constexpr inline const float FONT_WEIGHT_BOLD_VALUE     = 700.0f;

            static constexpr inline const char* LETTER_SPACING_ATTRIBUTE_NAME = "letter-spacing";

            /*
             * Template 1: "`PROPERTY` `DURATION`"
             * Template 2: "`PROPERTY` `DURATION` `TIMING_FUNCTION`"
             * Template 3: "`PROPERTY` `DURATION` `TIMING_FUNCTION` `DELAY`"
             */
            static constexpr inline const char* TRANSITION_ATTRIBUTE_NAME          = "transition";
            static constexpr inline const char* TRANSITION_PROPERTY_ATTRIBUTE_NAME = "transition-property";
            static constexpr inline const char* TRANSITION_DURATION_ATTRIBUTE_NAME = "transition-duration";
            static constexpr inline const char* TRANSITION_TIMING_FUNCTION_ATTRIBUTE_NAME =
                "transition-timing-function";
            static constexpr inline const char* TRANSITION_DELAY_ATTRIBUTE_NAME = "transition-delay";
            static constexpr inline const char* TRANSITION_PROPERTY_ALL         = "all";

            // Animation
            static constexpr inline const char* ANIMATION_ATTRIBUTE_NAME                 = "animation";
            static constexpr inline const char* ANIMATION_NAME_ATTRIBUTE_NAME            = "animation-name";
            static constexpr inline const char* ANIMATION_DURATION_ATTRIBUTE_NAME        = "animation-duration";
            static constexpr inline const char* ANIMATION_TIMING_FUNCTION_ATTRIBUTE_NAME = "animation-timing-function";
            static constexpr inline const char* ANIMATION_DELAY_ATTRIBUTE_NAME           = "animation-delay";
            static constexpr inline const char* ANIMATION_ITERATION_COUNT_ATTRIBUTE_NAME = "animation-iteration-count";
            static constexpr inline const char* ANIMATION_DIRECTION_ATTRIBUTE_NAME       = "animation-direction";
            static constexpr inline const char* ANIMATION_FILL_MODE_ATTRIBUTE_NAME       = "animation-fill-mode";
            static constexpr inline const char* ANIMATION_PLAY_STATE_ATTRIBUTE_NAME      = "animation-play-state";
            static constexpr inline const char* ANIMATION_NAME_NONE                      = "none";
            static constexpr inline const char* ANIMATION_ITERATION_INFINITE             = "infinite";
            static constexpr inline const char* ANIMATION_DIRECTION_TYPE_NORMAL          = "normal";
            static constexpr inline const char* ANIMATION_DIRECTION_TYPE_REVERSE         = "reverse";
            static constexpr inline const char* ANIMATION_DIRECTION_TYPE_ALTERNATE       = "alternate";
            static constexpr inline const char* ANIMATION_DIRECTION_TYPE_ALTERNATE_REVERSE = "alternate-reverse";
            static constexpr inline const char* ANIMATION_FILL_TYPE_NONE                   = "none";
            static constexpr inline const char* ANIMATION_FILL_TYPE_FORWARDS               = "forwards";
            static constexpr inline const char* ANIMATION_FILL_TYPE_BACKWARDS              = "backwards";
            static constexpr inline const char* ANIMATION_FILL_TYPE_BOTH                   = "both";
            static constexpr inline const char* ANIMATION_PLAY_STATE_TYPE_RUNNING          = "running";
            static constexpr inline const char* ANIMATION_PLAY_STATE_TYPE_PAUSED           = "paused";

            // Easing
            static constexpr inline const char* EASING_TYPE_LINEAR          = Drift::EasingCurve::TYPE_LINEAR;
            static constexpr inline const char* EASING_TYPE_EASE            = Drift::EasingCurve::TYPE_EASE;
            static constexpr inline const char* EASING_TYPE_EASE_IN         = Drift::EasingCurve::TYPE_EASE_IN;
            static constexpr inline const char* EASING_TYPE_EASE_OUT        = Drift::EasingCurve::TYPE_EASE_OUT;
            static constexpr inline const char* EASING_TYPE_EASE_IN_OUT     = Drift::EasingCurve::TYPE_EASE_IN_OUT;
            static constexpr inline const char* EASING_CUBIC_BEZIER_KEYWORD = Drift::EasingCurve::CUBIC_BEZIER_KEYWORD;

            static inline std::vector<String>   EASING_TYPES = {
                Style::EASING_TYPE_LINEAR,
                Style::EASING_TYPE_EASE,
                Style::EASING_TYPE_EASE_IN,
                Style::EASING_TYPE_EASE_OUT,
                Style::EASING_TYPE_EASE_IN_OUT
            };

            // Keyframes
            static constexpr inline const char* KEYFRAMES_KEYWORD      = "@keyframes";
            static constexpr inline const char* KEYFRAMES_FROM_KEYWORD = "from";
            static constexpr inline const char* KEYFRAMES_TO_KEYWORD   = "to";

            /*
             * Template 1: "`SINGLE_MARGIN`"
             * Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
             * Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
             * Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
             */
            static constexpr inline const char* MARGIN_ATTRIBUTE_NAME        = "margin";
            static constexpr inline const char* MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
            static constexpr inline const char* MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
            static constexpr inline const char* MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
            static constexpr inline const char* MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

            /*
             * Template 1: "`SINGLE_PADDING`"
             * Template 2: "`VERTICAL_PADDING` `HORIZONTAL_PADDING`"
             * Template 3: "`TOP_PADDING` `BOTTOM_PADDING` `HORIZONTAL_PADDING`"
             * Template 4: "`TOP_PADDING` `RIGHT_PADDING` `BOTTOM_PADDING` `LEFT_PADDING`"
             */
            static constexpr inline const char* PADDING_ATTRIBUTE_NAME        = "padding";
            static constexpr inline const char* PADDING_TOP_ATTRIBUTE_NAME    = "padding-top";
            static constexpr inline const char* PADDING_BOTTOM_ATTRIBUTE_NAME = "padding-bottom";
            static constexpr inline const char* PADDING_LEFT_ATTRIBUTE_NAME   = "padding-left";
            static constexpr inline const char* PADDING_RIGHT_ATTRIBUTE_NAME  = "padding-right";

            /*
             * Template 1: "`SINGLE_GAP`"
             * Template 2: "`VERTICAL_GAP` `HORIZONTAL_GAP`"
             * Template 3: "`TOP_GAP` `BOTTOM_GAP` `HORIZONTAL_GAP`"
             * Template 4: "`TOP_GAP` `RIGHT_GAP` `BOTTOM_GAP` `LEFT_GAP`"
             */
            static constexpr inline const char* GAP_ATTRIBUTE_NAME        = "gap";
            static constexpr inline const char* GAP_TOP_ATTRIBUTE_NAME    = "gap-top";
            static constexpr inline const char* GAP_BOTTOM_ATTRIBUTE_NAME = "gap-bottom";
            static constexpr inline const char* GAP_LEFT_ATTRIBUTE_NAME   = "gap-left";
            static constexpr inline const char* GAP_RIGHT_ATTRIBUTE_NAME  = "gap-right";

            /*
             * Template 1: "`SINGLE_ALIGNMENT`"
             * Template 2: "`VERTICAL_ALIGNMENT` `HORIZONTAL_ALIGNMENT`"
             */
            static constexpr inline const char* ALIGNMENT_ATTRIBUTE_NAME = "alignment";
            static constexpr inline const char* ALIGNMENT_TYPE_START     = "start";
            static constexpr inline const char* ALIGNMENT_TYPE_CENTER    = "center";
            static constexpr inline const char* ALIGNMENT_TYPE_END       = "end";

            /*
             * Template 1: "`SINGLE_RADIUS`"
             * Template 2: "`TOP_LEFT_AND_BOTTOM_RIGHT` `TOP_RIGHT_AND_BOTTOM_LEFT`"
             * Template 3: "`TOP_LEFT` `TOP_RIGHT_AND_BOTTOM_LEFT` `BOTTOM_RIGHT`"
             * Template 4: "`TOP_LEFT` `TOP_RIGHT` `BOTTOM_RIGHT` `BOTTOM_LEFT`"
             * Elliptical: "`HORIZONTAL_RADII` / `VERTICAL_RADII`"
             */
            static constexpr inline const char  RADIUS_SEPARATOR                       = '/';
            static constexpr inline const char* BORDER_RADIUS_ATTRIBUTE_NAME           = "border-radius";
            static constexpr inline const char* BORDER_TOP_LEFT_RADIUS_ATTRIBUTE_NAME  = "border-top-left-radius";
            static constexpr inline const char* BORDER_TOP_RIGHT_RADIUS_ATTRIBUTE_NAME = "border-top-right-radius";
            static constexpr inline const char* BORDER_BOTTOM_RIGHT_RADIUS_ATTRIBUTE_NAME =
                "border-bottom-right-radius";
            static constexpr inline const char* BORDER_BOTTOM_LEFT_RADIUS_ATTRIBUTE_NAME = "border-bottom-left-radius";

            // Lists
            static inline const std::vector<String> ANIMATABLE_PROPERTIES = {
                OPACITY_ATTRIBUTE_NAME,          WIDTH_ATTRIBUTE_NAME,
                HEIGHT_ATTRIBUTE_NAME,           Z_INDEX_ATTRIBUTE_NAME,
                FOREGROUND_COLOR_ATTRIBUTE_NAME, BACKGROUND_COLOR_ATTRIBUTE_NAME,
                FONT_SIZE_ATTRIBUTE_NAME,        LETTER_SPACING_ATTRIBUTE_NAME,
                FILTER_ATTRIBUTE_NAME,           BACKDROP_FILTER_ATTRIBUTE_NAME,
                MARGIN_TOP_ATTRIBUTE_NAME,       MARGIN_BOTTOM_ATTRIBUTE_NAME,
                MARGIN_LEFT_ATTRIBUTE_NAME,      MARGIN_RIGHT_ATTRIBUTE_NAME,
                PADDING_TOP_ATTRIBUTE_NAME,      PADDING_BOTTOM_ATTRIBUTE_NAME,
                PADDING_LEFT_ATTRIBUTE_NAME,     PADDING_RIGHT_ATTRIBUTE_NAME,
                GAP_TOP_ATTRIBUTE_NAME,          GAP_BOTTOM_ATTRIBUTE_NAME,
                GAP_LEFT_ATTRIBUTE_NAME,         GAP_RIGHT_ATTRIBUTE_NAME
            };

        public:
            Style(const StyleRuleset::Properties& inProperties, Component* inParent);
            Style();

        public:
            bool isDisplay(StyleDisplay inValue) const;
            bool isPosition(StylePosition inValue) const;
            bool isPositioned() const;
            bool isClippingOverflow() const;

            void setProperties(const StyleRuleset::Properties& inProperties);

            void copyValuesFrom(const Style& inStyle);

            void snapshot();
            void restore();

            Properties extractAnimatedProperties() const;
            std::vector<float> extractAnimatedProperty(const String& inName) const;
            void applyAnimatedProperty(const String& inName, const std::vector<float>& inValue);
            const StyleTransition* findTransition(const String& inName) const;
            const Properties& getSnapshot() const;

            bool hasParent() const;
            void setParent(const Component* inComponent);

            void refresh();

        private:
            void refreshDisplay();
            void refreshFlex();
            void refreshZIndex();
            void refreshSize();
            void refreshPosition();
            void refreshAlignment();
            void refreshMargin();
            void refreshPadding();
            void refreshGap();
            void refreshOverflow();
            void refreshRadius();
            void refreshBackground();
            void refreshForegroundColor();
            void refreshOpacity();
            void refreshFilter();
            void refreshFont();
            void refreshLetterSpacing();
            void refreshCursor();

            void parseTransitions(const StyleRuleset::Properties& inProperties);
            void parseAnimation(const StyleRuleset::Properties& inProperties);

            StyleOverflow parseOverflow(const String& inValue) const;
            WindowCursor parseCursor(const String& inValue) const;
            Color::Rgba parseColor(const String& inValue) const;
            float parseFilter(const String& inValue) const;
            float parseSize(const String& inValue, SizeDirection inDirection, const Vec2* inBox = nullptr) const;
            float parseWeight(const String& inValue) const;
            String parseReference(const String& inValue) const;
            String parseText(const String& inValue) const;

        public:
            // Keywords
            StyleImport::List             imports;

            // Visiblity
            StyleProperty<StyleDisplay>   display;
            StyleProperty<float>          zIndex; // [0.0f, 999.9f]

            // Size
            StyleProperty<float>          width;
            StyleProperty<float>          height;

            // Flex
            StyleFlex                     flex;

            // Positioning
            StyleProperty<StylePosition>  position;
            StyleProperty<StyleAlignment> align;
            StyleCorners                  margin;
            StyleCorners                  padding;
            StyleCorners                  gap;
            StyleProperty<StyleOverflow>  overflowX;
            StyleProperty<StyleOverflow>  overflowY;
            StyleRadius                   radius;

            // Background
            StyleBackground               background;

            // Color
            StyleProperty<Color::Rgba>    foregroundColor;
            StyleProperty<float>          opacity;
            StyleFilter                   filter;
            StyleFilter                   backdrop;

            // Text
            StyleFont                     font;
            StyleProperty<float>          letterSpacing;

            // Pointer
            StyleProperty<WindowCursor>   cursor;

            // Motion
            StyleTransition::List         transitions;
            StyleAnimation                animation;

        private:
            const Component* m_parent;
            Properties       m_snapshot;
        };

        CHICANE_GRID std::vector<String> splitOneliner(const String& inValue);
    }
}