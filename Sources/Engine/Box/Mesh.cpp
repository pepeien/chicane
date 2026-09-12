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
            pugi::xml_node root = getXML();
            for (pugi::xml_node child = root.first_child(); child;)
            {
                pugi::xml_node next = child.next_sibling();
                const String   name = child.name();
                if (!name.equals(AssetPreview::TAG) && !name.equals(Skeleton::TAG) && !name.equals(Animation::TAG))
                {
                    root.remove_child(child);
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

            pugi::xml_node root = getXML();

            if (!Xml::isEmpty(root.find_child_by_attribute(GROUP_ID_ATTRIBUTE_NAME, id.toChar())))
            {
                throw std::runtime_error("A group with the ID " + inGroup.getId().toStandard() + " already exists");
            }

            pugi::xml_node groupNode = root.append_child(GROUP_TAG);
            groupNode.append_attribute(GROUP_ID_ATTRIBUTE_NAME).set_value(id.toStandard());
            if (!inGroup.getBone().isEmpty())
            {
                groupNode.append_attribute(GROUP_BONE_ATTRIBUTE_NAME).set_value(inGroup.getBone().toStandard());
            }
            if (inGroup.getEmissiveStrength() != 1.0f)
            {
                groupNode.append_attribute(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME)
                    .set_value(inGroup.getEmissiveStrength());
            }

            // Model
            pugi::xml_node modelNode = groupNode.append_child(Model::TAG);
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

            pugi::xml_node root           = getXML();
            pugi::xml_node foundGroupNode = root.find_child_by_attribute(GROUP_ID_ATTRIBUTE_NAME, id.toChar());

            if (Xml::isEmpty(foundGroupNode))
            {
                appendGroup(inGroup);

                return;
            }

            pugi::xml_attribute boneAttribute = foundGroupNode.attribute(GROUP_BONE_ATTRIBUTE_NAME);
            if (inGroup.getBone().isEmpty())
            {
                if (!boneAttribute.empty())
                {
                    foundGroupNode.remove_attribute(boneAttribute);
                }
            }
            else if (boneAttribute.empty())
            {
                foundGroupNode.append_attribute(GROUP_BONE_ATTRIBUTE_NAME).set_value(inGroup.getBone().toStandard());
            }
            else
            {
                boneAttribute.set_value(inGroup.getBone().toStandard());
            }

            pugi::xml_attribute strengthAttribute =
                foundGroupNode.attribute(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME);
            if (inGroup.getEmissiveStrength() == 1.0f)
            {
                if (!strengthAttribute.empty())
                {
                    foundGroupNode.remove_attribute(strengthAttribute);
                }
            }
            else if (strengthAttribute.empty())
            {
                foundGroupNode.append_attribute(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME)
                    .set_value(inGroup.getEmissiveStrength());
            }
            else
            {
                strengthAttribute.set_value(inGroup.getEmissiveStrength());
            }

            // Model
            pugi::xml_node modelNode = foundGroupNode.child(Model::TAG);
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

            const pugi::xml_node skeletonNode = getXML().child(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                return;
            }

            m_skeleton.setFrom(skeletonNode, getFilepath());
        }

        void Mesh::writeSkeleton()
        {
            pugi::xml_node root         = getXML();
            pugi::xml_node skeletonNode = root.child(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                skeletonNode = root.prepend_child(Skeleton::TAG);
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

            for (const pugi::xml_node& child : getXML().children())
            {
                if (!String(child.name()).equals(Animation::TAG))
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
            pugi::xml_node root = getXML();
            for (pugi::xml_node child = root.first_child(); child;)
            {
                pugi::xml_node next = child.next_sibling();
                if (String(child.name()).equals(Animation::TAG))
                {
                    root.remove_child(child);
                }

                child = next;
            }

            pugi::xml_node after = root.child(Skeleton::TAG);
            for (const AssetReference& animation : m_animations)
            {
                pugi::xml_node animationNode =
                    after ? root.insert_child_after(Animation::TAG, after) : root.prepend_child(Animation::TAG);
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

            for (const auto& groupNode : getXML().children())
            {
                const String name = groupNode.name();

                if (!name.equals(GROUP_TAG))
                {
                    continue;
                }

                MeshGroup group;
                group.setId(Xml::getAttribute(GROUP_ID_ATTRIBUTE_NAME, groupNode).as_string());
                group.setBone(Xml::getAttribute(GROUP_BONE_ATTRIBUTE_NAME, groupNode).as_string());
                group.setEmissiveStrength(
                    Xml::getAttribute(MeshGroup::EMISSIVE_STRENGTH_ATTRIBUTE_NAME, groupNode).as_float(1.0f)
                );
                group.setTransform(groupNode);

                for (const auto& assetNode : groupNode.children())
                {
                    const String currentTag = assetNode.name();

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