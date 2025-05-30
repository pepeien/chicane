import os
import xml.etree.ElementTree as ET
import json
import re

def pascal_to_spaced(text):
    if not text:
        return text

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

            spaced_remainder = re.sub(r'(?<!^)(?=[A-Z])', ' ', remainder)

            return prefix + spaced_remainder

    return re.sub(r'(?<!^)(?=[A-Z])', ' ', text)

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

def add_definition(result, compounddef):
    constructors = []
    destructors  = []
    functions    = []
    members      = []

    for sectiondef in compounddef.findall('sectiondef'):
        kind = sectiondef.attrib.get('kind', '')

        for memberdef in sectiondef.findall('memberdef'):
            typing      = memberdef.find('type').text
            name        = memberdef.find('name').text
            accessor    = memberdef.attrib.get("prot", 'public')

            modifiers = []

            if memberdef.attrib.get("static", "no") == "yes":
                modifiers.append("static")

            if memberdef.attrib.get("inline", "no") == "yes":
                modifiers.append("inline")

            if memberdef.attrib.get("constexpr", "no") == "yes":
                modifiers.append("constexpr")

            if memberdef.attrib.get("const", "no") == "yes":
                modifiers.append("const")

            if kind in ["function", "friend", "public-func", "protected-func", "private-func"]:
                args = re.search(r"\((.*?)\)", memberdef.find('argsstring').text).group(1)

                data = {
                    "accessor": accessor,
                    "return": typing,
                    "name": name,
                    "args": [],
                    "modifiers": modifiers
                }

                if not args == "":
                    data["args"] = [arg.strip().split("=")[0] for arg in args.split(",")]

                if typing is None:
                    if "~" in name:
                        destructors.append(data)    
                    else:
                        constructors.append(data)
                else:
                    functions.append(data)

            if kind in ["public-var", "protected-var", "private-var"]:
                initializer = memberdef.find('initializer')

                members.append(
                    {
                        "accessor": accessor,
                        "modifiers": modifiers,
                        "type": typing,
                        "name": name,
                        "initializer": initializer.text.split("=")[-1]
                    }
                )

    namespace = compounddef.find('compoundname').text
    location  = compounddef.find('location')
    header    = location.attrib.get('file', '')

    key  = header.split('Includes/Chicane/')[-1].split('.')[0]
    data = {
        "title": pascal_to_spaced(namespace.split('::')[-1]),
        "source": {
            "header": header,
            "namespace": namespace,
            "constructors": constructors,
            "destructors": destructors,
            "functions": functions,
            "members": members
        }
    }

    to_dict(result, key, data, "/")

def add_metadata(result, refid):
    if refid is None:
        return

    filepath = os.path.join(input_dir, f"{refid}.xml")
    if not os.path.isfile(filepath):
        return

    root = ET.parse(filepath).getroot()
    if root is None:
        return

    compounddef = root.find('compounddef')
    if compounddef is None:
        return

    kind = compounddef.attrib.get('kind', '')
    if not kind in ["class", "struct", "enum", "interface"]:
        return

    add_definition(result, compounddef)

def generate_metadata(root):
    result = []

    namespaces = []
    types      = []

    for compound in root.findall("compound"):
        add_metadata(result, compound.attrib.get("refid", None))

    return result

# Values
input_dir  = os.path.join("..", ".docs", "xml")
output_dir = os.path.join("..", ".docs")

# Output
with open(os.path.join(output_dir, "metadata.json"), "w", encoding="utf-8") as f:
    json.dump(
        generate_metadata(ET.parse(os.path.join(input_dir, 'index.xml')).getroot()),
        f,
        indent=4,
        ensure_ascii=False
    )