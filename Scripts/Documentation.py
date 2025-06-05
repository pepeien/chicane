import argparse
import copy
import os
import json
import re
import xml.etree.ElementTree as ET

def get_type_def(type_elem: ET.Element):
    parts = []
    ref_elements = []

    if type_elem.text and type_elem.text.strip():
        if not type_elem.text in base_systems:
            parts.append((None, type_elem.text))
    
    for child in type_elem:
        if child.tag == 'ref':
            refid = child.get('refid')

            if child.text and not child.text in base_systems:
                parts.append((refid, child.text))

            if child.tail and child.tail.strip():
                parts.append((None, child.tail))
        else:
            pass

    full_type = ''.join(text for (_, text) in parts)
    parsed = parse_type(full_type)

    current_pos = 0

    for refid, text in parts:
        if refid is None:
            current_pos += len(text)

            continue

        text_len = len(text)
        match_pos = full_type.find(text, current_pos)
        
        if match_pos != -1:
            assign_refid_to_component(parsed, text, refid, match_pos)

            current_pos = match_pos + text_len
    
    return parsed

def assign_refid_to_component(parsed, text: str, refid: str, pos: int):
    if parsed["name"] == text:
        parsed["path"] = refid

        return

    for template in parsed["templates"]:
        template_text = template["prefix"] + template["name"] + template["suffix"]

        if text in template_text:
            assign_refid_to_component(template, text, refid, pos)

            return

def parse_type(type_str: str):
    type_str = type_str.strip()
    result = {
        "prefix": "",
        "name": "",
        "suffix": "",
        "path": "",
        "templates": []
    }

    for prefix in ['const ', 'volatile ', 'static ', 'mutable ', 'extern ']:
        if type_str.startswith(prefix):
            result["prefix"] = prefix
            type_str = type_str[len(prefix):].lstrip()
            break

    suffix_parts = []
    while True:
        suffix_part = None
        for suffix in ['*', '**', '&', '&&']:
            if type_str.endswith(suffix):
                suffix_part = suffix
                type_str = type_str[:-len(suffix)].rstrip()
                suffix_parts.insert(0, suffix_part)
                break

        if type_str.endswith(']'):
            end = type_str.rfind('[')
            if end != -1:
                suffix_part = type_str[end:]
                type_str = type_str[:end].rstrip()
                suffix_parts.insert(0, suffix_part)
                continue
        if not suffix_part:
            break
    result["suffix"] = ''.join(suffix_parts)

    template_start = type_str.find('<')
    if template_start != -1:
        result["name"] = type_str[:template_start].strip()

        depth = 1
        template_end = template_start + 1
        while template_end < len(type_str) and depth > 0:
            if type_str[template_end] == '<':
                depth += 1
            elif type_str[template_end] == '>':
                depth -= 1
            template_end += 1
        
        if depth == 0:
            template_args_str = type_str[template_start+1:template_end-1]
            result["templates"] = split_template_args(template_args_str)
        else:
            result["name"] = type_str.strip()
    else:
        result["name"] = type_str.strip()
    
    return result

def split_template_args(args_str: str):
    args = []
    current_arg = []
    depth = 0

    for char in args_str:
        if char == '<':
            depth += 1
            current_arg.append(char)
        elif char == '>':
            depth -= 1
            current_arg.append(char)
        elif char == ',' and depth == 0:
            args.append(''.join(current_arg).strip())
            current_arg = []
        else:
            current_arg.append(char)

    if current_arg:
        args.append(''.join(current_arg).strip())
    
    return [parse_type(arg) for arg in args if arg.strip()]

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

def cache_reference(component, path):
    refid = component.attrib.get('id', '')
    if not refid:
        return

    references[refid] = path

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
                "type": get_type_def(typing),
                "name": enum_name,
                "description": namespace_to_snake(namespace) + "_" + enum_name.upper() + "_DESCRIPTION"
            }
        )

    key  = header.split(include_dir)[-1].split('.')[0]
    data = {
        "title": pascal_to_spaced(name),
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

    cache_reference(memberdef, key)

def add_definition(result, compounddef):
    if compounddef is None:
        return

    namespace = compounddef.find('compoundname').text
    if not base_namespace in namespace:
        return

    kind     = compounddef.attrib.get('kind', '')
    name     = namespace.split("::")[-1]
    location = compounddef.find('location')
    header   = location.attrib.get('file', '').split(include_dir)[-1]
    filename = header.split(".")[0].split("/")[-1]

    types        = []
    enums        = []
    constructors = []
    destructors  = []
    functions    = []
    members      = []

    key = header.split(".")[0]

    for sectiondef in compounddef.findall('sectiondef'):
        for memberdef in sectiondef.findall('memberdef'):
            inner_id          = memberdef.attrib.get('id', '')
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
                        "type": get_type_def(inner_typing),
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION"
                    }
                )

                cache_reference(memberdef, key)

            if inner_kind == "enum":
                add_enum_definition(result, memberdef)

            if inner_kind in ["function", "friend"]:
                data = {
                    "accessor": inner_accessor,
                    "type": "standard",
                    "return": get_type_def(inner_typing),
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
                            "type": get_type_def(parameter.find("type")),
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
                        "type": get_type_def(inner_typing),
                        "name": inner_name,
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION"
                    }
                )

    if len(types) > 0 or len(constructors) > 0 or len(destructors) > 0 or len(functions) > 0 or len(members) > 0:
        data = {
            "title": pascal_to_spaced(filename),
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

        cache_reference(compounddef, key)

    for inner in compounddef.findall("innerclass") + compounddef.findall("innerstruct"):
        inner_refid = inner.attrib.get("refid", None)
        if not inner_refid:
            continue

        add_definition(result, get_definition(inner_refid))

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

def add_namespace_definition(result, refid):
    compounddef = get_definition(refid)
    if compounddef is None:
        return

    namespace = compounddef.find('compoundname').text
    if not base_namespace in namespace:
        return

    kind = compounddef.attrib.get('kind', '')
    if not kind == "namespace":
        return

    add_definition(result, compounddef)

def generate_metadata(root):
    result = []

    for compound in root.findall("compound"):
        add_namespace_definition(result, compound.get("refid", None))

    return result

def generate_index(result, metadata):
    for data in metadata:
        to_dict(
            result,
            data["path"],
            {
                "title": data["title"],
                "source": {
                    "header": data["source"]["header"],
                    "namespace": data["source"]["namespace"]
                },
                "children": []
            },
            "/"
        )

        generate_index(result, data["children"])

def add_index_file(metadata):
    result = []

    generate_index(result, metadata)

    os.makedirs(output_dir, exist_ok=True)
    with open(os.path.join(output_dir, "index.json"), "w", encoding="utf-8") as f:
        json.dump(result, f, indent=4, ensure_ascii=False)

def get_reference_path(refid):
    return references.get(refid) or refid

def add_reference_path(typedef, root):
    if typedef is None:
        return

    path = typedef["path"]
    if path:
        typedef["path"] = get_reference_path(path)

    for child_typedef in typedef["templates"]:
        add_reference_path(child_typedef, root)

def add_reference_paths(metadata, root):
    source = metadata["source"]
    if source is None:
        return

    for typedef in source["types"]:
        add_reference_path(typedef["type"], root)

    for function in source["constructors"] + source["destructors"] + source["functions"]:
        for parameter in function["parameters"]:
            add_reference_path(parameter["type"], root)

        add_reference_path(function["return"], root)

    for member in source["members"]:
        add_reference_path(member["type"], root)

def add_references_paths(metadata, root):
    for data in metadata:
        add_reference_paths(data, root)
        add_references_paths(data["children"], root)

def add_definition_files(metadata, root):
    for data in metadata:
        payload = copy.deepcopy(data)
        payload["children"] = []

        path = data["path"]
        os.makedirs(os.path.join(output_dir, *path.split("/")[:-1]), exist_ok=True)
        with open(os.path.join(output_dir, path + ".json"), "w", encoding="utf-8") as f:
            json.dump(payload, f, indent=4, ensure_ascii=False)

        add_definition_files(data["children"], root)

# Setup
parser = argparse.ArgumentParser(description='My script description')
parser.add_argument('--input',     '-i', help='Input filename')
parser.add_argument('--output',    '-o', help='Output folder')
parser.add_argument('--directory', '-d', help='Include folder')
parser.add_argument('--namespace', '-n', help='Base namespace')
parser.add_argument('--systems',   '-s', help='Base systems')

args = parser.parse_args()

# Values
input_dir = os.path.normpath(args.input)
if not os.path.exists(input_dir):
    raise Exception(f"Input directory [{input_dir}] doesn't exist") 

index_file = os.path.join(input_dir, 'index.xml')
if not os.path.exists(index_file):
    raise Exception(f"Index file [{index_file}] doesn't exist") 

output_dir = os.path.normpath(args.output)

include_dir = args.directory or ""

base_namespace = args.namespace or ""
base_systems   = (args.systems or "").split(",")

print(f"Input Folder:  {input_dir}")
print(f"Output Folder: {output_dir}")
print(f"Directory:     {include_dir}")
print(f"Namespace:     {base_namespace}")
print(f"Systems:       {base_systems}")

## Output
references = {}
metadata   = generate_metadata(ET.parse(index_file).getroot())

add_index_file(metadata)
add_references_paths(metadata, metadata)
add_definition_files(metadata, metadata)
