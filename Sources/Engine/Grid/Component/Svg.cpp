#include "Chicane/Grid/Component/Svg.reflected.hpp"

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <string>
#include <unordered_map>
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
#include "Chicane/Grid/Component/Svg/ViewBox.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr int   kBezierSegments = 8;
        constexpr float kMinLength      = 1.0e-5f;
        constexpr float kKappa          = 0.5522847498f;

        String tagName(const pugi::xml_node& inNode)
        {
            String            name  = inNode.name();
            const std::size_t split = name.lastOf(':');

            if (split != String::npos)
            {
                name = name.substr(split + 1);
            }

            name = name.toLower();

            if (name.startsWith("svg") && name.size() > 3)
            {
                name = name.substr(3);
            }

            if (name.equals("group"))
            {
                return "g";
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
                std::clamp(static_cast<float>(inColor.a) * std::clamp(inOpacity, 0.0f, 1.0f), 0.0f, 255.0f) + 0.5f
            );

            return inColor;
        }

        bool parsePaint(const String& inValue, const Color::Rgba inCurrent, Color::Rgba& outColor, bool& outEnabled)
        {
            const String value = inValue.trim();

            if (value.isEmpty() || value.equals("inherit"))
            {
                return false;
            }

            if (value.equals("none", "transparent") || value.startsWith("url("))
            {
                outEnabled = false;
                outColor   = Color::toRgba(Color::TEXT_COLOR_TRANSPARENT);

                return true;
            }

            outEnabled = true;
            outColor   = value.equals("currentColor", "currentcolor") ? inCurrent : Color::toRgba(value);

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

                if (name.equals("matrix") && params.size() >= 6)
                {
                    local[0] = glm::vec3(params[0], params[1], 0.0f);
                    local[1] = glm::vec3(params[2], params[3], 0.0f);
                    local[2] = glm::vec3(params[4], params[5], 1.0f);
                }
                else if (name.equals("translate") && !params.empty())
                {
                    local[2] = glm::vec3(params[0], params.size() > 1 ? params[1] : 0.0f, 1.0f);
                }
                else if (name.equals("scale") && !params.empty())
                {
                    const float sx = params[0];
                    const float sy = params.size() > 1 ? params[1] : sx;
                    local[0][0]    = sx;
                    local[1][1]    = sy;
                }
                else if (name.equals("rotate") && !params.empty())
                {
                    const float angle = params[0] * static_cast<float>(M_PI) / 180.0f;
                    const float cosA  = std::cos(angle);
                    const float sinA  = std::sin(angle);
                    Mat3        rotate(1.0f);
                    rotate[0] = Vec3(cosA, sinA, 0.0f);
                    rotate[1] = Vec3(-sinA, cosA, 0.0f);

                    if (params.size() >= 3)
                    {
                        Mat3 to(1.0f);
                        Mat3 from(1.0f);
                        to[2]   = Vec3(params[1], params[2], 1.0f);
                        from[2] = Vec3(-params[1], -params[2], 1.0f);
                        local   = to * rotate * from;
                    }
                    else
                    {
                        local = rotate;
                    }
                }
                else if (name.equals("skewx") && !params.empty())
                {
                    local[1][0] = std::tan(params[0] * static_cast<float>(M_PI) / 180.0f);
                }
                else if (name.equals("skewy") && !params.empty())
                {
                    local[0][1] = std::tan(params[0] * static_cast<float>(M_PI) / 180.0f);
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
            const glm::vec3 mapped = static_cast<glm::mat3>(inTransform) * glm::vec3(inPoint.x, inPoint.y, 1.0f);

            return {mapped.x, mapped.y};
        }

        Vec2 toLocal(const Vec2& inPoint, const SvgViewBox& inView)
        {
            const float extent = std::max(inView.width, inView.height);

            if (extent <= 0.0f)
            {
                return Vec2::Zero();
            }

            const float localX = ((inPoint.x - inView.x) - (inView.width * 0.5f)) / extent;
            const float localY = ((inPoint.y - inView.y) - (inView.height * 0.5f)) / extent;

            return {localX, -localY};
        }

        SvgViewBox parseViewBox(const String& inValue)
        {
            SvgViewBox result;
            SvgScanner scanner(inValue);

            if (scanner.hasNumber())
            {
                result.x = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.y = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.width = scanner.number();
            }
            if (scanner.hasNumber())
            {
                result.height = scanner.number();
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
                vertex.uv.x     = position.x + 0.5f;
                vertex.uv.y     = position.y + 0.5f;
                primitive.vertices.push_back(vertex);
            }

            return primitive;
        }

        void addArc(
            Curve&      outCurve,
            const Vec2& inFrom,
            float       inRx,
            float       inRy,
            float       inXAngle,
            bool        inLarge,
            bool        inSweep,
            const Vec2& inTo
        )
        {
            if (std::fabs(inRx) < kMinLength || std::fabs(inRy) < kMinLength)
            {
                outCurve.addPoint(inTo);

                return;
            }

            float rx = std::fabs(inRx);
            float ry = std::fabs(inRy);

            const float xAngle = inXAngle * static_cast<float>(M_PI) / 180.0f;
            const float cosA   = std::cos(xAngle);
            const float sinA   = std::sin(xAngle);

            const float dx  = (inFrom.x - inTo.x) * 0.5f;
            const float dy  = (inFrom.y - inTo.y) * 0.5f;
            const float x1p = (cosA * dx) + (sinA * dy);
            const float y1p = (-sinA * dx) + (cosA * dy);

            const float lambda = ((x1p * x1p) / (rx * rx)) + ((y1p * y1p) / (ry * ry));

            if (lambda > 1.0f)
            {
                const float scale = std::sqrt(lambda);
                rx *= scale;
                ry *= scale;
            }

            const float rx2  = rx * rx;
            const float ry2  = ry * ry;
            const float x1p2 = x1p * x1p;
            const float y1p2 = y1p * y1p;
            const float den  = (rx2 * y1p2) + (ry2 * x1p2);
            float c = den <= 0.0f ? 0.0f : std::sqrt(std::max(0.0f, ((rx2 * ry2) - (rx2 * y1p2) - (ry2 * x1p2)) / den));

            if (inLarge == inSweep)
            {
                c = -c;
            }

            const float cxp = c * ((rx * y1p) / ry);
            const float cyp = c * -((ry * x1p) / rx);
            const float cx  = (cosA * cxp) - (sinA * cyp) + ((inFrom.x + inTo.x) * 0.5f);
            const float cy  = (sinA * cxp) + (cosA * cyp) + ((inFrom.y + inTo.y) * 0.5f);

            auto vectorAngle = [](float inUx, float inUy, float inVx, float inVy)
            {
                const float norm  = std::sqrt(((inUx * inUx) + (inUy * inUy)) * ((inVx * inVx) + (inVy * inVy)));
                const float value = norm <= 0.0f ? 1.0f : std::clamp((inUx * inVx + inUy * inVy) / norm, -1.0f, 1.0f);
                float       angle = std::acos(value);

                if ((inUx * inVy - inUy * inVx) < 0.0f)
                {
                    angle = -angle;
                }

                return angle;
            };

            const float theta1 = vectorAngle(1.0f, 0.0f, (x1p - cxp) / rx, (y1p - cyp) / ry);
            float       dTheta = vectorAngle((x1p - cxp) / rx, (y1p - cyp) / ry, (-x1p - cxp) / rx, (-y1p - cyp) / ry);

            if (!inSweep && dTheta > 0.0f)
            {
                dTheta -= 2.0f * static_cast<float>(M_PI);
            }

            if (inSweep && dTheta < 0.0f)
            {
                dTheta += 2.0f * static_cast<float>(M_PI);
            }

            const int segments =
                std::max(1, static_cast<int>(std::ceil(std::fabs(dTheta) / (static_cast<float>(M_PI) * 0.5f))));
            const float delta = dTheta / static_cast<float>(segments);

            for (int i = 0; i < segments; i++)
            {
                const float t1   = theta1 + (delta * static_cast<float>(i));
                const float t2   = t1 + delta;
                const float half = (t2 - t1) * 0.5f;
                const float alpha =
                    (std::sin(t2 - t1) * (std::sqrt(4.0f + (3.0f * std::tan(half) * std::tan(half))) - 1.0f)) / 3.0f;

                const Vec2 p1 = {
                    cx + (rx * std::cos(t1) * cosA) - (ry * std::sin(t1) * sinA),
                    cy + (rx * std::cos(t1) * sinA) + (ry * std::sin(t1) * cosA)
                };
                const Vec2 p2 = {
                    cx + (rx * std::cos(t2) * cosA) - (ry * std::sin(t2) * sinA),
                    cy + (rx * std::cos(t2) * sinA) + (ry * std::sin(t2) * cosA)
                };
                const Vec2 d1 = {
                    (-rx * std::sin(t1) * cosA) - (ry * std::cos(t1) * sinA),
                    (-rx * std::sin(t1) * sinA) + (ry * std::cos(t1) * cosA)
                };
                const Vec2 d2 = {
                    (-rx * std::sin(t2) * cosA) - (ry * std::cos(t2) * sinA),
                    (-rx * std::sin(t2) * sinA) + (ry * std::cos(t2) * cosA)
                };

                if (i == 0 && outCurve.isEmpty())
                {
                    outCurve.addPoint(p1);
                }

                outCurve.addBezierPoint(
                    {p1.x + (alpha * d1.x), p1.y + (alpha * d1.y)},
                    {p2.x - (alpha * d2.x), p2.y - (alpha * d2.y)},
                    p2
                );
            }
        }

        std::vector<Curve> parsePath(const String& inValue)
        {
            std::vector<Curve> contours;
            SvgScanner         scanner(inValue);

            Curve curve;
            curve.setSegmentCount(kBezierSegments);

            Vec2 current   = Vec2::Zero();
            Vec2 start     = Vec2::Zero();
            Vec2 lastCubic = Vec2::Zero();
            Vec2 lastQuad  = Vec2::Zero();
            char command   = 0;
            bool hasCubic  = false;
            bool hasQuad   = false;

            auto flush = [&]()
            {
                if (curve.getPoints().size() < 2)
                {
                    curve = Curve();
                    curve.setSegmentCount(kBezierSegments);

                    return;
                }

                contours.push_back(curve);
                curve = Curve();
                curve.setSegmentCount(kBezierSegments);
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

                const bool relative = std::islower(static_cast<unsigned char>(command));
                const char type     = static_cast<char>(std::toupper(static_cast<unsigned char>(command)));

                auto readPoint = [&](const Vec2& inOrigin) -> Vec2
                {
                    const float x = scanner.number();
                    const float y = scanner.number();

                    return relative ? Vec2(inOrigin.x + x, inOrigin.y + y) : Vec2(x, y);
                };

                if (type == 'M')
                {
                    flush();
                    current = readPoint(current);
                    start   = current;
                    curve.addPoint(current);
                    hasCubic = false;
                    hasQuad  = false;
                    command  = relative ? 'l' : 'L';

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
                        current.x     = relative ? current.x + x : x;
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
                        current.y     = relative ? current.y + y : y;
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
                        const Vec2 controlA =
                            hasCubic ? Vec2((2.0f * current.x) - lastCubic.x, (2.0f * current.y) - lastCubic.y)
                                     : current;
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
                        const Vec2 control =
                            hasQuad ? Vec2((2.0f * current.x) - lastQuad.x, (2.0f * current.y) - lastQuad.y) : current;
                        const Vec2 point = readPoint(current);
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
                        const float rx    = scanner.number();
                        const float ry    = scanner.number();
                        const float angle = scanner.number();
                        const bool  large = scanner.flag();
                        const bool  sweep = scanner.flag();
                        const Vec2  point = readPoint(current);
                        addArc(curve, current, rx, ry, angle, large, sweep, point);
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

                points.push_back({x, scanner.number()});
            }

            return points;
        }

        Curve makeEllipse(float inCx, float inCy, float inRx, float inRy)
        {
            Curve curve;
            curve.setSegmentCount(kBezierSegments);
            curve.addPoint({inCx + inRx, inCy});
            curve.addBezierPoint(
                {inCx + inRx, inCy + (kKappa * inRy)},
                {inCx + (kKappa * inRx), inCy + inRy},
                {inCx, inCy + inRy}
            );
            curve.addBezierPoint(
                {inCx - (kKappa * inRx), inCy + inRy},
                {inCx - inRx, inCy + (kKappa * inRy)},
                {inCx - inRx, inCy}
            );
            curve.addBezierPoint(
                {inCx - inRx, inCy - (kKappa * inRy)},
                {inCx - (kKappa * inRx), inCy - inRy},
                {inCx, inCy - inRy}
            );
            curve.addBezierPoint(
                {inCx + (kKappa * inRx), inCy - inRy},
                {inCx + inRx, inCy - (kKappa * inRy)},
                {inCx + inRx, inCy}
            );

            return curve;
        }

        Curve makeRect(float inX, float inY, float inWidth, float inHeight, float inRx, float inRy)
        {
            Curve curve;
            curve.setSegmentCount(kBezierSegments);

            float rx = std::max(0.0f, inRx);
            float ry = std::max(0.0f, inRy);

            if (rx <= 0.0f && ry > 0.0f)
            {
                rx = ry;
            }

            if (ry <= 0.0f && rx > 0.0f)
            {
                ry = rx;
            }

            rx = std::min(rx, inWidth * 0.5f);
            ry = std::min(ry, inHeight * 0.5f);

            if (rx <= 0.0f || ry <= 0.0f)
            {
                curve.addPoint({inX, inY});
                curve.addPoint({inX + inWidth, inY});
                curve.addPoint({inX + inWidth, inY + inHeight});
                curve.addPoint({inX, inY + inHeight});
                curve.addPoint({inX, inY});

                return curve;
            }

            const float kx = kKappa * rx;
            const float ky = kKappa * ry;

            curve.addPoint({inX + rx, inY});
            curve.addPoint({inX + inWidth - rx, inY});
            curve.addBezierPoint(
                {inX + inWidth - rx + kx, inY},
                {inX + inWidth, inY + ry - ky},
                {inX + inWidth, inY + ry}
            );
            curve.addPoint({inX + inWidth, inY + inHeight - ry});
            curve.addBezierPoint(
                {inX + inWidth, inY + inHeight - ry + ky},
                {inX + inWidth - rx + kx, inY + inHeight},
                {inX + inWidth - rx, inY + inHeight}
            );
            curve.addPoint({inX + rx, inY + inHeight});
            curve.addBezierPoint(
                {inX + rx - kx, inY + inHeight},
                {inX, inY + inHeight - ry + ky},
                {inX, inY + inHeight - ry}
            );
            curve.addPoint({inX, inY + ry});
            curve.addBezierPoint({inX, inY + ry - ky}, {inX + rx - kx, inY}, {inX + rx, inY});

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

                if (local.getPoints().size() >= 2)
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
                    inView.x,
                    inView.y,
                    inView.width,
                    inView.height,
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
            static std::unordered_map<std::string, Primitive> cache;

            const String key   = makeGeometryKey("fill", inContours, inPaint, inView);
            const auto   found = cache.find(key.toStandard());
            if (found != cache.end())
            {
                return found->second;
            }

            return cache.emplace(key.toStandard(), buildFill(inContours, inPaint, inView)).first->second;
        }

        Primitive buildStroke(const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView)
        {
            Primitive primitive;

            if (inPaint.strokeWidth <= 0.0f)
            {
                return primitive;
            }

            const float scaleX = std::sqrt(
                (inPaint.transform[0][0] * inPaint.transform[0][0]) +
                (inPaint.transform[0][1] * inPaint.transform[0][1])
            );
            const float scaleY = std::sqrt(
                (inPaint.transform[1][0] * inPaint.transform[1][0]) +
                (inPaint.transform[1][1] * inPaint.transform[1][1])
            );
            const float half = inPaint.strokeWidth * 0.5f * ((scaleX + scaleY) * 0.5f);

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
                vertex.uv.x       = inPoint.x + 0.5f;
                vertex.uv.y       = inPoint.y + 0.5f;
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

                if (length < kMinLength)
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
                    const float angle =
                        static_cast<float>(M_PI) * (static_cast<float>(i) / SvgPaint::STROKE_ARC_SEGMENTS);
                    const Vec2 curr = inCenter + scaleVec(inNormal, -std::cos(angle) * half) +
                                      scaleVec(inOutbound, std::sin(angle) * half);

                    emitTriangle(inCenter, prev, curr);

                    prev = curr;
                }
            };

            auto unitVec = [&](const Vec2& inValue) -> Vec2
            {
                const float length = vecLength(inValue);

                if (length < kMinLength)
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

                const float pi    = static_cast<float>(M_PI);
                const float twoPi = pi * 2.0f;
                float       start = std::atan2(from.y, from.x);
                float       delta = std::atan2(to.y, to.x) - start;

                while (delta > pi)
                {
                    delta -= twoPi;
                }

                while (delta < -pi)
                {
                    delta += twoPi;
                }

                const float stepSize = pi / static_cast<float>(SvgPaint::STROKE_ARC_SEGMENTS);
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

                    if (!points.empty() && vecLength(mapped - points.back()) < kMinLength)
                    {
                        continue;
                    }

                    points.push_back(mapped);
                }

                bool bIsClosed = false;

                if (points.size() >= 3 && vecLength(points.front() - points.back()) < kMinLength)
                {
                    points.pop_back();
                    bIsClosed = true;
                }

                if (points.size() < 2)
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
                    startLen >= kMinLength ? Vec2(-startDir.x / startLen, -startDir.y / startLen) : Vec2::Zero();
                const Vec2 endOut = endLen >= kMinLength ? Vec2(endDir.x / endLen, endDir.y / endLen) : Vec2::Zero();

                emitCap(points.front(), startOut, sideNormal(startDir));
                emitCap(points.back(), endOut, sideNormal(endDir));
            }

            return primitive;
        }

        const Primitive& cachedStroke(
            const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
        )
        {
            static std::unordered_map<std::string, Primitive> cache;

            const String key   = makeGeometryKey("stroke", inContours, inPaint, inView);
            const auto   found = cache.find(key.toStandard());
            if (found != cache.end())
            {
                return found->second;
            }

            return cache.emplace(key.toStandard(), buildStroke(inContours, inPaint, inView)).first->second;
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
            const String display    = attribute(inNode, "display").toLower();
            const String visibility = attribute(inNode, "visibility").toLower();

            return display.equals("none") || visibility.equals("hidden");
        }

        Svg::Svg(const pugi::xml_node& inNode)
            : Component(inNode),
              m_intrinsic(Vec2::Zero()),
              m_viewBox({}),
              m_signature(""),
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
            Component::refresh();

            if (m_style.isDisplay(StyleDisplay::None) || m_bIsCulled)
            {
                return;
            }

            syncShapes();
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
            if (m_style.isDisplay(StyleDisplay::None) || m_bIsCulled)
            {
                return;
            }

            applySizeAttributes();
            rebuildShapes();
        }

        void Svg::refreshSize()
        {
            const bool bIsWidthAuto  = m_style.width.getRaw().isEmpty() || m_style.width.isRaw(Size::AUTO_KEYWORD);
            const bool bIsHeightAuto = m_style.height.getRaw().isEmpty() || m_style.height.isRaw(Size::AUTO_KEYWORD);

            Component::refreshSize();

            if (m_intrinsic.x <= 0.0f && m_intrinsic.y <= 0.0f)
            {
                return;
            }

            float width  = m_size.x;
            float height = m_size.y;

            if (bIsWidthAuto && bIsHeightAuto)
            {
                width  = m_intrinsic.x;
                height = m_intrinsic.y;
            }
            else if (bIsWidthAuto && m_intrinsic.y > 0.0f)
            {
                width = height * (m_intrinsic.x / m_intrinsic.y);
            }
            else if (bIsHeightAuto && m_intrinsic.x > 0.0f)
            {
                height = width * (m_intrinsic.y / m_intrinsic.x);
            }

            setSize(width, height);
        }

        void Svg::applySizeAttributes()
        {
            if (m_style.width.getRaw().isEmpty())
            {
                const String width = parseText(getAttribute(WIDTH_ATTRIBUTE_NAME)).trim();

                if (!width.isEmpty())
                {
                    m_style.width.setRaw(width);
                }
            }

            if (m_style.height.getRaw().isEmpty())
            {
                const String height = parseText(getAttribute(HEIGHT_ATTRIBUTE_NAME)).trim();

                if (!height.isEmpty())
                {
                    m_style.height.setRaw(height);
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

            if (!m_shapes.empty() && signature.equals(m_signature))
            {
                return;
            }

            m_signature = signature;

            SvgViewBox view;
            view.width  = parseNumber(parseText(attribute(m_sourceNode, WIDTH_ATTRIBUTE_NAME)));
            view.height = parseNumber(parseText(attribute(m_sourceNode, HEIGHT_ATTRIBUTE_NAME)));

            const String viewBoxValue = parseText(attribute(m_sourceNode, VIEWBOX_ATTRIBUTE_NAME)).trim();

            if (!viewBoxValue.isEmpty())
            {
                view = parseViewBox(viewBoxValue);
            }

            if (view.width <= 0.0f)
            {
                view.width = 300.0f;
            }

            if (view.height <= 0.0f)
            {
                view.height = 150.0f;
            }

            m_viewBox     = view;
            m_intrinsic.x = parseNumber(parseText(attribute(m_sourceNode, WIDTH_ATTRIBUTE_NAME)), view.width);
            m_intrinsic.y = parseNumber(parseText(attribute(m_sourceNode, HEIGHT_ATTRIBUTE_NAME)), view.height);

            std::size_t index = 0;
            SvgPaint    root  = applyNode(m_sourceNode, SvgPaint(current), current);

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

                    if (tag.equals("g", "svg", "a"))
                    {
                        walk(child, paint);

                        continue;
                    }

                    std::vector<Curve> contours;

                    if (tag.equals("path"))
                    {
                        contours = parsePath(parseText(attribute(child, "d")));
                    }
                    else if (tag.equals("circle"))
                    {
                        contours.push_back(makeEllipse(
                            parseNumber(parseText(attribute(child, "cx"))),
                            parseNumber(parseText(attribute(child, "cy"))),
                            parseNumber(parseText(attribute(child, "r"))),
                            parseNumber(parseText(attribute(child, "r")))
                        ));
                    }
                    else if (tag.equals("ellipse"))
                    {
                        contours.push_back(makeEllipse(
                            parseNumber(parseText(attribute(child, "cx"))),
                            parseNumber(parseText(attribute(child, "cy"))),
                            parseNumber(parseText(attribute(child, "rx"))),
                            parseNumber(parseText(attribute(child, "ry")))
                        ));
                    }
                    else if (tag.equals("rect"))
                    {
                        contours.push_back(makeRect(
                            parseNumber(parseText(attribute(child, "x"))),
                            parseNumber(parseText(attribute(child, "y"))),
                            parseNumber(parseText(attribute(child, "width"))),
                            parseNumber(parseText(attribute(child, "height"))),
                            parseNumber(parseText(attribute(child, "rx"))),
                            parseNumber(parseText(attribute(child, "ry")))
                        ));
                    }
                    else if (tag.equals("line"))
                    {
                        Curve line;
                        line.addPoint(
                            {parseNumber(parseText(attribute(child, "x1"))),
                             parseNumber(parseText(attribute(child, "y1")))}
                        );
                        line.addPoint(
                            {parseNumber(parseText(attribute(child, "x2"))),
                             parseNumber(parseText(attribute(child, "y2")))}
                        );
                        contours.push_back(line);
                    }
                    else if (tag.equals("polyline", "polygon"))
                    {
                        Curve                   poly;
                        const std::vector<Vec2> points = parsePoints(parseText(attribute(child, "points")));

                        for (const Vec2& point : points)
                        {
                            poly.addPoint(point);
                        }

                        if (tag.equals("polygon") && !points.empty())
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

            for (std::size_t i = index; i < m_shapes.size(); i++)
            {
                m_shapes.at(i)->clear();
            }
        }

        void Svg::syncShapes()
        {
            const Vec2& size   = getSize();
            const float extent = std::max(m_viewBox.width, m_viewBox.height);
            const float fit    = std::min(
                m_viewBox.width > 0.0f ? size.x / m_viewBox.width : 0.0f,
                m_viewBox.height > 0.0f ? size.y / m_viewBox.height : 0.0f
            );
            const float scale = extent * fit;

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
                markFlatDirty();
            }

            return m_shapes.at(inIndex);
        }
    }
}
