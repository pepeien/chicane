#include "Chicane/Box/Mesh.hpp"

#include <algorithm>

#include "Chicane/Box/Animation.hpp"
#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Skeleton.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        Mesh::Mesh(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_skeleton({}),
              m_animations({}),
              m_groups({})
        {
            fetchSkeleton();
            fetchAnimations();
            fetchGroups();
        }

        const std::vector<MeshGroup>& Mesh::getGroups() const
        {
            return m_groups;
        }

        void Mesh::setGroups(const std::vector<MeshGroup>& inGroups)
        {
            XmlNode root = getXML();
            for (XmlNode child = root.getFirstChild(); child;)
            {
                XmlNode      next = child.getNextSibling();
                const String name = child.getName();
                if (!name.equals(AssetPreview::TAG) && !name.equals(Skeleton::TAG) && !name.equals(Animation::TAG))
                {
                    root.removeChild(child);
                }

                child = next;
            }

            for (const MeshGroup& group : inGroups)
            {
                appendGroup(group);
            }
        }

        void Mesh::appendGroup(const MeshGroup& inGroup)
        {
            if (!inGroup.isValid())
            {
                return;
            }

            String id = inGroup.getId();

            XmlNode root = getXML();

            if (!Xml::isEmpty(root.findChildByAttribute(MeshGroup::ID_ATTRIBUTE_NAME, id.toChar())))
            {
                throw std::runtime_error("A group with the ID " + inGroup.getId().toStandard() + " already exists");
            }

            XmlNode groupNode = root.appendChild(MeshGroup::TAG);
            groupNode.setAttribute(MeshGroup::ID_ATTRIBUTE_NAME, id);
            if (!inGroup.getBone().isEmpty())
            {
                groupNode.setAttribute(MeshGroup::BONE_ATTRIBUTE_NAME, inGroup.getBone());
            }

            if (inGroup.getEmissiveStrength() != 1.0f)
            {
                groupNode.setAttribute(
                    MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME,
                    String::sprint("%g", inGroup.getEmissiveStrength())
                );
            }

            if (inGroup.getTileSize() > 0.0f)
            {
                groupNode.setAttribute(
                    MeshGroup::TILE_SIZE_ATTRIBUTE_NAME,
                    String::sprint("%g", inGroup.getTileSize())
                );
            }

            // Model
            XmlNode modelNode = groupNode.appendChild(Model::TAG);
            inGroup.getModel().saveTo(modelNode);

            // Textures
            inGroup.saveTextures(groupNode);

            // Transform
            inGroup.saveTransform(groupNode);
        }

        void Mesh::updateGroup(const MeshGroup& inGroup)
        {
            if (!inGroup.isValid())
            {
                return;
            }

            auto foundGroupEntry = std::find_if(
                m_groups.begin(),
                m_groups.end(),
                [inGroup](const MeshGroup& inValue) { return inGroup.getId().equals(inValue.getId()); }
            );

            if (foundGroupEntry == m_groups.end())
            {
                throw std::runtime_error("The group " + inGroup.getId().toStandard() + " wasn't found");
            }

            m_groups[foundGroupEntry - m_groups.begin()] = inGroup;

            String id = inGroup.getId();

            XmlNode root           = getXML();
            XmlNode foundGroupNode = root.findChildByAttribute(MeshGroup::ID_ATTRIBUTE_NAME, id.toChar());

            if (Xml::isEmpty(foundGroupNode))
            {
                appendGroup(inGroup);

                return;
            }

            if (inGroup.getBone().isEmpty())
            {
                foundGroupNode.removeAttribute(MeshGroup::BONE_ATTRIBUTE_NAME);
            }
            else
            {
                foundGroupNode.setAttribute(MeshGroup::BONE_ATTRIBUTE_NAME, inGroup.getBone());
            }

            if (inGroup.getEmissiveStrength() == 1.0f)
            {
                foundGroupNode.removeAttribute(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME);
            }
            else
            {
                foundGroupNode.setAttribute(
                    MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME,
                    String::sprint("%g", inGroup.getEmissiveStrength())
                );
            }

            if (inGroup.getTileSize() <= 0.0f)
            {
                foundGroupNode.removeAttribute(MeshGroup::TILE_SIZE_ATTRIBUTE_NAME);
            }
            else
            {
                foundGroupNode.setAttribute(
                    MeshGroup::TILE_SIZE_ATTRIBUTE_NAME,
                    String::sprint("%g", inGroup.getTileSize())
                );
            }

            // Model
            XmlNode modelNode = foundGroupNode.getChild(Model::TAG);
            inGroup.getModel().saveTo(modelNode);

            // Textures
            inGroup.saveTextures(foundGroupNode);

            // Transform
            inGroup.saveTransform(foundGroupNode);
        }

        bool Mesh::hasSkeleton() const
        {
            return m_skeleton.isValid();
        }

        const AssetReference& Mesh::getSkeleton() const
        {
            return m_skeleton;
        }

        void Mesh::setSkeleton(const FileSystem::Path& inSource)
        {
            AssetReference reference;
            reference.setSource(inSource);
            setSkeleton(reference);
        }

        void Mesh::setSkeleton(const AssetReference& inValue)
        {
            m_skeleton = inValue;
            writeSkeleton();
        }

        void Mesh::fetchSkeleton()
        {
            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            const XmlNode skeletonNode = getXML().getChild(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                return;
            }

            m_skeleton.setFrom(skeletonNode, getFilepath());
        }

        void Mesh::writeSkeleton()
        {
            XmlNode root         = getXML();
            XmlNode skeletonNode = root.getChild(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                skeletonNode = root.prependChild(Skeleton::TAG);
            }

            m_skeleton.saveTo(skeletonNode);
        }

        const std::vector<AssetReference>& Mesh::getAnimations() const
        {
            return m_animations;
        }

        bool Mesh::hasAnimation(const FileSystem::Path& inSource) const
        {
            for (const AssetReference& animation : m_animations)
            {
                if (animation.getSource() == inSource)
                {
                    return true;
                }
            }

            return false;
        }

        void Mesh::setAnimations(const std::vector<AssetReference>& inAnimations)
        {
            m_animations = inAnimations;
            writeAnimations();
        }

        void Mesh::appendAnimation(const FileSystem::Path& inSource)
        {
            AssetReference reference;
            reference.setSource(inSource);
            appendAnimation(reference);
        }

        void Mesh::appendAnimation(const AssetReference& inValue)
        {
            if (!inValue.isValid() || hasAnimation(inValue.getSource()))
            {
                return;
            }

            m_animations.push_back(inValue);
            writeAnimations();
        }

        void Mesh::fetchAnimations()
        {
            m_animations.clear();

            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            for (const XmlNode& child : getXML().getChildren())
            {
                if (!String(child.getName()).equals(Animation::TAG))
                {
                    continue;
                }

                AssetReference reference;
                reference.setFrom(child, getFilepath());
                if (!reference.isValid())
                {
                    continue;
                }

                m_animations.push_back(reference);
            }
        }

        void Mesh::writeAnimations()
        {
            XmlNode root = getXML();
            for (XmlNode child = root.getFirstChild(); child;)
            {
                XmlNode next = child.getNextSibling();
                if (String(child.getName()).equals(Animation::TAG))
                {
                    root.removeChild(child);
                }

                child = next;
            }

            XmlNode after = root.getChild(Skeleton::TAG);
            for (const AssetReference& animation : m_animations)
            {
                XmlNode animationNode =
                    after ? root.insertChildAfter(Animation::TAG, after) : root.prependChild(Animation::TAG);
                animation.saveTo(animationNode);
                after = animationNode;
            }
        }

        void Mesh::fetchGroups()
        {
            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            for (const auto& groupNode : getXML().getChildren())
            {
                const String name = groupNode.getName();

                if (!name.equals(MeshGroup::TAG))
                {
                    continue;
                }

                MeshGroup group;
                group.setId(Xml::getAttribute(MeshGroup::ID_ATTRIBUTE_NAME, groupNode));
                group.setBone(Xml::getAttribute(MeshGroup::BONE_ATTRIBUTE_NAME, groupNode));
                group.setEmissiveStrength(groupNode.parseFloat(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME, 1.0f));
                group.setTileSize(groupNode.parseFloat(MeshGroup::TILE_SIZE_ATTRIBUTE_NAME, 0.0f));
                group.setTransform(groupNode);

                for (const auto& assetNode : groupNode.getChildren())
                {
                    const String currentTag = assetNode.getName();

                    if (currentTag.equals(Model::TAG))
                    {
                        group.setModel(assetNode, getFilepath());

                        continue;
                    }

                    const TextureMap textureMap = toTextureMap(currentTag);
                    if (textureMap != TextureMap::Count)
                    {
                        group.setTexture(textureMap, assetNode, getFilepath());

                        continue;
                    }
                }

                m_groups.push_back(group);
            }
        }
    }
}