#include "Chicane/Box/Asset/Preview.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <vector>

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static constexpr std::size_t PREVIEW_EXTRACT_LIMIT = 512 * 1024;

        static std::unique_ptr<AssetPreview> parsePreviewNode(
            const pugi::xml_node&     inNode,
            const FileSystem::Path&   inAsset
        )
        {
            if (inNode.empty() || !String(inNode.name()).equals(AssetPreview::TAG))
            {
                return nullptr;
            }

            const int width  = Xml::getAttribute(AssetPreview::WIDTH_ATTRIBUTE_NAME, inNode).as_int();
            const int height = Xml::getAttribute(AssetPreview::HEIGHT_ATTRIBUTE_NAME, inNode).as_int();
            if (width <= 0 || height <= 0)
            {
                return nullptr;
            }

            const Image::Raw encoded = Base64::decodeToUnsigned(inNode.text().as_string());
            if (encoded.empty())
            {
                return nullptr;
            }

            Image::Instance image;
            const bool      isPng = encoded.size() >= 8 && encoded[0] == 137 && encoded[1] == 80 &&
                               encoded[2] == 78 && encoded[3] == 71;
            if (isPng)
            {
                try
                {
                    image = std::make_shared<Image>(encoded, ImageVendor::Png);
                }
                catch (...)
                {
                    return nullptr;
                }
            }
            else
            {
                const std::size_t bytes = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) *
                                          static_cast<std::size_t>(AssetPreview::CHANNELS);
                if (encoded.size() < bytes)
                {
                    return nullptr;
                }

                image = std::make_shared<Image>(
                    encoded.data(),
                    width,
                    height,
                    AssetPreview::CHANNELS,
                    AssetPreview::CHANNELS
                );
            }

            if (!image || image->getPixels() == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->id = Xml::getAttribute(Asset::ID_ATTRIBUTE_NAME, inNode).as_string();
            result->type =
                AssetHeader::getTypeFromTag(Xml::getAttribute(AssetPreview::TYPE_ATTRIBUTE_NAME, inNode).as_string());
            result->image = image;

            return result;
        }

        String AssetPreview::textureIdOf(const FileSystem::Path& inAsset)
        {
            String result = TEXTURE_PREFIX;
            result.append(inAsset.lexicallyNormal().toString());

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::create(
            const FileSystem::Path& inAsset,
            const String&           inId,
            AssetType               inType,
            const Image&            inImage
        )
        {
            if (inImage.getPixels() == nullptr || inImage.getWidth() <= 0 || inImage.getHeight() <= 0)
            {
                return nullptr;
            }

            std::vector<unsigned char> pixels(
                static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(CHANNELS)
            );
            inImage.blit(pixels.data(), SIZE, SIZE, 0);

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->id                           = inId;
            result->type                         = inType;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        void previewCamera(Vec3& outViewDir, Vec3& outRight, Vec3& outUp, Vec3& outLight)
        {
            outViewDir = Vec3(-0.9f, -0.6f, -0.7f).normalize();
            outRight   = outViewDir.cross(Vec3::Up());
            if (outRight.dot(outRight) < 1e-8f)
            {
                outRight = Vec3::Right();
            }

            outRight = outRight.normalize();
            outUp    = outRight.cross(outViewDir).normalize();
            outLight = Vec3(0.45f, 0.25f, 0.85f).normalize();
        }

        void sampleCubemap(
            const std::vector<Image::Instance>& inFaces,
            const Vec3&                         inDirection,
            unsigned char*                      outRgba
        )
        {
            outRgba[0] = 36;
            outRgba[1] = 36;
            outRgba[2] = 40;
            outRgba[3] = 255;

            if (inFaces.empty())
            {
                return;
            }

            Vec3 direction = inDirection;
            if (direction.dot(direction) < 1e-10f)
            {
                return;
            }

            direction = direction.normalize();

            const float absX = std::fabs(direction.x);
            const float absY = std::fabs(direction.y);
            const float absZ = std::fabs(direction.z);

            int   face = 0;
            float sc   = 0.0f;
            float tc   = 0.0f;
            float ma   = absX;

            if (absX >= absY && absX >= absZ)
            {
                face = direction.x >= 0.0f ? 0 : 1;
                ma   = absX;
                sc   = direction.x >= 0.0f ? -direction.z : direction.z;
                tc   = -direction.y;
            }
            else if (absY >= absZ)
            {
                face = direction.y >= 0.0f ? 2 : 3;
                ma   = absY;
                sc   = direction.x;
                tc   = direction.y >= 0.0f ? direction.z : -direction.z;
            }
            else
            {
                face = direction.z >= 0.0f ? 4 : 5;
                ma   = absZ;
                sc   = direction.z >= 0.0f ? direction.x : -direction.x;
                tc   = -direction.y;
            }

            const Image::Instance& image = inFaces.at(std::min(static_cast<std::size_t>(face), inFaces.size() - 1));
            if (!image || image->getPixels() == nullptr || image->getWidth() <= 0 || image->getHeight() <= 0)
            {
                return;
            }

            const int             width    = image->getWidth();
            const int             height   = image->getHeight();
            const int             channels = std::max(1, image->getChannel());
            const float           u        = ((sc / ma) + 1.0f) * 0.5f;
            const float           v        = ((tc / ma) + 1.0f) * 0.5f;
            const int             x = std::clamp(static_cast<int>(u * static_cast<float>(width - 1)), 0, width - 1);
            const int             y = std::clamp(static_cast<int>(v * static_cast<float>(height - 1)), 0, height - 1);
            const Image::Pixels   pixels = image->getPixels();
            const std::size_t     offset =
                ((static_cast<std::size_t>(y) * static_cast<std::size_t>(width)) + static_cast<std::size_t>(x)) *
                static_cast<std::size_t>(channels);

            outRgba[0] = pixels[offset];
            outRgba[1] = channels > 1 ? pixels[offset + 1] : pixels[offset];
            outRgba[2] = channels > 2 ? pixels[offset + 2] : pixels[offset];
            outRgba[3] = channels > 3 ? pixels[offset + 3] : 255;
        }

        void appendUnitCube(Vertex::List& outVertices, Vertex::Indices& outIndices)
        {
            const Vertex::Index base = static_cast<Vertex::Index>(outVertices.size());
            const std::array<Vec3, 8> corners = {
                Vec3(-1.0f, -1.0f, -1.0f),
                Vec3(1.0f,  -1.0f, -1.0f),
                Vec3(1.0f,  1.0f,  -1.0f),
                Vec3(-1.0f, 1.0f,  -1.0f),
                Vec3(-1.0f, -1.0f, 1.0f),
                Vec3(1.0f,  -1.0f, 1.0f),
                Vec3(1.0f,  1.0f,  1.0f),
                Vec3(-1.0f, 1.0f,  1.0f)
            };

            for (const Vec3& corner : corners)
            {
                Vertex vertex;
                vertex.position = corner;
                outVertices.push_back(vertex);
            }

            const std::array<Vertex::Index, 36> faces = {
                1, 2, 6, 1, 6, 5, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2,
                0, 1, 5, 0, 5, 4, 4, 5, 6, 4, 6, 7, 0, 3, 2, 0, 2, 1
            };

            for (const Vertex::Index index : faces)
            {
                outIndices.push_back(base + index);
            }
        }

        std::unique_ptr<AssetPreview> rasterPreview(
            const FileSystem::Path&             inAsset,
            const String&                       inId,
            AssetType                           inType,
            Vertex::List                        inVertices,
            Vertex::Indices                     inIndices,
            const std::vector<Image::Instance>& inFaces
        )
        {
            if (inVertices.empty() && !inFaces.empty())
            {
                appendUnitCube(inVertices, inIndices);
            }

            if (inAsset.isEmpty() || inVertices.empty())
            {
                return nullptr;
            }

            if (inIndices.empty())
            {
                inIndices.resize(inVertices.size());
                for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(inVertices.size()); i++)
                {
                    inIndices[i] = i;
                }
            }

            const std::size_t triangleCount = inIndices.size() / 3;
            if (triangleCount == 0)
            {
                return nullptr;
            }

            Vec3 minPosition = inVertices.front().position;
            Vec3 maxPosition = minPosition;
            for (const Vertex& vertex : inVertices)
            {
                minPosition = minPosition.min(vertex.position);
                maxPosition = maxPosition.max(vertex.position);
            }

            const Vec3 center = (minPosition + maxPosition) * 0.5f;
            const Vec3 extent = maxPosition - minPosition;
            if (extent.dot(extent) <= 1e-10f)
            {
                return nullptr;
            }

            Vec3 viewDir;
            Vec3 right;
            Vec3 up;
            Vec3 light;
            previewCamera(viewDir, right, up, light);

            struct Projected
            {
                float x;
                float y;
                float z;
            };

            std::vector<Projected> projected(inVertices.size());
            float                  minX = std::numeric_limits<float>::max();
            float                  maxX = std::numeric_limits<float>::lowest();
            float                  minY = std::numeric_limits<float>::max();
            float                  maxY = std::numeric_limits<float>::lowest();

            for (std::size_t i = 0; i < inVertices.size(); i++)
            {
                const Vec3 offset = inVertices.at(i).position - center;
                projected[i].x    = offset.dot(right);
                projected[i].y    = offset.dot(up);
                projected[i].z    = offset.dot(viewDir);
                minX              = std::min(minX, projected[i].x);
                maxX              = std::max(maxX, projected[i].x);
                minY              = std::min(minY, projected[i].y);
                maxY              = std::max(maxY, projected[i].y);
            }

            const float spanX   = std::max(maxX - minX, 1e-5f);
            const float spanY   = std::max(maxY - minY, 1e-5f);
            const float scale   = (static_cast<float>(AssetPreview::SIZE) * 0.82f) / std::max(spanX, spanY);
            const float midX    = (minX + maxX) * 0.5f;
            const float midY    = (minY + maxY) * 0.5f;
            const float originX = (static_cast<float>(AssetPreview::SIZE) * 0.5f) - (midX * scale);
            const float originY = (static_cast<float>(AssetPreview::SIZE) * 0.5f) - (midY * scale);

            const std::size_t          pixelCount =
                static_cast<std::size_t>(AssetPreview::SIZE) * static_cast<std::size_t>(AssetPreview::SIZE);
            std::vector<unsigned char> pixels(
                pixelCount * static_cast<std::size_t>(AssetPreview::CHANNELS), 0
            );
            std::vector<float> depth(pixelCount, std::numeric_limits<float>::max());

            for (std::size_t i = 0; i < pixelCount; i++)
            {
                pixels[(i * 4) + 0] = 36;
                pixels[(i * 4) + 1] = 36;
                pixels[(i * 4) + 2] = 40;
                pixels[(i * 4) + 3] = 255;
            }

            auto edge = [](float ax, float ay, float bx, float by, float cx, float cy)
            {
                return ((cx - ax) * (by - ay)) - ((cy - ay) * (bx - ax));
            };

            for (std::size_t triangle = 0; triangle < triangleCount; triangle++)
            {
                const Vertex::Index i0 = inIndices.at((triangle * 3) + 0);
                const Vertex::Index i1 = inIndices.at((triangle * 3) + 1);
                const Vertex::Index i2 = inIndices.at((triangle * 3) + 2);
                if (i0 >= inVertices.size() || i1 >= inVertices.size() || i2 >= inVertices.size())
                {
                    continue;
                }

                const Projected& a = projected.at(i0);
                const Projected& b = projected.at(i1);
                const Projected& c = projected.at(i2);

                const float ax   = (a.x * scale) + originX;
                const float ay   = originY + (a.y * scale);
                const float bx   = (b.x * scale) + originX;
                const float by   = originY + (b.y * scale);
                const float cx   = (c.x * scale) + originX;
                const float cy   = originY + (c.y * scale);
                const float area = edge(ax, ay, bx, by, cx, cy);
                if (std::fabs(area) < 1e-5f)
                {
                    continue;
                }

                Vec3 normal = inVertices.at(i0).normal + inVertices.at(i1).normal + inVertices.at(i2).normal;
                if (normal.dot(normal) < 1e-8f)
                {
                    const Vec3 e0 = inVertices.at(i1).position - inVertices.at(i0).position;
                    const Vec3 e1 = inVertices.at(i2).position - inVertices.at(i0).position;
                    normal        = e0.cross(e1);
                }

                if (normal.dot(normal) < 1e-8f)
                {
                    continue;
                }

                normal                    = normal.normalize();
                const float shade         = 0.22f + (0.78f * std::abs(normal.dot(light)));
                const unsigned char clayR = static_cast<unsigned char>(std::clamp(228.0f * shade, 0.0f, 255.0f));
                const unsigned char clayG = static_cast<unsigned char>(std::clamp(232.0f * shade, 0.0f, 255.0f));
                const unsigned char clayB = static_cast<unsigned char>(std::clamp(236.0f * shade, 0.0f, 255.0f));

                const int minPx = std::max(0, static_cast<int>(std::floor(std::min({ax, bx, cx}))));
                const int maxPx = std::min(AssetPreview::SIZE - 1, static_cast<int>(std::ceil(std::max({ax, bx, cx}))));
                const int minPy = std::max(0, static_cast<int>(std::floor(std::min({ay, by, cy}))));
                const int maxPy = std::min(AssetPreview::SIZE - 1, static_cast<int>(std::ceil(std::max({ay, by, cy}))));

                for (int py = minPy; py <= maxPy; py++)
                {
                    const float sampleY = static_cast<float>(py) + 0.5f;

                    for (int px = minPx; px <= maxPx; px++)
                    {
                        const float sampleX = static_cast<float>(px) + 0.5f;
                        const float w0      = edge(bx, by, cx, cy, sampleX, sampleY);
                        const float w1      = edge(cx, cy, ax, ay, sampleX, sampleY);
                        const float w2      = edge(ax, ay, bx, by, sampleX, sampleY);
                        if ((w0 * area) < 0.0f || (w1 * area) < 0.0f || (w2 * area) < 0.0f)
                        {
                            continue;
                        }

                        const float z = ((a.z * w0) + (b.z * w1) + (c.z * w2)) / area;
                        const std::size_t index =
                            (static_cast<std::size_t>(py) * static_cast<std::size_t>(AssetPreview::SIZE)) +
                            static_cast<std::size_t>(px);
                        if (z >= depth.at(index))
                        {
                            continue;
                        }

                        depth[index] = z;

                        if (inFaces.empty())
                        {
                            pixels[(index * 4) + 0] = clayR;
                            pixels[(index * 4) + 1] = clayG;
                            pixels[(index * 4) + 2] = clayB;
                            pixels[(index * 4) + 3] = 255;

                            continue;
                        }

                        const Vec3 position =
                            ((inVertices.at(i0).position * w0) + (inVertices.at(i1).position * w1) +
                             (inVertices.at(i2).position * w2)) /
                            area;
                        unsigned char color[4] = {36, 36, 40, 255};
                        sampleCubemap(inFaces, position, color);
                        pixels[(index * 4) + 0] = color[0];
                        pixels[(index * 4) + 1] = color[1];
                        pixels[(index * 4) + 2] = color[2];
                        pixels[(index * 4) + 3] = color[3];
                    }
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->id                           = inId;
            result->type                         = inType;
            result->image                        = std::make_shared<Image>(
                pixels.data(),
                AssetPreview::SIZE,
                AssetPreview::SIZE,
                AssetPreview::CHANNELS,
                AssetPreview::CHANNELS
            );

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromGeometry(
            const FileSystem::Path& inAsset,
            const String&           inId,
            const Vertex::List&     inVertices,
            const Vertex::Indices&  inIndices
        )
        {
            return rasterPreview(inAsset, inId, AssetType::Mesh, inVertices, inIndices, {});
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromSky(
            const FileSystem::Path&             inAsset,
            const String&                       inId,
            const Vertex::List&                 inVertices,
            const Vertex::Indices&              inIndices,
            const std::vector<Image::Instance>& inFaces
        )
        {
            return rasterPreview(inAsset, inId, AssetType::Sky, inVertices, inIndices, inFaces);
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromFont(
            const FileSystem::Path& inAsset,
            const String&           inId,
            const FontFamily&       inFamily,
            const String&           inLabel
        )
        {
            if (inAsset.isEmpty())
            {
                return nullptr;
            }

            String label = inLabel.trim();
            if (label.isEmpty())
            {
                label = inFamily.getFamily();
            }
            if (label.isEmpty())
            {
                label = inFamily.getName();
            }
            if (label.isEmpty())
            {
                label = inId;
            }

            struct Point
            {
                float x;
                float y;
            };

            auto appendGlyph = [&](std::vector<Point>&         outPoints,
                                   std::vector<Vertex::Index>& outIndices,
                                   const FontGlyph&            inGlyph,
                                   float                       inCursor)
            {
                if (inGlyph.vertices.empty())
                {
                    return false;
                }

                const Vertex::Index base = static_cast<Vertex::Index>(outPoints.size());
                for (const Vertex& vertex : inGlyph.vertices)
                {
                    outPoints.push_back({vertex.position.x + inCursor, vertex.position.y});
                }

                if (inGlyph.indices.empty())
                {
                    for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(inGlyph.vertices.size()); i++)
                    {
                        outIndices.push_back(base + i);
                    }
                }
                else
                {
                    for (const Vertex::Index index : inGlyph.indices)
                    {
                        outIndices.push_back(base + index);
                    }
                }

                return true;
            };

            auto layout = [&](const std::vector<char32_t>& inCodes)
            {
                std::vector<Point>         points  = {};
                std::vector<Vertex::Index> indices = {};
                float                      cursor  = 0.0f;
                char32_t                   previous = 0;

                for (char32_t code : inCodes)
                {
                    if (previous != 0)
                    {
                        cursor += inFamily.getKerning(previous, code);
                    }

                    if (!inFamily.hasGlyph(code))
                    {
                        if (code == U' ')
                        {
                            cursor += 0.35f;
                        }

                        previous = code;

                        continue;
                    }

                    const FontGlyph& glyph = inFamily.getGlyph(code);
                    appendGlyph(points, indices, glyph, cursor);
                    cursor += glyph.advance;
                    previous = code;
                }

                return std::make_pair(std::move(points), std::move(indices));
            };

            std::vector<char32_t> codes = label.toUnicode();
            auto                  mesh  = layout(codes);
            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes = {'A', 'a'};
                mesh  = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes.clear();
                for (char32_t code = U'A'; code <= U'Z' && codes.size() < 4; code++)
                {
                    if (inFamily.hasGlyph(code))
                    {
                        codes.push_back(code);
                    }
                }

                mesh = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes.clear();
                for (const auto& entry : inFamily.getGlyphs())
                {
                    if (entry.second.vertices.empty())
                    {
                        continue;
                    }

                    codes.push_back(entry.first);
                    if (codes.size() >= 4)
                    {
                        break;
                    }
                }

                mesh = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                return nullptr;
            }

            const std::vector<Point>&         points  = mesh.first;
            const std::vector<Vertex::Index>& indices = mesh.second;

            float minX = points.front().x;
            float maxX = minX;
            float minY = points.front().y;
            float maxY = minY;
            for (const Point& point : points)
            {
                minX = std::min(minX, point.x);
                maxX = std::max(maxX, point.x);
                minY = std::min(minY, point.y);
                maxY = std::max(maxY, point.y);
            }

            const float spanX = std::max(maxX - minX, 1e-5f);
            const float spanY = std::max(maxY - minY, 1e-5f);
            const float scale = (static_cast<float>(SIZE) * 0.78f) / std::max(spanX, spanY);
            const float midX  = (minX + maxX) * 0.5f;
            const float midY  = (minY + maxY) * 0.5f;
            const float originX = (static_cast<float>(SIZE) * 0.5f) - (midX * scale);
            const float originY = (static_cast<float>(SIZE) * 0.5f) - (midY * scale);

            const std::size_t          pixelCount = static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE);
            std::vector<unsigned char> pixels(pixelCount * static_cast<std::size_t>(CHANNELS), 0);

            for (std::size_t i = 0; i < pixelCount; i++)
            {
                pixels[(i * 4) + 0] = 36;
                pixels[(i * 4) + 1] = 36;
                pixels[(i * 4) + 2] = 40;
                pixels[(i * 4) + 3] = 255;
            }

            auto edge = [](float ax, float ay, float bx, float by, float cx, float cy)
            {
                return ((cx - ax) * (by - ay)) - ((cy - ay) * (bx - ax));
            };

            const std::size_t triangleCount = indices.size() / 3;
            for (std::size_t triangle = 0; triangle < triangleCount; triangle++)
            {
                const Vertex::Index i0 = indices.at((triangle * 3) + 0);
                const Vertex::Index i1 = indices.at((triangle * 3) + 1);
                const Vertex::Index i2 = indices.at((triangle * 3) + 2);
                if (i0 >= points.size() || i1 >= points.size() || i2 >= points.size())
                {
                    continue;
                }

                const float ax = (points.at(i0).x * scale) + originX;
                const float ay = originY + (points.at(i0).y * scale);
                const float bx = (points.at(i1).x * scale) + originX;
                const float by = originY + (points.at(i1).y * scale);
                const float cx = (points.at(i2).x * scale) + originX;
                const float cy = originY + (points.at(i2).y * scale);
                const float area = edge(ax, ay, bx, by, cx, cy);
                if (std::fabs(area) < 1e-5f)
                {
                    continue;
                }

                const int minPx = std::max(0, static_cast<int>(std::floor(std::min({ax, bx, cx}))));
                const int maxPx = std::min(SIZE - 1, static_cast<int>(std::ceil(std::max({ax, bx, cx}))));
                const int minPy = std::max(0, static_cast<int>(std::floor(std::min({ay, by, cy}))));
                const int maxPy = std::min(SIZE - 1, static_cast<int>(std::ceil(std::max({ay, by, cy}))));

                for (int py = minPy; py <= maxPy; py++)
                {
                    const float sampleY = static_cast<float>(py) + 0.5f;

                    for (int px = minPx; px <= maxPx; px++)
                    {
                        const float sampleX = static_cast<float>(px) + 0.5f;
                        const float w0      = edge(bx, by, cx, cy, sampleX, sampleY);
                        const float w1      = edge(cx, cy, ax, ay, sampleX, sampleY);
                        const float w2      = edge(ax, ay, bx, by, sampleX, sampleY);
                        if ((w0 * area) < 0.0f || (w1 * area) < 0.0f || (w2 * area) < 0.0f)
                        {
                            continue;
                        }

                        const std::size_t index = (static_cast<std::size_t>(py) * static_cast<std::size_t>(SIZE)) +
                                                  static_cast<std::size_t>(px);
                        pixels[(index * 4) + 0] = 236;
                        pixels[(index * 4) + 1] = 236;
                        pixels[(index * 4) + 2] = 240;
                        pixels[(index * 4) + 3] = 255;
                    }
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->id                           = inId;
            result->type                         = AssetType::Font;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromSound(
            const FileSystem::Path&           inAsset,
            const String&                     inId,
            const std::vector<unsigned char>& inData
        )
        {
            if (inData.empty())
            {
                return nullptr;
            }

            auto readU16 = [](const unsigned char* inBytes)
            {
                return static_cast<std::uint16_t>(inBytes[0] | (static_cast<std::uint16_t>(inBytes[1]) << 8));
            };
            auto readU32 = [](const unsigned char* inBytes)
            {
                return static_cast<std::uint32_t>(
                    inBytes[0] | (static_cast<std::uint32_t>(inBytes[1]) << 8) |
                    (static_cast<std::uint32_t>(inBytes[2]) << 16) | (static_cast<std::uint32_t>(inBytes[3]) << 24)
                );
            };

            std::vector<float> samples;
            const unsigned char* bytes  = inData.data();
            const std::size_t    size   = inData.size();

            if (size >= 12 && bytes[0] == 'R' && bytes[1] == 'I' && bytes[2] == 'F' && bytes[3] == 'F' &&
                bytes[8] == 'W' && bytes[9] == 'A' && bytes[10] == 'V' && bytes[11] == 'E')
            {
                std::uint16_t format       = 1;
                std::uint16_t channels     = 1;
                std::uint16_t bits         = 16;
                const unsigned char* data  = nullptr;
                std::size_t          dataSize = 0;
                std::size_t          offset   = 12;

                while (offset + 8 <= size)
                {
                    const char*       chunkId   = reinterpret_cast<const char*>(bytes + offset);
                    const std::uint32_t chunkSize = readU32(bytes + offset + 4);
                    offset += 8;

                    if (offset + chunkSize > size)
                    {
                        break;
                    }

                    if (chunkId[0] == 'f' && chunkId[1] == 'm' && chunkId[2] == 't' && chunkId[3] == ' ')
                    {
                        if (chunkSize >= 16)
                        {
                            format   = readU16(bytes + offset);
                            channels = std::max<std::uint16_t>(1, readU16(bytes + offset + 2));
                            bits     = readU16(bytes + offset + 14);
                        }
                    }
                    else if (chunkId[0] == 'd' && chunkId[1] == 'a' && chunkId[2] == 't' && chunkId[3] == 'a')
                    {
                        data     = bytes + offset;
                        dataSize = chunkSize;
                    }

                    offset += chunkSize + (chunkSize & 1);
                }

                const int bytesPerSample = std::max(1, static_cast<int>(bits / 8));
                const int frameSize      = std::max(1, static_cast<int>(channels) * bytesPerSample);
                const std::size_t frames = data && frameSize > 0 ? dataSize / static_cast<std::size_t>(frameSize) : 0;
                samples.reserve(frames);

                for (std::size_t frame = 0; frame < frames; frame++)
                {
                    const unsigned char* cursor = data + (frame * static_cast<std::size_t>(frameSize));
                    float                mixed  = 0.0f;

                    for (std::uint16_t channel = 0; channel < channels; channel++)
                    {
                        const unsigned char* sample = cursor + (channel * bytesPerSample);
                        float                value  = 0.0f;

                        if (format == 3 && bits == 32)
                        {
                            std::uint32_t bitsValue = readU32(sample);
                            float         decoded   = 0.0f;
                            std::memcpy(&decoded, &bitsValue, sizeof(float));
                            value = decoded;
                        }
                        else if (bits == 8)
                        {
                            value = (static_cast<float>(sample[0]) - 128.0f) / 128.0f;
                        }
                        else if (bits == 16)
                        {
                            const std::int16_t decoded =
                                static_cast<std::int16_t>(sample[0] | (static_cast<std::uint16_t>(sample[1]) << 8));
                            value = static_cast<float>(decoded) / 32768.0f;
                        }
                        else if (bits == 24)
                        {
                            std::int32_t decoded =
                                sample[0] | (static_cast<std::int32_t>(sample[1]) << 8) |
                                (static_cast<std::int32_t>(sample[2]) << 16);
                            if ((decoded & 0x800000) != 0)
                            {
                                decoded |= static_cast<std::int32_t>(0xFF000000);
                            }
                            value = static_cast<float>(decoded) / 8388608.0f;
                        }
                        else if (bits == 32)
                        {
                            const std::int32_t decoded = static_cast<std::int32_t>(readU32(sample));
                            value = static_cast<float>(decoded) / 2147483648.0f;
                        }

                        mixed += value;
                    }

                    samples.push_back(mixed / static_cast<float>(channels));
                }
            }

            if (samples.empty())
            {
                samples.reserve(size);
                for (unsigned char byte : inData)
                {
                    samples.push_back((static_cast<float>(byte) - 128.0f) / 128.0f);
                }
            }

            if (samples.empty())
            {
                return nullptr;
            }

            const std::size_t          pixelCount = static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE);
            std::vector<unsigned char> pixels(pixelCount * static_cast<std::size_t>(CHANNELS), 0);

            for (std::size_t i = 0; i < pixelCount; i++)
            {
                pixels[(i * 4) + 0] = 36;
                pixels[(i * 4) + 1] = 36;
                pixels[(i * 4) + 2] = 40;
                pixels[(i * 4) + 3] = 255;
            }

            const int mid = SIZE / 2;
            for (int x = 12; x < SIZE - 12; x++)
            {
                const std::size_t index = (static_cast<std::size_t>(mid) * static_cast<std::size_t>(SIZE)) +
                                          static_cast<std::size_t>(x);
                pixels[(index * 4) + 0] = 58;
                pixels[(index * 4) + 1] = 58;
                pixels[(index * 4) + 2] = 64;
            }

            const float   inner      = static_cast<float>(SIZE - 24);
            const int     maxHeight  = static_cast<int>(static_cast<float>(SIZE) * 0.38f);
            const std::size_t count  = samples.size();

            for (int x = 12; x < SIZE - 12; x++)
            {
                const float       t     = static_cast<float>(x - 12) / std::max(inner - 1.0f, 1.0f);
                const std::size_t start = static_cast<std::size_t>(t * static_cast<float>(count));
                const std::size_t end   = std::min(
                    count,
                    static_cast<std::size_t>(((static_cast<float>(x - 11) / std::max(inner, 1.0f)) * static_cast<float>(count)) + 1.0f)
                );

                float peak = 0.0f;
                for (std::size_t i = start; i < end; i++)
                {
                    peak = std::max(peak, std::fabs(samples.at(i)));
                }

                const int height = std::max(1, static_cast<int>(peak * static_cast<float>(maxHeight)));
                const int minY   = std::max(0, mid - height);
                const int maxY   = std::min(SIZE - 1, mid + height);

                for (int y = minY; y <= maxY; y++)
                {
                    const std::size_t index = (static_cast<std::size_t>(y) * static_cast<std::size_t>(SIZE)) +
                                              static_cast<std::size_t>(x);
                    pixels[(index * 4) + 0] = 236;
                    pixels[(index * 4) + 1] = 236;
                    pixels[(index * 4) + 2] = 240;
                    pixels[(index * 4) + 3] = 255;
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->id                           = inId;
            result->type                         = AssetType::Sound;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        bool AssetPreview::write(
            pugi::xml_node inRoot,
            const String&  inId,
            AssetType      inType,
            const Image&   inImage
        )
        {
            if (inRoot.empty())
            {
                return false;
            }

            const std::unique_ptr<AssetPreview> preview = create(FileSystem::Path(), inId, inType, inImage);
            if (!preview || !preview->image || !preview->image->getPixels())
            {
                return false;
            }

            pugi::xml_node node = inRoot.child(TAG);
            if (!node.empty())
            {
                inRoot.remove_child(node);
            }

            node = inRoot.prepend_child(TAG);
            if (node.empty())
            {
                return false;
            }

            Xml::addAttribute(node, Asset::ID_ATTRIBUTE_NAME, preview->id);
            Xml::addAttribute(node, TYPE_ATTRIBUTE_NAME, AssetHeader::getTypeTag(inType));
            Xml::addAttribute(node, WIDTH_ATTRIBUTE_NAME, String(std::to_string(preview->image->getWidth())));
            Xml::addAttribute(node, HEIGHT_ATTRIBUTE_NAME, String(std::to_string(preview->image->getHeight())));

            const Image::Raw encoded = preview->image->encode();
            if (encoded.empty())
            {
                return false;
            }

            Xml::addText(node, Base64::encode(encoded));

            return true;
        }

        bool AssetPreview::bake(
            const FileSystem::Path& inAsset,
            const String&           inId,
            AssetType               inType,
            const Image&            inImage
        )
        {
            if (inAsset.isEmpty() || !FileSystem::exists(inAsset))
            {
                return false;
            }

            try
            {
                pugi::xml_document document = Xml::load(inAsset);
                pugi::xml_node     root     = document.first_child();
                if (root.empty() || !write(root, inId, inType, inImage))
                {
                    return false;
                }

                Xml::save(document, inAsset);

                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        std::unique_ptr<AssetPreview> AssetPreview::read(const FileSystem::Path& inAsset)
        {
            if (inAsset.isEmpty() || !FileSystem::exists(inAsset))
            {
                return nullptr;
            }

            try
            {
                std::ifstream file(inAsset.toStandard(), std::ios::binary);
                if (!file)
                {
                    return nullptr;
                }

                std::string buffer;
                buffer.reserve(PREVIEW_EXTRACT_LIMIT);

                char chunk[4096];
                while (file && buffer.size() < PREVIEW_EXTRACT_LIMIT)
                {
                    file.read(chunk, sizeof(chunk));
                    buffer.append(chunk, static_cast<std::size_t>(file.gcount()));

                    const std::size_t start = buffer.find("<Preview");
                    if (start == std::string::npos)
                    {
                        continue;
                    }

                    const std::size_t end = buffer.find("</Preview>", start);
                    if (end == std::string::npos)
                    {
                        continue;
                    }

                    const std::size_t close = end + 10;
                    pugi::xml_document document;
                    if (!document.load_buffer(buffer.data() + start, close - start, pugi::parse_default | pugi::parse_fragment))
                    {
                        return nullptr;
                    }

                    return parsePreviewNode(document.first_child(), inAsset);
                }

                return nullptr;
            }
            catch (...)
            {
                return nullptr;
            }
        }

        std::unique_ptr<AssetPreview> AssetPreview::read(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                return nullptr;
            }

            if (String(inRoot.name()).equals(TAG))
            {
                return parsePreviewNode(inRoot, {});
            }

            return parsePreviewNode(inRoot.child(TAG), {});
        }

        String AssetPreview::textureId() const
        {
            return textureIdOf(path);
        }
    }
}
