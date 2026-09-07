#include "Chicane/Grid/Component/Svg.reflected.hpp"

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Contour.hpp"
#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Size/Direction.hpp"

#include "Chicane/Grid/Component/Svg/Paint.hpp"
#include "Chicane/Grid/Component/Svg/Scanner.hpp"
#include "Chicane/Grid/Component/Svg/Tessellation.hpp"
#include "Chicane/Grid/Component/Svg/ViewBox.hpp"

namespace Chicane
{
    namespace Grid
    {
        float degreesToRadians(float inDegrees)
        {
            return inDegrees * Svg::DEG_TO_RAD;
        }

        Vec2 reflectControl(const Vec2& inCurrent, const Vec2& inLast)
        {
            return (inCurrent * Svg::CONTROL_REFLECT) - inLast;
        }

        String tagName(const pugi::xml_node& inNode)
        {
            String            name  = inNode.name();
            const std::size_t split = name.lastOf(':');

            if (split != String::npos)
            {
                name = name.substr(split + 1);
            }

            name = name.toLower();

            if (name.startsWith(Svg::NAMESPACE_PREFIX) && name.size() > Svg::NAMESPACE_PREFIX_LENGTH)
            {
                name = name.substr(Svg::NAMESPACE_PREFIX_LENGTH);
            }

            if (name.equals(Svg::GROUP_ALIAS))
            {
                return Svg::GROUP_TAG;
            }

            return name;
        }

        String attribute(const pugi::xml_node& inNode, const char* inName)
        {
            const pugi::xml_attribute found = inNode.attribute(inName);

            if (!found.empty())
            {
                return found.as_string();
            }

            const String target = String(inName).toLower();

            for (pugi::xml_attribute attr : inNode.attributes())
            {
                if (!String(attr.name()).toLower().equals(target))
                {
                    continue;
                }

                return attr.as_string();
            }

            return "";
        }

        float parseNumber(const String& inValue, float inFallback = 0.0f)
        {
            if (inValue.isEmpty())
            {
                return inFallback;
            }

            return std::strtof(inValue.toChar(), nullptr);
        }

        Color::Rgba withOpacity(Color::Rgba inColor, float inOpacity)
        {
            inColor.a = static_cast<std::uint8_t>(
                std::clamp(static_cast<float>(inColor.a) * std::clamp(inOpacity, 0.0f, 1.0f), 0.0f, Svg::CHANNEL_MAX) +
                Svg::HALF
            );

            return inColor;
        }

        bool parsePaint(const String& inValue, const Color::Rgba inCurrent, Color::Rgba& outColor, bool& outEnabled)
        {
            const String value = inValue.trim();

            if (value.isEmpty() || value.equals(SvgPaint::PAINT_INHERIT))
            {
                return false;
            }

            if (value.equals(SvgPaint::PAINT_NONE, SvgPaint::PAINT_TRANSPARENT) ||
                value.startsWith(SvgPaint::PAINT_URL_PREFIX))
            {
                outEnabled = false;
                outColor   = Color::toRgba(Color::TEXT_COLOR_TRANSPARENT);

                return true;
            }

            outEnabled = true;
            outColor =
                value.equals(SvgPaint::PAINT_CURRENT, SvgPaint::PAINT_CURRENT_ALT) ? inCurrent : Color::toRgba(value);

            return true;
        }

        void parseStyle(const String& inStyle, const Color::Rgba& inCurrent, SvgPaint& outPaint)
        {
            for (const String& block : inStyle.split(';'))
            {
                const std::size_t split = block.firstOf(':');

                if (split == String::npos)
                {
                    continue;
                }

                const String key   = block.substr(0, split).trim().toLower();
                const String value = block.substr(split + 1).trim();

                if (key.equals(SvgPaint::FILL_ATTRIBUTE_NAME))
                {
                    parsePaint(value, inCurrent, outPaint.fill, outPaint.bIsFillEnabled);

                    continue;
                }

                if (key.equals(SvgPaint::STROKE_ATTRIBUTE_NAME))
                {
                    parsePaint(value, inCurrent, outPaint.stroke, outPaint.bIsStrokeEnabled);

                    continue;
                }

                if (key.equals(SvgPaint::STROKE_WIDTH_ATTRIBUTE_NAME))
                {
                    outPaint.strokeWidth = parseNumber(value, outPaint.strokeWidth);

                    continue;
                }

                if (key.equals(SvgPaint::STROKE_LINECAP_ATTRIBUTE_NAME))
                {
                    outPaint.lineCap = SvgPaint::parseLineCap(value);

                    continue;
                }

                if (key.equals(SvgPaint::STROKE_LINEJOIN_ATTRIBUTE_NAME))
                {
                    outPaint.lineJoin = SvgPaint::parseLineJoin(value);

                    continue;
                }

                if (key.equals(SvgPaint::OPACITY_ATTRIBUTE_NAME))
                {
                    outPaint.opacity = parseNumber(value, outPaint.opacity);

                    continue;
                }

                if (key.equals(SvgPaint::FILL_OPACITY_ATTRIBUTE_NAME))
                {
                    outPaint.fillOpacity = parseNumber(value, outPaint.fillOpacity);

                    continue;
                }

                if (key.equals(SvgPaint::STROKE_OPACITY_ATTRIBUTE_NAME))
                {
                    outPaint.strokeOpacity = parseNumber(value, outPaint.strokeOpacity);

                    continue;
                }

                if (key.equals(SvgPaint::FILL_RULE_ATTRIBUTE_NAME))
                {
                    outPaint.bIsEvenOdd = value.toLower().equals(SvgPaint::FILL_RULE_TYPE_EVENODD);
                }
            }
        }

        Mat3 parseTransform(const String& inValue)
        {
            Mat3       result(1.0f);
            SvgScanner scanner(inValue);

            while (!scanner.done())
            {
                scanner.skip();

                const char* start = scanner.p;

                while (scanner.p < scanner.end && std::isalpha(static_cast<unsigned char>(*scanner.p)))
                {
                    scanner.p++;
                }

                const String name = String(start, scanner.p).toLower();

                scanner.skip();

                if (scanner.p < scanner.end && *scanner.p == '(')
                {
                    scanner.p++;
                }

                std::vector<float> params;

                while (scanner.hasNumber())
                {
                    params.push_back(scanner.number());
                }

                scanner.skip();

                if (scanner.p < scanner.end && *scanner.p == ')')
                {
                    scanner.p++;
                }

                Mat3 local(1.0f);

                if (name.equals(Svg::TRANSFORM_MATRIX) && params.size() >= Svg::MATRIX_PARAM_COUNT)
                {
                    local[0] = glm::vec3(params[0], params[1], 0.0f);
                    local[1] = glm::vec3(params[2], params[3], 0.0f);
                    local[2] = glm::vec3(params[4], params[5], Svg::HOMOGENEOUS);
                }
                else if (name.equals(Svg::TRANSFORM_TRANSLATE) && !params.empty())
                {
                    const Vec2 offset(params[0], params.size() > 1 ? params[1] : 0.0f);
                    local[2] = glm::vec3(offset.x, offset.y, Svg::HOMOGENEOUS);
                }
                else if (name.equals(Svg::TRANSFORM_SCALE) && !params.empty())
                {
                    const Vec2 scale(params[0], params.size() > 1 ? params[1] : params[0]);
                    local[0][0] = scale.x;
                    local[1][1] = scale.y;
                }
                else if (name.equals(Svg::TRANSFORM_ROTATE) && !params.empty())
                {
                    const float angle = degreesToRadians(params[0]);
                    const float cosA  = std::cos(angle);
                    const float sinA  = std::sin(angle);
                    Mat3        rotate(1.0f);
                    rotate[0] = Vec3(cosA, sinA, 0.0f);
                    rotate[1] = Vec3(-sinA, cosA, 0.0f);

                    if (params.size() >= Svg::ROTATE_ORIGIN_PARAM_COUNT)
                    {
                        const Vec2 origin(params[1], params[2]);
                        Mat3       to(1.0f);
                        Mat3       from(1.0f);
                        to[2]   = Vec3(origin.x, origin.y, Svg::HOMOGENEOUS);
                        from[2] = Vec3(-origin.x, -origin.y, Svg::HOMOGENEOUS);
                        local   = to * rotate * from;
                    }
                    else
                    {
                        local = rotate;
                    }
                }
                else if (name.equals(Svg::TRANSFORM_SKEW_X) && !params.empty())
                {
                    local[1][0] = std::tan(degreesToRadians(params[0]));
                }
                else if (name.equals(Svg::TRANSFORM_SKEW_Y) && !params.empty())
                {
                    local[0][1] = std::tan(degreesToRadians(params[0]));
                }

                result = local * result;
            }

            return result;
        }

        SvgPaint applyNode(const pugi::xml_node& inNode, const SvgPaint& inParent, const Color::Rgba& inCurrent)
        {
            SvgPaint    paint            = inParent;
            const float inheritedOpacity = inParent.opacity;
            paint.opacity                = SvgPaint::OPACITY_DEFAULT_VALUE;

            parsePaint(attribute(inNode, SvgPaint::FILL_ATTRIBUTE_NAME), inCurrent, paint.fill, paint.bIsFillEnabled);
            parsePaint(
                attribute(inNode, SvgPaint::STROKE_ATTRIBUTE_NAME),
                inCurrent,
                paint.stroke,
                paint.bIsStrokeEnabled
            );

            const String strokeWidth = attribute(inNode, SvgPaint::STROKE_WIDTH_ATTRIBUTE_NAME);
            if (!strokeWidth.isEmpty())
            {
                paint.strokeWidth = parseNumber(strokeWidth, paint.strokeWidth);
            }

            const String strokeLinecap = attribute(inNode, SvgPaint::STROKE_LINECAP_ATTRIBUTE_NAME);
            if (!strokeLinecap.isEmpty())
            {
                paint.lineCap = SvgPaint::parseLineCap(strokeLinecap);
            }

            const String strokeLinejoin = attribute(inNode, SvgPaint::STROKE_LINEJOIN_ATTRIBUTE_NAME);
            if (!strokeLinejoin.isEmpty())
            {
                paint.lineJoin = SvgPaint::parseLineJoin(strokeLinejoin);
            }

            const String opacity = attribute(inNode, SvgPaint::OPACITY_ATTRIBUTE_NAME);
            if (!opacity.isEmpty())
            {
                paint.opacity = parseNumber(opacity, SvgPaint::OPACITY_DEFAULT_VALUE);
            }

            const String fillOpacity = attribute(inNode, SvgPaint::FILL_OPACITY_ATTRIBUTE_NAME);
            if (!fillOpacity.isEmpty())
            {
                paint.fillOpacity = parseNumber(fillOpacity, paint.fillOpacity);
            }

            const String strokeOpacity = attribute(inNode, SvgPaint::STROKE_OPACITY_ATTRIBUTE_NAME);
            if (!strokeOpacity.isEmpty())
            {
                paint.strokeOpacity = parseNumber(strokeOpacity, paint.strokeOpacity);
            }

            const String fillRule = attribute(inNode, SvgPaint::FILL_RULE_ATTRIBUTE_NAME).toLower();
            if (!fillRule.isEmpty())
            {
                paint.bIsEvenOdd = fillRule.equals(SvgPaint::FILL_RULE_TYPE_EVENODD);
            }

            const String transform = attribute(inNode, SvgPaint::TRANSFORM_ATTRIBUTE_NAME);
            if (!transform.isEmpty())
            {
                paint.transform = parseTransform(transform) * paint.transform;
            }

            parseStyle(attribute(inNode, SvgPaint::STYLE_ATTRIBUTE_NAME), inCurrent, paint);

            paint.opacity *= inheritedOpacity;

            return paint;
        }

        Vec2 transformPoint(const Mat3& inTransform, const Vec2& inPoint)
        {
            const glm::vec3 mapped =
                static_cast<glm::mat3>(inTransform) * glm::vec3(inPoint.x, inPoint.y, Svg::HOMOGENEOUS);

            return {mapped.x, mapped.y};
        }

        Vec2 toLocal(const Vec2& inPoint, const SvgViewBox& inView)
        {
            const float extent = std::max(inView.size.x, inView.size.y);

            if (extent <= 0.0f)
            {
                return Vec2::Zero();
            }

            const Vec2 local = ((inPoint - inView.origin) - (Svg::HALF * inView.size)) * (1.0f / extent);

            return {local.x, -local.y};
        }

        SvgViewBox parseViewBox(const String& inValue)
        {
            SvgViewBox result;
            SvgScanner scanner(inValue);

            if (scanner.hasNumber())
            {
                result.origin.x = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.origin.y = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.size.x = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.size.y = scanner.number();
            }

            return result;
        }

        Primitive toPrimitive(const Vertex::Positions& inPositions, const Vertex::Indices& inIndices)
        {
            Primitive primitive;
            primitive.indices = inIndices;

            for (const Vertex::Position& position : inPositions)
            {
                Vertex vertex;
                vertex.position = position;
                vertex.uv       = Vec2(position.x, position.y) + Svg::HALF;
                primitive.vertices.push_back(vertex);
            }

            return primitive;
        }

        void addArc(
            Curve&      outCurve,
            const Vec2& inFrom,
            const Vec2& inRadius,
            float       inXAngle,
            bool        inIsLarge,
            bool        inIsSweep,
            const Vec2& inTo
        )
        {
            if (std::fabs(inRadius.x) < Svg::MIN_LENGTH || std::fabs(inRadius.y) < Svg::MIN_LENGTH)
            {
                outCurve.addPoint(inTo);

                return;
            }

            Vec2 radius(std::fabs(inRadius.x), std::fabs(inRadius.y));

            const float xAngle = degreesToRadians(inXAngle);
            const float cosA   = std::cos(xAngle);
            const float sinA   = std::sin(xAngle);

            const Vec2 delta  = Svg::HALF * (inFrom - inTo);
            const Vec2 primed = Vec2((cosA * delta.x) + (sinA * delta.y), (-sinA * delta.x) + (cosA * delta.y));

            const float lambda =
                ((primed.x * primed.x) / (radius.x * radius.x)) + ((primed.y * primed.y) / (radius.y * radius.y));

            if (lambda > 1.0f)
            {
                radius *= std::sqrt(lambda);
            }

            const Vec2  radiusSq = radius * radius;
            const Vec2  primedSq = primed * primed;
            const float den      = (radiusSq.x * primedSq.y) + (radiusSq.y * primedSq.x);
            float       c        = 0.0f;
            if (den > 0.0f)
            {
                const float num = (radiusSq.x * radiusSq.y) - (radiusSq.x * primedSq.y) - (radiusSq.y * primedSq.x);
                c               = std::sqrt(std::max(0.0f, num / den));
            }

            if (inIsLarge == inIsSweep)
            {
                c = -c;
            }

            const Vec2 centerPrime(c * ((radius.x * primed.y) / radius.y), c * -((radius.y * primed.x) / radius.x));
            const Vec2 mid = Svg::HALF * (inFrom + inTo);
            const Vec2 center(
                (cosA * centerPrime.x) - (sinA * centerPrime.y) + mid.x,
                (sinA * centerPrime.x) + (cosA * centerPrime.y) + mid.y
            );

            auto vectorAngle = [](const Vec2& inU, const Vec2& inV)
            {
                const float norm = std::sqrt(((inU.x * inU.x) + (inU.y * inU.y)) * ((inV.x * inV.x) + (inV.y * inV.y)));
                const float value =
                    norm <= 0.0f ? 1.0f : std::clamp(((inU.x * inV.x) + (inU.y * inV.y)) / norm, -1.0f, 1.0f);
                float angle = std::acos(value);

                if (((inU.x * inV.y) - (inU.y * inV.x)) < 0.0f)
                {
                    angle = -angle;
                }

                return angle;
            };

            const Vec2 startDir((primed.x - centerPrime.x) / radius.x, (primed.y - centerPrime.y) / radius.y);
            const Vec2 endDir((-primed.x - centerPrime.x) / radius.x, (-primed.y - centerPrime.y) / radius.y);

            const float theta1 = vectorAngle(Vec2(1.0f, 0.0f), startDir);
            float       dTheta = vectorAngle(startDir, endDir);

            if (!inIsSweep && dTheta > 0.0f)
            {
                dTheta -= Svg::TWO_PI;
            }

            if (inIsSweep && dTheta < 0.0f)
            {
                dTheta += Svg::TWO_PI;
            }

            const int   segments = std::max(1, static_cast<int>(std::ceil(std::fabs(dTheta) / Svg::HALF_PI)));
            const float deltaT   = dTheta / static_cast<float>(segments);

            for (int i = 0; i < segments; i++)
            {
                const float t1   = theta1 + (deltaT * static_cast<float>(i));
                const float t2   = t1 + deltaT;
                const float half = (t2 - t1) * Svg::HALF;
                const float alpha =
                    (std::sin(t2 - t1) *
                     (std::sqrt(Svg::ARC_CUBIC_OFFSET + (Svg::ARC_CUBIC_SCALE * std::tan(half) * std::tan(half))) -
                      1.0f)) /
                    Svg::ARC_CUBIC_SCALE;

                const Vec2 p1 = {
                    center.x + (radius.x * std::cos(t1) * cosA) - (radius.y * std::sin(t1) * sinA),
                    center.y + (radius.x * std::cos(t1) * sinA) + (radius.y * std::sin(t1) * cosA)
                };
                const Vec2 p2 = {
                    center.x + (radius.x * std::cos(t2) * cosA) - (radius.y * std::sin(t2) * sinA),
                    center.y + (radius.x * std::cos(t2) * sinA) + (radius.y * std::sin(t2) * cosA)
                };
                const Vec2 d1 = {
                    (-radius.x * std::sin(t1) * cosA) - (radius.y * std::cos(t1) * sinA),
                    (-radius.x * std::sin(t1) * sinA) + (radius.y * std::cos(t1) * cosA)
                };
                const Vec2 d2 = {
                    (-radius.x * std::sin(t2) * cosA) - (radius.y * std::cos(t2) * sinA),
                    (-radius.x * std::sin(t2) * sinA) + (radius.y * std::cos(t2) * cosA)
                };

                if (i == 0 && outCurve.isEmpty())
                {
                    outCurve.addPoint(p1);
                }

                outCurve.addBezierPoint(p1 + (alpha * d1), p2 - (alpha * d2), p2);
            }
        }

        std::vector<Curve> parsePath(const String& inValue)
        {
            std::vector<Curve> contours;
            SvgScanner         scanner(inValue);

            Curve curve;
            curve.setSegmentCount(Svg::BEZIER_SEGMENTS);

            Vec2 current   = Vec2::Zero();
            Vec2 start     = Vec2::Zero();
            Vec2 lastCubic = Vec2::Zero();
            Vec2 lastQuad  = Vec2::Zero();
            char command   = 0;
            bool hasCubic  = false;
            bool hasQuad   = false;

            auto flush = [&]()
            {
                if (curve.getPoints().size() < Svg::MIN_CONTOUR_POINTS)
                {
                    curve = Curve();
                    curve.setSegmentCount(Svg::BEZIER_SEGMENTS);

                    return;
                }

                contours.push_back(curve);
                curve = Curve();
                curve.setSegmentCount(Svg::BEZIER_SEGMENTS);
            };

            while (!scanner.done())
            {
                const char next = scanner.command();

                if (next != 0)
                {
                    command = next;
                }

                if (command == 0)
                {
                    if (scanner.hasNumber())
                    {
                        scanner.number();

                        continue;
                    }

                    break;
                }

                const bool bIsRelative = std::islower(static_cast<unsigned char>(command));
                const char type        = static_cast<char>(std::toupper(static_cast<unsigned char>(command)));

                auto readPoint = [&](const Vec2& inOrigin) -> Vec2
                {
                    const float x = scanner.number();
                    const float y = scanner.number();
                    const Vec2  point(x, y);

                    return bIsRelative ? inOrigin + point : point;
                };

                if (type == 'M')
                {
                    flush();
                    current = readPoint(current);
                    start   = current;
                    curve.addPoint(current);
                    hasCubic = false;
                    hasQuad  = false;
                    command  = bIsRelative ? 'l' : 'L';

                    while (scanner.hasNumber())
                    {
                        current = readPoint(current);
                        curve.addPoint(current);
                    }

                    continue;
                }

                if (curve.isEmpty())
                {
                    curve.addPoint(current);
                }

                if (type == 'Z')
                {
                    curve.addPoint(start);
                    current  = start;
                    hasCubic = false;
                    hasQuad  = false;
                    flush();

                    continue;
                }

                if (type == 'L')
                {
                    while (scanner.hasNumber())
                    {
                        current = readPoint(current);
                        curve.addPoint(current);
                    }

                    hasCubic = false;
                    hasQuad  = false;

                    continue;
                }

                if (type == 'H')
                {
                    while (scanner.hasNumber())
                    {
                        const float x = scanner.number();
                        current.x     = bIsRelative ? current.x + x : x;
                        curve.addPoint(current);
                    }

                    hasCubic = false;
                    hasQuad  = false;

                    continue;
                }

                if (type == 'V')
                {
                    while (scanner.hasNumber())
                    {
                        const float y = scanner.number();
                        current.y     = bIsRelative ? current.y + y : y;
                        curve.addPoint(current);
                    }

                    hasCubic = false;
                    hasQuad  = false;

                    continue;
                }

                if (type == 'C')
                {
                    while (scanner.hasNumber())
                    {
                        const Vec2 controlA = readPoint(current);
                        const Vec2 controlB = readPoint(current);
                        const Vec2 point    = readPoint(current);
                        curve.addBezierPoint(controlA, controlB, point);
                        lastCubic = controlB;
                        current   = point;
                        hasCubic  = true;
                        hasQuad   = false;
                    }

                    continue;
                }

                if (type == 'S')
                {
                    while (scanner.hasNumber())
                    {
                        const Vec2 controlA = hasCubic ? reflectControl(current, lastCubic) : current;
                        const Vec2 controlB = readPoint(current);
                        const Vec2 point    = readPoint(current);
                        curve.addBezierPoint(controlA, controlB, point);
                        lastCubic = controlB;
                        current   = point;
                        hasCubic  = true;
                        hasQuad   = false;
                    }

                    continue;
                }

                if (type == 'Q')
                {
                    while (scanner.hasNumber())
                    {
                        const Vec2 control = readPoint(current);
                        const Vec2 point   = readPoint(current);
                        curve.addQuadraticPoint(control, point);
                        lastQuad = control;
                        current  = point;
                        hasQuad  = true;
                        hasCubic = false;
                    }

                    continue;
                }

                if (type == 'T')
                {
                    while (scanner.hasNumber())
                    {
                        const Vec2 control = hasQuad ? reflectControl(current, lastQuad) : current;
                        const Vec2 point   = readPoint(current);
                        curve.addQuadraticPoint(control, point);
                        lastQuad = control;
                        current  = point;
                        hasQuad  = true;
                        hasCubic = false;
                    }

                    continue;
                }

                if (type == 'A')
                {
                    while (scanner.hasNumber())
                    {
                        Vec2 radius;
                        radius.x             = scanner.number();
                        radius.y             = scanner.number();
                        const float angle    = scanner.number();
                        const bool  bIsLarge = scanner.flag();
                        const bool  bIsSweep = scanner.flag();
                        const Vec2  point    = readPoint(current);
                        addArc(curve, current, radius, angle, bIsLarge, bIsSweep, point);
                        current  = point;
                        hasCubic = false;
                        hasQuad  = false;
                    }
                }
            }

            flush();

            return contours;
        }

        std::vector<Vec2> parsePoints(const String& inValue)
        {
            std::vector<Vec2> points;
            SvgScanner        scanner(inValue);

            while (scanner.hasNumber())
            {
                const float x = scanner.number();

                if (!scanner.hasNumber())
                {
                    break;
                }

                points.push_back(Vec2(x, scanner.number()));
            }

            return points;
        }

        Curve makeEllipse(const Vec2& inCenter, const Vec2& inRadius)
        {
            const Vec2 kappa = Svg::KAPPA * inRadius;

            Curve curve;
            curve.setSegmentCount(Svg::BEZIER_SEGMENTS);
            curve.addPoint({inCenter.x + inRadius.x, inCenter.y});
            curve.addBezierPoint(
                {inCenter.x + inRadius.x, inCenter.y + kappa.y},
                {inCenter.x + kappa.x, inCenter.y + inRadius.y},
                {inCenter.x, inCenter.y + inRadius.y}
            );
            curve.addBezierPoint(
                {inCenter.x - kappa.x, inCenter.y + inRadius.y},
                {inCenter.x - inRadius.x, inCenter.y + kappa.y},
                {inCenter.x - inRadius.x, inCenter.y}
            );
            curve.addBezierPoint(
                {inCenter.x - inRadius.x, inCenter.y - kappa.y},
                {inCenter.x - kappa.x, inCenter.y - inRadius.y},
                {inCenter.x, inCenter.y - inRadius.y}
            );
            curve.addBezierPoint(
                {inCenter.x + kappa.x, inCenter.y - inRadius.y},
                {inCenter.x + inRadius.x, inCenter.y - kappa.y},
                {inCenter.x + inRadius.x, inCenter.y}
            );

            return curve;
        }

        Curve makeRect(const Vec2& inOrigin, const Vec2& inSize, const Vec2& inRadius)
        {
            Curve curve;
            curve.setSegmentCount(Svg::BEZIER_SEGMENTS);

            Vec2 radius(std::max(0.0f, inRadius.x), std::max(0.0f, inRadius.y));

            if (radius.x <= 0.0f && radius.y > 0.0f)
            {
                radius.x = radius.y;
            }

            if (radius.y <= 0.0f && radius.x > 0.0f)
            {
                radius.y = radius.x;
            }

            const Vec2 halfSize = Svg::HALF * inSize;
            radius.x            = std::min(radius.x, halfSize.x);
            radius.y            = std::min(radius.y, halfSize.y);

            const Vec2 end = inOrigin + inSize;

            if (radius.x <= 0.0f || radius.y <= 0.0f)
            {
                curve.addPoint(inOrigin);
                curve.addPoint({end.x, inOrigin.y});
                curve.addPoint(end);
                curve.addPoint({inOrigin.x, end.y});
                curve.addPoint(inOrigin);

                return curve;
            }

            const Vec2 kappa = Svg::KAPPA * radius;

            curve.addPoint({inOrigin.x + radius.x, inOrigin.y});
            curve.addPoint({end.x - radius.x, inOrigin.y});
            curve.addBezierPoint(
                {end.x - radius.x + kappa.x, inOrigin.y},
                {end.x, inOrigin.y + radius.y - kappa.y},
                {end.x, inOrigin.y + radius.y}
            );
            curve.addPoint({end.x, end.y - radius.y});
            curve.addBezierPoint(
                {end.x, end.y - radius.y + kappa.y},
                {end.x - radius.x + kappa.x, end.y},
                {end.x - radius.x, end.y}
            );
            curve.addPoint({inOrigin.x + radius.x, end.y});
            curve.addBezierPoint(
                {inOrigin.x + radius.x - kappa.x, end.y},
                {inOrigin.x, end.y - radius.y + kappa.y},
                {inOrigin.x, end.y - radius.y}
            );
            curve.addPoint({inOrigin.x, inOrigin.y + radius.y});
            curve.addBezierPoint(
                {inOrigin.x, inOrigin.y + radius.y - kappa.y},
                {inOrigin.x + radius.x - kappa.x, inOrigin.y},
                {inOrigin.x + radius.x, inOrigin.y}
            );

            return curve;
        }

        Curve::List toLocalContours(
            const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
        )
        {
            Curve::List result;

            for (const Curve& contour : inContours)
            {
                Curve local;

                for (const Vec2& point : contour.getPoints())
                {
                    local.addPoint(toLocal(transformPoint(inPaint.transform, point), inView));
                }

                if (local.getPoints().size() >= Svg::MIN_CONTOUR_POINTS)
                {
                    result.push_back(local);
                }
            }

            return result;
        }

        Primitive buildFill(const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView)
        {
            const Curve::List local = toLocalContours(inContours, inPaint, inView);

            Contour mesh;
            mesh.triangulate(local, inPaint.bIsEvenOdd);

            return toPrimitive(mesh.getPositions(), mesh.getIndices());
        }

        String makeGeometryKey(
            const char* inKind, const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
        )
        {
            String key = inKind;
            key.append('|');
            key.append(
                String::sprint(
                    "%.3f,%.3f,%.3f,%.3f|%.3f|%d|",
                    inView.origin.x,
                    inView.origin.y,
                    inView.size.x,
                    inView.size.y,
                    inPaint.strokeWidth,
                    inPaint.bIsEvenOdd ? 1 : 0
                )
            );

            for (const Curve& contour : inContours)
            {
                for (const Vec2& point : contour.getPoints())
                {
                    key.append(String::sprint("%.3f,%.3f;", point.x, point.y));
                }

                key.append('#');
            }

            return key;
        }

        const Primitive& cachedFill(
            const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
        )
        {
            static Primitive empty;

            const std::string key = makeGeometryKey("fill", inContours, inPaint, inView).toStandard();
            SvgTessellation&  tess = SvgTessellation::instance();
            if (const Primitive* hit = tess.find(key))
            {
                return *hit;
            }

            tess.request(
                key,
                [inContours, inPaint, inView]()
                {
                    return buildFill(inContours, inPaint, inView);
                }
            );

            return empty;
        }

        Primitive buildStroke(const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView)
        {
            Primitive primitive;

            if (inPaint.strokeWidth <= 0.0f)
            {
                return primitive;
            }

            const Vec2 scale(
                std::sqrt(
                    (inPaint.transform[0][0] * inPaint.transform[0][0]) +
                    (inPaint.transform[0][1] * inPaint.transform[0][1])
                ),
                std::sqrt(
                    (inPaint.transform[1][0] * inPaint.transform[1][0]) +
                    (inPaint.transform[1][1] * inPaint.transform[1][1])
                )
            );
            const float half = inPaint.strokeWidth * Svg::HALF * ((scale.x + scale.y) * Svg::HALF);

            if (half <= 0.0f)
            {
                return primitive;
            }

            auto vecLength = [](const Vec2& inValue) -> float
            { return std::sqrt((inValue.x * inValue.x) + (inValue.y * inValue.y)); };

            auto scaleVec = [](const Vec2& inValue, float inScale) -> Vec2
            { return Vec2(inValue.x * inScale, inValue.y * inScale); };

            auto push = [&](const Vec2& inPoint)
            {
                Vertex vertex;
                vertex.position.x = inPoint.x;
                vertex.position.y = inPoint.y;
                vertex.uv         = inPoint + Svg::HALF;
                primitive.vertices.push_back(vertex);
            };

            auto emitTriangle = [&](const Vec2& inA, const Vec2& inB, const Vec2& inC)
            {
                const std::uint32_t index = static_cast<std::uint32_t>(primitive.vertices.size());

                push(toLocal(inA, inView));
                push(toLocal(inB, inView));
                push(toLocal(inC, inView));

                primitive.indices.push_back(index);
                primitive.indices.push_back(index + 1);
                primitive.indices.push_back(index + 2);
            };

            auto emitQuad = [&](const Vec2& inA, const Vec2& inB, const Vec2& inC, const Vec2& inD)
            {
                emitTriangle(inA, inB, inC);
                emitTriangle(inA, inC, inD);
            };

            auto sideNormal = [&](const Vec2& inDelta) -> Vec2
            {
                const float length = vecLength(inDelta);

                if (length < Svg::MIN_LENGTH)
                {
                    return Vec2::Zero();
                }

                return Vec2(-inDelta.y / length, inDelta.x / length);
            };

            auto emitCap = [&](const Vec2& inCenter, const Vec2& inOutbound, const Vec2& inNormal)
            {
                if (inPaint.lineCap == SvgLineCap::Butt)
                {
                    return;
                }

                const Vec2 left  = inCenter + scaleVec(inNormal, half);
                const Vec2 right = inCenter - scaleVec(inNormal, half);

                if (inPaint.lineCap == SvgLineCap::Square)
                {
                    const Vec2 extend = scaleVec(inOutbound, half);

                    emitQuad(left, left + extend, right + extend, right);

                    return;
                }

                Vec2 prev = right;

                for (int i = 1; i <= SvgPaint::STROKE_ARC_SEGMENTS; i++)
                {
                    const float angle = Svg::PI * (static_cast<float>(i) / SvgPaint::STROKE_ARC_SEGMENTS);
                    const Vec2  curr  = inCenter + scaleVec(inNormal, -std::cos(angle) * half) +
                                      scaleVec(inOutbound, std::sin(angle) * half);

                    emitTriangle(inCenter, prev, curr);

                    prev = curr;
                }
            };

            auto unitVec = [&](const Vec2& inValue) -> Vec2
            {
                const float length = vecLength(inValue);

                if (length < Svg::MIN_LENGTH)
                {
                    return Vec2::Zero();
                }

                return Vec2(inValue.x / length, inValue.y / length);
            };

            auto emitJoin = [&](const Vec2& inCenter, const Vec2& inIncoming, const Vec2& inOutgoing)
            {
                if (inPaint.lineJoin == SvgLineJoin::Miter)
                {
                    return;
                }

                const Vec2  inDir  = unitVec(inIncoming);
                const Vec2  outDir = unitVec(inOutgoing);
                const float cross  = (inDir.x * outDir.y) - (inDir.y * outDir.x);

                if (std::fabs(cross) < SvgPaint::STROKE_JOIN_MIN_LENGTH)
                {
                    return;
                }

                const Vec2 inN         = sideNormal(inIncoming);
                const Vec2 outN        = sideNormal(inOutgoing);
                const bool bIsLeftTurn = cross > 0.0f;
                const Vec2 from        = bIsLeftTurn ? Vec2(-inN.x, -inN.y) : inN;
                const Vec2 to          = bIsLeftTurn ? Vec2(-outN.x, -outN.y) : outN;
                const Vec2 fromP       = inCenter + scaleVec(from, half);
                const Vec2 toP         = inCenter + scaleVec(to, half);

                if (inPaint.lineJoin == SvgLineJoin::Bevel)
                {
                    emitTriangle(inCenter, fromP, toP);

                    return;
                }

                float start = std::atan2(from.y, from.x);
                float delta = std::atan2(to.y, to.x) - start;

                while (delta > Svg::PI)
                {
                    delta -= Svg::TWO_PI;
                }

                while (delta < -Svg::PI)
                {
                    delta += Svg::TWO_PI;
                }

                const float stepSize = Svg::PI / static_cast<float>(SvgPaint::STROKE_ARC_SEGMENTS);
                const int   steps    = std::max(1, static_cast<int>(std::ceil(std::fabs(delta) / stepSize)));
                Vec2        prev     = fromP;

                for (int i = 1; i <= steps; i++)
                {
                    const float t    = static_cast<float>(i) / static_cast<float>(steps);
                    const float a    = start + (delta * t);
                    const Vec2  curr = inCenter + Vec2(std::cos(a) * half, std::sin(a) * half);

                    emitTriangle(inCenter, prev, curr);

                    prev = curr;
                }
            };

            for (const Curve& contour : inContours)
            {
                std::vector<Vec2> points;

                for (const Vec2& point : contour.getPoints())
                {
                    const Vec2 mapped = transformPoint(inPaint.transform, point);

                    if (!points.empty() && vecLength(mapped - points.back()) < Svg::MIN_LENGTH)
                    {
                        continue;
                    }

                    points.push_back(mapped);
                }

                bool bIsClosed = false;

                if (points.size() >= Svg::MIN_CLOSED_POINTS &&
                    vecLength(points.front() - points.back()) < Svg::MIN_LENGTH)
                {
                    points.pop_back();
                    bIsClosed = true;
                }

                if (points.size() < Svg::MIN_CONTOUR_POINTS)
                {
                    continue;
                }

                const std::size_t count = points.size();

                if (inPaint.lineJoin == SvgLineJoin::Miter)
                {
                    std::vector<Vec2> left(count);
                    std::vector<Vec2> right(count);

                    for (std::size_t i = 0; i < count; i++)
                    {
                        Vec2 normal;

                        if (!bIsClosed && i == 0)
                        {
                            normal = sideNormal(points.at(1) - points.at(0));
                        }
                        else if (!bIsClosed && i + 1 == count)
                        {
                            normal = sideNormal(points.at(i) - points.at(i - 1));
                        }
                        else
                        {
                            const Vec2  prev       = points.at((i + count - 1) % count);
                            const Vec2  next       = points.at((i + 1) % count);
                            const Vec2  inN        = sideNormal(points.at(i) - prev);
                            const Vec2  outN       = sideNormal(next - points.at(i));
                            Vec2        join       = inN + outN;
                            const float joinLength = vecLength(join);

                            if (joinLength < SvgPaint::STROKE_JOIN_MIN_LENGTH)
                            {
                                normal = inN;
                            }
                            else
                            {
                                join.x /= joinLength;
                                join.y /= joinLength;

                                const float cosine = std::clamp(
                                    join.x * inN.x + join.y * inN.y,
                                    SvgPaint::STROKE_MITER_COSINE_MIN,
                                    SvgPaint::STROKE_MITER_COSINE_MAX
                                );

                                if ((1.0f / cosine) > SvgPaint::STROKE_MITER_LIMIT)
                                {
                                    normal = inN;
                                }
                                else
                                {
                                    normal = scaleVec(join, 1.0f / cosine);
                                }
                            }
                        }

                        left.at(i)  = points.at(i) + scaleVec(normal, half);
                        right.at(i) = points.at(i) - scaleVec(normal, half);
                    }

                    for (std::size_t i = 1; i < count; i++)
                    {
                        emitQuad(left.at(i - 1), left.at(i), right.at(i), right.at(i - 1));
                    }

                    if (bIsClosed)
                    {
                        emitQuad(left.back(), left.front(), right.front(), right.back());
                    }
                }
                else
                {
                    auto emitSegment = [&](const Vec2& inStart, const Vec2& inEnd)
                    {
                        const Vec2 normal = scaleVec(sideNormal(inEnd - inStart), half);

                        emitQuad(inStart + normal, inEnd + normal, inEnd - normal, inStart - normal);
                    };

                    for (std::size_t i = 1; i < count; i++)
                    {
                        emitSegment(points.at(i - 1), points.at(i));
                    }

                    for (std::size_t i = 1; i + 1 < count; i++)
                    {
                        emitJoin(points.at(i), points.at(i) - points.at(i - 1), points.at(i + 1) - points.at(i));
                    }

                    if (bIsClosed)
                    {
                        emitSegment(points.back(), points.front());
                        emitJoin(points.front(), points.front() - points.back(), points.at(1) - points.front());
                        emitJoin(points.back(), points.back() - points.at(count - 2), points.front() - points.back());
                    }
                }

                if (bIsClosed)
                {
                    continue;
                }

                const Vec2  startDir = points.at(1) - points.front();
                const Vec2  endDir   = points.back() - points.at(count - 2);
                const float startLen = vecLength(startDir);
                const float endLen   = vecLength(endDir);
                const Vec2  startOut =
                    startLen >= Svg::MIN_LENGTH ? Vec2(-startDir.x / startLen, -startDir.y / startLen) : Vec2::Zero();
                const Vec2 endOut =
                    endLen >= Svg::MIN_LENGTH ? Vec2(endDir.x / endLen, endDir.y / endLen) : Vec2::Zero();

                emitCap(points.front(), startOut, sideNormal(startDir));
                emitCap(points.back(), endOut, sideNormal(endDir));
            }

            return primitive;
        }

        const Primitive& cachedStroke(
            const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
        )
        {
            static Primitive empty;

            const std::string key = makeGeometryKey("stroke", inContours, inPaint, inView).toStandard();
            SvgTessellation&  tess = SvgTessellation::instance();
            if (const Primitive* hit = tess.find(key))
            {
                return *hit;
            }

            tess.request(
                key,
                [inContours, inPaint, inView]()
                {
                    return buildStroke(inContours, inPaint, inView);
                }
            );

            return empty;
        }

        bool skipSubtree(const String& inTag)
        {
            return inTag.equals(
                "defs",
                "clippath",
                "mask",
                "lineargradient",
                "radialgradient",
                "style",
                "title",
                "desc",
                "symbol",
                "use",
                "filter",
                "marker"
            );
        }

        bool hidden(const pugi::xml_node& inNode)
        {
            const String display    = attribute(inNode, Svg::DISPLAY_ATTRIBUTE_NAME).toLower();
            const String visibility = attribute(inNode, Svg::VISIBILITY_ATTRIBUTE_NAME).toLower();

            return display.equals(Svg::DISPLAY_NONE) || visibility.equals(Svg::VISIBILITY_HIDDEN);
        }

        Svg::Svg(const pugi::xml_node& inNode)
            : Component(inNode),
              m_intrinsic(Vec2::Zero()),
              m_viewBox({}),
              m_signature(""),
              m_syncedSize(Vec2::Zero()),
              m_syncedPosition(Vec2::Zero()),
              m_syncedScale(UNSYNCED_SCALE),
              m_tessVersion(0),
              m_shapes({})
        {
            while (!m_children.empty())
            {
                removeChild(m_children.front());
            }
        }

        Svg::~Svg()
        {
            for (SvgShape* shape : m_shapes)
            {
                delete shape;
            }

            m_shapes.clear();
        }

        void Svg::refresh()
        {
            SvgTessellation& tess           = SvgTessellation::instance();
            bool             bNeedsTessSync = tess.pump();
            if (bNeedsTessSync || m_tessVersion != tess.generation())
            {
                m_tessVersion = tess.generation();
                m_signature = String::empty();
                bNeedsTessSync = true;
            }

            Component::refresh();

            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            const Vec2& size   = getSize();
            const Vec2& origin = getPosition();
            const Vec2  fit(
                m_viewBox.size.x > 0.0f ? size.x / m_viewBox.size.x : 0.0f,
                m_viewBox.size.y > 0.0f ? size.y / m_viewBox.size.y : 0.0f
            );
            const float scale = std::max(m_viewBox.size.x, m_viewBox.size.y) * std::min(fit.x, fit.y);

            if (!bNeedsTessSync && size == m_syncedSize && origin == m_syncedPosition && scale == m_syncedScale)
            {
                return;
            }

            m_syncedSize     = size;
            m_syncedPosition = origin;
            m_syncedScale    = scale;

            syncShapes();
        }

        void Svg::invalidateDrawCacheSubtree()
        {
            Component::invalidateDrawCacheSubtree();

            for (SvgShape* shape : m_shapes)
            {
                if (shape)
                {
                    shape->invalidateDrawCache();
                }
            }
        }

        std::vector<Component*> Svg::getChildrenFlat() const
        {
            std::vector<Component*> result = Component::getChildrenFlat();

            for (SvgShape* shape : m_shapes)
            {
                if (!shape)
                {
                    continue;
                }

                result.push_back(shape);
            }

            return result;
        }

        void Svg::onRefresh()
        {
            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            if (!m_bIsLaidOutThisFrame && !m_shapes.empty())
            {
                rebuildShapes();

                return;
            }

            applySizeAttributes();
            rebuildShapes();
        }

        void Svg::refreshPosition()
        {
            Component::refreshPosition();

            const Vec2& origin = getPosition();
            if (origin == m_syncedPosition && !m_shapes.empty())
            {
                return;
            }

            m_syncedPosition = origin;
            syncShapes();
        }

        void Svg::refreshSize()
        {
            const bool bIsWidthAuto  = m_style.width.isAuto();
            const bool bIsHeightAuto = m_style.height.isAuto();

            Component::refreshSize();

            if (m_intrinsic.x <= 0.0f && m_intrinsic.y <= 0.0f)
            {
                return;
            }

            Vec2 size = m_size;

            if (bIsWidthAuto && bIsHeightAuto)
            {
                size = m_intrinsic;
            }
            else if (bIsWidthAuto && m_intrinsic.y > 0.0f)
            {
                size.x = size.y * (m_intrinsic.x / m_intrinsic.y);
            }
            else if (bIsHeightAuto && m_intrinsic.x > 0.0f)
            {
                size.y = size.x * (m_intrinsic.y / m_intrinsic.x);
            }

            setSize(size);
        }

        void Svg::applySizeAttributes()
        {
            if (m_style.width.value.getRaw().isEmpty())
            {
                const String width = parseText(getAttribute(WIDTH_ATTRIBUTE_NAME)).trim();

                if (!width.isEmpty())
                {
                    m_style.width.value.setRaw(width);
                }
            }

            if (m_style.height.value.getRaw().isEmpty())
            {
                const String height = parseText(getAttribute(HEIGHT_ATTRIBUTE_NAME)).trim();

                if (!height.isEmpty())
                {
                    m_style.height.value.setRaw(height);
                }
            }
        }

        void Svg::rebuildShapes()
        {
            const Color::Rgba current   = m_style.foregroundColor.get();
            const String      signature = String::sprint(
                "%d,%d,%d,%d",
                static_cast<int>(current.r),
                static_cast<int>(current.g),
                static_cast<int>(current.b),
                static_cast<int>(current.a)
            );

            if (signature.equals(m_signature))
            {
                return;
            }

            m_signature = signature;

            SvgViewBox view;
            view.size.x = parseNumber(parseText(attribute(m_sourceNode, WIDTH_ATTRIBUTE_NAME)));
            view.size.y = parseNumber(parseText(attribute(m_sourceNode, HEIGHT_ATTRIBUTE_NAME)));

            const String viewBoxValue = parseText(attribute(m_sourceNode, VIEWBOX_ATTRIBUTE_NAME)).trim();

            if (!viewBoxValue.isEmpty())
            {
                view = parseViewBox(viewBoxValue);
            }

            if (view.size.x <= 0.0f)
            {
                view.size.x = SvgViewBox::DEFAULT_WIDTH;
            }

            if (view.size.y <= 0.0f)
            {
                view.size.y = SvgViewBox::DEFAULT_HEIGHT;
            }

            m_viewBox   = view;
            m_intrinsic = Vec2(
                parseNumber(parseText(attribute(m_sourceNode, WIDTH_ATTRIBUTE_NAME)), view.size.x),
                parseNumber(parseText(attribute(m_sourceNode, HEIGHT_ATTRIBUTE_NAME)), view.size.y)
            );

            const std::size_t shapeCount = m_shapes.size();
            std::size_t       index      = 0;
            SvgPaint          root       = applyNode(m_sourceNode, SvgPaint(current), current);

            std::function<void(const pugi::xml_node&, const SvgPaint&)> walk;
            walk = [&](const pugi::xml_node& inNode, const SvgPaint& inPaint)
            {
                for (pugi::xml_node child : inNode.children())
                {
                    if (child.type() != pugi::node_element || hidden(child))
                    {
                        continue;
                    }

                    const String   tag   = tagName(child);
                    const SvgPaint paint = applyNode(child, inPaint, current);

                    if (skipSubtree(tag))
                    {
                        continue;
                    }

                    if (tag.equals(GROUP_TAG, SVG_TAG, ANCHOR_TAG))
                    {
                        walk(child, paint);

                        continue;
                    }

                    std::vector<Curve> contours;

                    if (tag.equals(PATH_TAG))
                    {
                        contours = parsePath(parseText(attribute(child, D_ATTRIBUTE_NAME)));
                    }
                    else if (tag.equals(CIRCLE_TAG))
                    {
                        const float radius = parseNumber(parseText(attribute(child, R_ATTRIBUTE_NAME)));
                        contours.push_back(makeEllipse(
                            Vec2(
                                parseNumber(parseText(attribute(child, CX_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, CY_ATTRIBUTE_NAME)))
                            ),
                            Vec2(radius, radius)
                        ));
                    }
                    else if (tag.equals(ELLIPSE_TAG))
                    {
                        contours.push_back(makeEllipse(
                            Vec2(
                                parseNumber(parseText(attribute(child, CX_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, CY_ATTRIBUTE_NAME)))
                            ),
                            Vec2(
                                parseNumber(parseText(attribute(child, RX_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, RY_ATTRIBUTE_NAME)))
                            )
                        ));
                    }
                    else if (tag.equals(RECT_TAG))
                    {
                        contours.push_back(makeRect(
                            Vec2(
                                parseNumber(parseText(attribute(child, X_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, Y_ATTRIBUTE_NAME)))
                            ),
                            Vec2(
                                parseNumber(parseText(attribute(child, WIDTH_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, HEIGHT_ATTRIBUTE_NAME)))
                            ),
                            Vec2(
                                parseNumber(parseText(attribute(child, RX_ATTRIBUTE_NAME))),
                                parseNumber(parseText(attribute(child, RY_ATTRIBUTE_NAME)))
                            )
                        ));
                    }
                    else if (tag.equals(LINE_TAG))
                    {
                        Curve line;
                        line.addPoint(Vec2(
                            parseNumber(parseText(attribute(child, X1_ATTRIBUTE_NAME))),
                            parseNumber(parseText(attribute(child, Y1_ATTRIBUTE_NAME)))
                        ));
                        line.addPoint(Vec2(
                            parseNumber(parseText(attribute(child, X2_ATTRIBUTE_NAME))),
                            parseNumber(parseText(attribute(child, Y2_ATTRIBUTE_NAME)))
                        ));
                        contours.push_back(line);
                    }
                    else if (tag.equals(POLYLINE_TAG, POLYGON_TAG))
                    {
                        Curve                   poly;
                        const std::vector<Vec2> points =
                            parsePoints(parseText(attribute(child, POINTS_ATTRIBUTE_NAME)));

                        for (const Vec2& point : points)
                        {
                            poly.addPoint(point);
                        }

                        if (tag.equals(POLYGON_TAG) && !points.empty())
                        {
                            poly.addPoint(points.front());
                        }

                        contours.push_back(poly);
                    }
                    else
                    {
                        walk(child, paint);

                        continue;
                    }

                    if (paint.bIsFillEnabled &&
                        Color::isVisible(withOpacity(paint.fill, paint.fillOpacity * paint.opacity)))
                    {
                        const Primitive& primitive = cachedFill(contours, paint, view);

                        if (!primitive.isEmpty())
                        {
                            acquireShape(index)->configure(
                                primitive,
                                withOpacity(paint.fill, paint.fillOpacity * paint.opacity)
                            );
                            index++;
                        }
                    }

                    if (paint.bIsStrokeEnabled && paint.strokeWidth > 0.0f &&
                        Color::isVisible(withOpacity(paint.stroke, paint.strokeOpacity * paint.opacity)))
                    {
                        const Primitive& primitive = cachedStroke(contours, paint, view);

                        if (!primitive.isEmpty())
                        {
                            acquireShape(index)->configure(
                                primitive,
                                withOpacity(paint.stroke, paint.strokeOpacity * paint.opacity)
                            );
                            index++;
                        }
                    }
                }
            };

            walk(m_sourceNode, root);

            if (m_shapes.size() != shapeCount)
            {
                markFlatDirty();
            }

            for (std::size_t i = index; i < m_shapes.size(); i++)
            {
                m_shapes.at(i)->clear();
            }
        }

        void Svg::syncShapes()
        {
            const Vec2& size = getSize();
            const Vec2  fit(
                m_viewBox.size.x > 0.0f ? size.x / m_viewBox.size.x : 0.0f,
                m_viewBox.size.y > 0.0f ? size.y / m_viewBox.size.y : 0.0f
            );
            const float scale = std::max(m_viewBox.size.x, m_viewBox.size.y) * std::min(fit.x, fit.y);

            for (SvgShape* shape : m_shapes)
            {
                if (!shape || !shape->hasPrimitive())
                {
                    continue;
                }

                shape->sync(this, scale);
            }
        }

        SvgShape* Svg::acquireShape(std::size_t inIndex)
        {
            while (m_shapes.size() <= inIndex)
            {
                SvgShape* shape = new SvgShape();
                shape->setRoot(m_root);
                shape->setParent(this);
                shape->setStyleFile(m_styleFile);

                m_shapes.push_back(shape);
            }

            return m_shapes.at(inIndex);
        }
    }
}
