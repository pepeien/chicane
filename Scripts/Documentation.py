import copy
import os
import json
import re
import xml.etree.ElementTree as ET

prefix_patterns = ["const", "static", "volatile", "mutable"]
suffix_patterns = ["*", "&", "**", "&&", "[]"]

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

def get_value_between(text, open_char = "(", close_char = ")"):
    if not text:
        return None

    start = text.find(open_char)
    if start == -1:
        return None

    end = text.find(close_char, start)
    if end == -1:
        return None

    nesting_level = 0
    for i in range(start, len(text)):
        if text[i] == open_char:
            nesting_level += 1

        elif text[i] == close_char:
            nesting_level -= 1

            if nesting_level == 0:
                return text[start + 1:i].strip()
    
    return None

def type_to_string(element):
    if element is None:
        return []
    
    result = []

    for child in element.iter():
        if child is element:
            if child.text:
                text = child.text.strip()

                if text and not text in base_systems:
                    result.append(text)
        elif child.tag == "ref":
            if child.text:
                text = child.text.strip()

                if text and not text in base_systems:
                    result.append(text)

        if child.tail:
            text = child.tail.strip()

            if text and not text in base_systems:
                result.append(text)

    return " ".join(" ".join(result).split())

def get_type(text):
    result = {
        "prefix": "",
        "name": "",
        "suffix": "",
        "path": "",
        "templates": []
    }

    if text:
        prefix    = ""
        name      = ""
        suffix    = ""
        templates = []

        if text:
            splitted_text = text.split(" ")

            if len(splitted_text) == 1:
                name = text.strip()
            elif len(splitted_text) == 2:
                valueA = splitted_text[0].strip()
                valueB = splitted_text[1].strip()

                prefix = valueA if valueA in prefix_patterns else valueB if valueB in prefix_patterns else ""
                suffix = valueA if valueA in suffix_patterns else valueB if valueB in suffix_patterns else ""
                name   = valueA if valueB in [prefix, suffix] else valueB
            elif len(splitted_text) >= 3:
                prefix_index     = 0 if splitted_text[0] in prefix_patterns else None
                suffix_index     = -1 if splitted_text[-1] in suffix_patterns else None
                name_index_start = 0 if prefix_index is None else 1
                name_index_end   = None if suffix_index is None else -1

                prefix = "" if prefix_index is None else splitted_text[prefix_index].strip()
                name   = " ".join(splitted_text[name_index_start:name_index_end]).strip()
                suffix = "" if suffix_index is None else splitted_text[suffix_index].strip()

        result["prefix"] = prefix.strip()
        result["name"]   = name.split("<")[0].strip()
        result["suffix"] = suffix.strip()

        template_text = get_value_between(name, "<", ">")
        if template_text:
            if "<" in template_text or ">" in template_text:
                templates.append(get_type(template_text))
            else:
                for typename in template_text.split(","):
                    templates.append(get_type(typename))
        
        result["templates"] = templates

    return result

def set_type_def_ref(result, component):
    ref = component.find("ref")
    if ref is None:
        return

    if ref.text == result["name"]:
        result["path"] = ref.attrib.get("refid", None)

    for child in result["templates"]:
        set_type_def_ref(child, ref)

def get_type_def(typedef):
    result = get_type(type_to_string(typedef))

    set_type_def_ref(result, typedef)

    return result

def cache_reference(component, path):
    refid = component.attrib.get('id', '')
    if not refid:
        return

    ref_map[refid] = path

def add_definition(result, compounddef):
    if compounddef is None:
        return

    namespace = compounddef.find('compoundname').text
    if not base_namespace in namespace:
        return

    kind     = compounddef.attrib.get('kind', '')
    name     = namespace.split("::")[-1]
    location = compounddef.find('location')
    header   = location.attrib.get('file', '')

    types        = []
    enums        = []
    constructors = []
    destructors  = []
    functions    = []
    members      = []

    normalized_header = header.split(include_dir)[-1].split(".")[0].split("/")
    if not normalized_header[-1] == name:
        normalized_header.append(name)

    key = "/".join(normalized_header)

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

    data = {
        "title": pascal_to_spaced(name),
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

        print(inner_refid)
        add_definition(result, get_definition(inner_refid))

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
    return ref_map.get(refid) or refid

def add_type_path(typedef, root):
    if typedef is None:
        return

    path = typedef["path"]
    if path:
        typedef["path"] = get_reference_path(path)

    for child_typedef in typedef["templates"]:
        add_type_path(child_typedef, root)

def add_type_paths(metadata, root):
    source = metadata["source"]
    if source is None:
        return

    for typedef in source["types"]:
        add_type_path(typedef["type"], root)

    for function in source["constructors"] + source["destructors"] + source["functions"]:
        for parameter in function["parameters"]:
            add_type_path(parameter["type"], root)

        add_type_path(function["return"], root)

    for member in source["members"]:
        add_type_path(member["type"], root)

def add_types_paths(metadata, root):
    for data in metadata:
        add_types_paths(data["children"], root)
        add_type_paths(data, root)

def add_definition_files(metadata, root):
    for data in metadata:
        payload = copy.deepcopy(data)
        payload["children"] = []

        path = data["path"]
        os.makedirs(os.path.join(output_dir, *path.split("/")[:-1]), exist_ok=True)
        with open(os.path.join(output_dir, path + ".json"), "w", encoding="utf-8") as f:
            json.dump(payload, f, indent=4, ensure_ascii=False)

        add_definition_files(data["children"], root)

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
output_dir = os.path.join(".docs", "json")

# Output
ref_map  = {}

metadata = generate_metadata(ET.parse(os.path.join(input_dir, 'index.xml')).getroot())

add_index_file(metadata)
add_types_paths(metadata, metadata)
add_definition_files(metadata, metadata)