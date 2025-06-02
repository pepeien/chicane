import os
import xml.etree.ElementTree as ET
import json
import re

def pascal_to_spaced(text, character = " "):
    if not text:
        return ""

    if len(text) > 1 and text[0].isupper() and text[1].isupper():
        prefix_end = 0
        for i in range(len(text) - 1):
            if text[i].isupper() and text[i + 1].islower():
                prefix_end = i
                break
            elif text[i].isupper() and i + 1 < len(text) - 1 and text[i + 1].isupper() and text[i + 2].islower():
                prefix_end = i + 1
                break
        
        if prefix_end > 0:
            prefix = text[:prefix_end + 1]
            remainder = text[prefix_end + 1:]

            if not remainder or not re.search(r'[A-Z]', remainder[1:]):
                return text

            spaced_remainder = re.sub(r'(?<!^)(?=[A-Z])', character, remainder)

            return prefix + spaced_remainder

    return re.sub(r'(?<!^)(?=[A-Z])', character, text)

def namespace_to_snake(text, character = "_"):
    if not text:
        return ""

    return text.replace("::", character).upper()

def insert_node(tree, parts, value, accumulated_path=""):
    part = parts[0]
    path = accumulated_path + part

    existing = next((item for item in tree if item.get("path") == path), None)

    if existing is None:
        existing = {
            "title": pascal_to_spaced(part),
            "path": path,
            "source": {
                "header": None,
                "namespace": None,
                "types": [],
                "enums": [],
                "constructors": [],
                "destructors": [],
                "functions": [],
                "members": [],
            },
            "children": []
        }
        tree.append(existing)

    if len(parts) == 1:
        existing.update(value)
    else:
        insert_node(existing["children"], parts[1:], value, path + "/")

    tree.sort(key=lambda x: x["title"])

def to_dict(tree, key, value, separator):
    if not key:
        return

    insert_node(tree, key.split(separator), value) 

def get_type(element):
    if element is None:
        return None

    text = "" if element.text is None else element.text.strip()

    reference = element.find("ref")
    if reference is None:
        return {
            "name": text,
            "path": ""
        }

    prefixes   = ""
    references = ""
    suffixes   = ""

    for node in element.iter():
        if node is element:
            if node.text:
                prefixes += node.text.strip()
        elif node.tag == "ref":
            text = node.text.strip() or ""

            if not text in base_systems:
                references += text

            if node.tail:
                suffixes += node.tail.strip()

    return {
        "name": prefixes + " " + references + suffixes,
        "path": references.replace("::", "/")
    }

def get_definition(refid):
    if refid is None:
        return None

    filepath = os.path.join(input_dir, f"{refid}.xml")
    if not os.path.isfile(filepath):
        return None

    root = ET.parse(filepath).getroot()
    if root is None:
        return None

    return root.find('compounddef')

def add_enum_definition(result, memberdef):
    name      = memberdef.find('name').text
    namespace = memberdef.find('qualifiedname').text
    typing    = memberdef.find('type')
    location  = memberdef.find('location')
    header    = location.attrib.get('file', '')

    values = []

    for enumvalue in memberdef.findall("enumvalue"):
        enum_name = enumvalue.find("name").text

        values.append(
            {
                "type": get_type(typing),
                "name": enum_name,
                "description": namespace_to_snake(namespace) + "_" + enum_name.upper() + "_DESCRIPTION"
            }
        )

    key  = header.split(include_dir)[-1].split('.')[0]
    data = {
        "title": pascal_to_spaced(namespace.split("::")[-1]),
        "source": {
            "header": header,
            "namespace": namespace,
            "types": [],
            "enums": values,
            "constructors": [],
            "destructors": [],
            "functions": [],
            "members": [],
            "description": namespace_to_snake(namespace) + "_DESCRIPTION"
        }
    }

    to_dict(result, key, data, "/")

def add_definition(result, refid):
    compounddef = get_definition(refid)
    if compounddef is None:
        return

    namespace = compounddef.find('compoundname').text
    if not base_namespace in namespace:
        return

    types        = []
    enums        = []
    constructors = []
    destructors  = []
    functions    = []
    members      = []
    description  = compounddef.find("briefdescription").find("para")

    name     = namespace.split("::")[-1]
    kind     = compounddef.attrib.get('kind', '')
    location = compounddef.find('location')
    header   = location.attrib.get('file', '')

    for sectiondef in compounddef.findall('sectiondef'):
        for memberdef in sectiondef.findall('memberdef'):
            inner_kind        = memberdef.attrib.get('kind', '')
            inner_typing      = memberdef.find('type')
            inner_name        = memberdef.find('name').text
            inner_namespace   = memberdef.find('qualifiedname').text
            inner_accessor    = memberdef.attrib.get("prot", 'public')
            inner_description = memberdef.find("briefdescription").find("para")

            inner_modifiers = []

            if memberdef.attrib.get("static", "no") == "yes":
                inner_modifiers.append("static")

            if memberdef.attrib.get("inline", "no") == "yes":
                inner_modifiers.append("inline")

            if memberdef.attrib.get("constexpr", "no") == "yes":
                inner_modifiers.append("constexpr")

            if memberdef.attrib.get("const", "no") == "yes":
                inner_modifiers.append("const")

            if inner_kind == "typedef":
                types.append(
                    {
                        "name": inner_name,
                        "type": get_type(inner_typing),
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION"
                    }
                )

            if inner_kind == "enum":
                enums.append(memberdef)

            if inner_kind in ["function", "friend"]:
                data = {
                    "accessor": inner_accessor,
                    "type": "standard",
                    "return": get_type(inner_typing),
                    "name": inner_name,
                    "parameters": [],
                    "modifiers": inner_modifiers,
                    "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION"
                }

                if  memberdef.attrib.get('virt', '') == 'virtual':
                    if memberdef.find("reimplements") is None:
                        data["type"] = "virtual"
                    else:
                        data["type"] = "override"

                for parameter in memberdef.findall("param"):
                    data["parameters"].append(
                        {
                            "type": get_type(parameter.find("type")),
                            "name": parameter.find("declname").text
                        }
                    )

                if inner_name in (name, "~" + name):
                    if "~" in inner_name:
                        destructors.append(data)    
                    else:
                        constructors.append(data)
                else:
                    functions.append(data)

            if inner_kind == "variable":
                members.append(
                    {
                        "accessor": inner_accessor,
                        "modifiers": inner_modifiers,
                        "type": get_type(inner_typing),
                        "name": inner_name,
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION"
                    }
                )

    key  = header.split(include_dir)[-1].split('.')[0]
    data = {
        "title": pascal_to_spaced(key.split('/')[-1]),
        "source": {
            "header": header,
            "namespace": namespace,
            "types": types,
            "enums": [],
            "constructors": constructors,
            "destructors": destructors,
            "functions": functions,
            "members": members,
            "description": namespace_to_snake(namespace) + "_DESCRIPTION"
        }
    }

    to_dict(result, key, data, "/")

    # Process enums later because they aren't treated as types
    for enum in enums:
        add_enum_definition(result, enum)

def generate_metadata(root):
    result = []

    for compound in root.findall("compound"):
        add_definition(result, compound.get("refid", None))

    return result

# Values
base_namespace = "Chicane"
base_systems = [
    "CHICANE_BOX",
    "CHICANE_CORE",
    "CHICANE_GRID",
    "CHICANE_RUNTIME"
]

include_dir = "Includes/Chicane/"

input_dir  = os.path.join(".docs", "xml")
output_dir = os.path.join(".docs")

# Output
with open(os.path.join(output_dir, "metadata.json"), "w", encoding="utf-8") as f:
    json.dump(
        generate_metadata(ET.parse(os.path.join(input_dir, 'index.xml')).getroot()),
        f,
        indent=4,
        ensure_ascii=False
    )