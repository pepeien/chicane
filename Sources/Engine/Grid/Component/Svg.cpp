#include "Chicane/Grid/Component/Svg.reflected.hpp"

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <sstream>
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
        namespace
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

                    if (key.equals("fill"))
                    {
                        parsePaint(value, inCurrent, outPaint.fill, outPaint.bIsFillEnabled);

                        continue;
                    }

                    if (key.equals("stroke"))
                    {
                        parsePaint(value, inCurrent, outPaint.stroke, outPaint.bIsStrokeEnabled);

                        continue;
                    }

                    if (key.equals("stroke-width"))
                    {
                        outPaint.strokeWidth = parseNumber(value, outPaint.strokeWidth);

                        continue;
                    }

                    if (key.equals("opacity"))
                    {
                        outPaint.opacity = parseNumber(value, outPaint.opacity);

                        continue;
                    }

                    if (key.equals("fill-opacity"))
                    {
                        outPaint.fillOpacity = parseNumber(value, outPaint.fillOpacity);

                        continue;
                    }

                    if (key.equals("stroke-opacity"))
                    {
                        outPaint.strokeOpacity = parseNumber(value, outPaint.strokeOpacity);

                        continue;
                    }

                    if (key.equals("fill-rule"))
                    {
                        outPaint.bIsEvenOdd = value.toLower().equals("evenodd");
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
                        rotate[0] = glm::vec3(cosA, sinA, 0.0f);
                        rotate[1] = glm::vec3(-sinA, cosA, 0.0f);

                        if (params.size() >= 3)
                        {
                            Mat3 to(1.0f);
                            Mat3 from(1.0f);
                            to[2]   = glm::vec3(params[1], params[2], 1.0f);
                            from[2] = glm::vec3(-params[1], -params[2], 1.0f);
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
                paint.opacity                = 1.0f;

                parsePaint(attribute(inNode, "fill"), inCurrent, paint.fill, paint.bIsFillEnabled);
                parsePaint(attribute(inNode, "stroke"), inCurrent, paint.stroke, paint.bIsStrokeEnabled);

                const String strokeWidth = attribute(inNode, "stroke-width");
                if (!strokeWidth.isEmpty())
                {
                    paint.strokeWidth = parseNumber(strokeWidth, paint.strokeWidth);
                }

                const String opacity = attribute(inNode, "opacity");
                if (!opacity.isEmpty())
                {
                    paint.opacity = parseNumber(opacity, 1.0f);
                }

                const String fillOpacity = attribute(inNode, "fill-opacity");
                if (!fillOpacity.isEmpty())
                {
                    paint.fillOpacity = parseNumber(fillOpacity, paint.fillOpacity);
                }

                const String strokeOpacity = attribute(inNode, "stroke-opacity");
                if (!strokeOpacity.isEmpty())
                {
                    paint.strokeOpacity = parseNumber(strokeOpacity, paint.strokeOpacity);
                }

                const String fillRule = attribute(inNode, "fill-rule").toLower();
                if (!fillRule.isEmpty())
                {
                    paint.bIsEvenOdd = fillRule.equals("evenodd");
                }

                const String transform = attribute(inNode, "transform");
                if (!transform.isEmpty())
                {
                    paint.transform = parseTransform(transform) * paint.transform;
                }

                parseStyle(attribute(inNode, "style"), inCurrent, paint);

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

                return {
                    ((inPoint.x - inView.x) - (inView.width * 0.5f)) / extent,
                    ((inPoint.y - inView.y) - (inView.height * 0.5f)) / extent
                };
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
                float       c =
                    den <= 0.0f ? 0.0f : std::sqrt(std::max(0.0f, ((rx2 * ry2) - (rx2 * y1p2) - (ry2 * x1p2)) / den));

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
                    const float norm = std::sqrt(((inUx * inUx) + (inUy * inUy)) * ((inVx * inVx) + (inVy * inVy)));
                    const float value =
                        norm <= 0.0f ? 1.0f : std::clamp((inUx * inVx + inUy * inVy) / norm, -1.0f, 1.0f);
                    float angle = std::acos(value);

                    if ((inUx * inVy - inUy * inVx) < 0.0f)
                    {
                        angle = -angle;
                    }

                    return angle;
                };

                const float theta1 = vectorAngle(1.0f, 0.0f, (x1p - cxp) / rx, (y1p - cyp) / ry);
                float dTheta = vectorAngle((x1p - cxp) / rx, (y1p - cyp) / ry, (-x1p - cxp) / rx, (-y1p - cyp) / ry);

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
                        (std::sin(t2 - t1) * (std::sqrt(4.0f + (3.0f * std::tan(half) * std::tan(half))) - 1.0f)) /
                        3.0f;

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
                                hasQuad ? Vec2((2.0f * current.x) - lastQuad.x, (2.0f * current.y) - lastQuad.y)
                                        : current;
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

                curve.addPoint({inX + rx, inY});
                curve.addPoint({inX + inWidth - rx, inY});
                addArc(curve, {inX + inWidth - rx, inY}, rx, ry, 0.0f, false, true, {inX + inWidth, inY + ry});
                curve.addPoint({inX + inWidth, inY + inHeight - ry});
                addArc(
                    curve,
                    {inX + inWidth, inY + inHeight - ry},
                    rx,
                    ry,
                    0.0f,
                    false,
                    true,
                    {inX + inWidth - rx, inY + inHeight}
                );
                curve.addPoint({inX + rx, inY + inHeight});
                addArc(curve, {inX + rx, inY + inHeight}, rx, ry, 0.0f, false, true, {inX, inY + inHeight - ry});
                curve.addPoint({inX, inY + ry});
                addArc(curve, {inX, inY + ry}, rx, ry, 0.0f, false, true, {inX + rx, inY});

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

            Primitive buildStroke(
                const std::vector<Curve>& inContours, const SvgPaint& inPaint, const SvgViewBox& inView
            )
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

                for (const Curve& contour : inContours)
                {
                    const Line::Points& points = contour.getPoints();

                    for (std::size_t i = 1; i < points.size(); i++)
                    {
                        const Vec2  start  = transformPoint(inPaint.transform, points.at(i - 1));
                        const Vec2  end    = transformPoint(inPaint.transform, points.at(i));
                        const Vec2  delta  = end - start;
                        const float length = std::sqrt((delta.x * delta.x) + (delta.y * delta.y));

                        if (length < kMinLength)
                        {
                            continue;
                        }

                        const Vec2 normal(-delta.y / length * half, delta.x / length * half);
                        const Vec2 a = toLocal({start.x + normal.x, start.y + normal.y}, inView);
                        const Vec2 b = toLocal({end.x + normal.x, end.y + normal.y}, inView);
                        const Vec2 c = toLocal({end.x - normal.x, end.y - normal.y}, inView);
                        const Vec2 d = toLocal({start.x - normal.x, start.y - normal.y}, inView);

                        const std::uint32_t index = static_cast<std::uint32_t>(primitive.vertices.size());

                        auto push = [&](const Vec2& inPoint)
                        {
                            Vertex vertex;
                            vertex.position.x = inPoint.x;
                            vertex.position.y = inPoint.y;
                            vertex.uv.x       = inPoint.x + 0.5f;
                            vertex.uv.y       = inPoint.y + 0.5f;
                            primitive.vertices.push_back(vertex);
                        };

                        push(a);
                        push(b);
                        push(c);
                        push(d);

                        primitive.indices.push_back(index);
                        primitive.indices.push_back(index + 1);
                        primitive.indices.push_back(index + 2);
                        primitive.indices.push_back(index);
                        primitive.indices.push_back(index + 2);
                        primitive.indices.push_back(index + 3);
                    }
                }

                return primitive;
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
            std::stringstream stream;
            m_sourceNode.print(stream, "", pugi::format_raw);

            const Color::Rgba current = m_style.foregroundColor.get();
            const String signature    = String(stream.str()) + "|" + std::to_string(static_cast<int>(current.r)) + "," +
                                     std::to_string(static_cast<int>(current.g)) + "," +
                                     std::to_string(static_cast<int>(current.b)) + "," +
                                     std::to_string(static_cast<int>(current.a));

            if (signature.equals(m_signature))
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
                        const Primitive primitive = buildFill(contours, paint, view);

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
                        const Primitive primitive = buildStroke(contours, paint, view);

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
            }

            return m_shapes.at(inIndex);
        }
    }
}
