import argparse
import copy
import os
import json
import xml.etree.ElementTree as ET


def get_type_def(type_elem: ET.Element):
    parts = []

    if type_elem.text and type_elem.text.strip():
        if type_elem.text not in base_systems:
            parts.append((None, type_elem.text))

    for child in type_elem:
        if child.tag == "ref":
            refid = child.get("refid")

            if child.text and child.text not in base_systems:
                parts.append((refid, child.text))

            if child.tail and child.tail.strip():
                parts.append((None, child.tail))

    full_type = "".join(text for (_, text) in parts)
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
        "templates": [],
    }

    for prefix in ["const ", "volatile ", "static ", "mutable ", "extern "]:
        if type_str.startswith(prefix):
            result["prefix"] = prefix
            type_str = type_str[len(prefix) :].lstrip()
            break

    suffix_parts = []
    while True:
        suffix_part = None
        for suffix in ["*", "**", "&", "&&"]:
            if type_str.endswith(suffix):
                suffix_part = suffix
                type_str = type_str[: -len(suffix)].rstrip()
                suffix_parts.insert(0, suffix_part)
                break

        if type_str.endswith("]"):
            end = type_str.rfind("[")
            if end != -1:
                suffix_part = type_str[end:]
                type_str = type_str[:end].rstrip()
                suffix_parts.insert(0, suffix_part)
                continue
        if not suffix_part:
            break
    result["suffix"] = "".join(suffix_parts)

    template_start = type_str.find("<")
    if template_start != -1:
        result["name"] = type_str[:template_start].strip()

        depth = 1
        template_end = template_start + 1
        while template_end < len(type_str) and depth > 0:
            if type_str[template_end] == "<":
                depth += 1
            elif type_str[template_end] == ">":
                depth -= 1
            template_end += 1

        if depth == 0:
            template_args_str = type_str[template_start + 1 : template_end - 1]
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
        if char == "<":
            depth += 1
            current_arg.append(char)
        elif char == ">":
            depth -= 1
            current_arg.append(char)
        elif char == "," and depth == 0:
            args.append("".join(current_arg).strip())
            current_arg = []
        else:
            current_arg.append(char)

    if current_arg:
        args.append("".join(current_arg).strip())

    return [parse_type(arg) for arg in args if arg.strip()]


def pascal_to_spaced(text, character=" "):
    if not text:
        return ""
    return text


def namespace_to_snake(text, character="_"):
    if not text:
        return ""
    return text.replace("::", character).upper()


def normalize_slashes(path: str) -> str:
    return path.replace("\\", "/")


def header_to_key(header: str) -> str:
    header = normalize_slashes(header)
    markers = [
        normalize_slashes(include_dir),
        "Includes/Chicane/",
        "Chicane/",
    ]

    for marker in markers:
        if not marker:
            continue
        if marker in header:
            return header.split(marker, 1)[-1].rsplit(".", 1)[0]

    basename = os.path.basename(header)
    return os.path.splitext(basename)[0]


def empty_source(header=None, namespace=None):
    return {
        "header": header,
        "namespace": namespace,
        "types": [],
        "enums": [],
        "constructors": [],
        "destructors": [],
        "functions": [],
        "members": [],
        "description": None,
    }


def make_node(path: str, header=None, namespace=None):
    title = path.split("/")[-1]
    return {
        "title": pascal_to_spaced(title),
        "filename": pascal_to_spaced(title),
        "path": path,
        "source": empty_source(header, namespace),
        "children": [],
    }


def merge_node(existing, value, replace_namespace=False):
    for key, val in value.items():
        if key != "source":
            if val is not None:
                existing[key] = val
            continue

        for src_key, src_val in val.items():
            if isinstance(src_val, list):
                existing["source"][src_key].extend(src_val)
            elif src_key == "namespace":
                # Member FQ names must not overwrite a class/enum namespace.
                if src_val is None:
                    continue
                if replace_namespace or existing["source"].get("namespace") is None:
                    existing["source"]["namespace"] = src_val
            elif src_val is not None:
                existing["source"][src_key] = src_val


def add_node(tree, parts, value, accumulated_path="", replace_namespace=False):
    part = parts[0]
    path = accumulated_path + part

    existing = next((item for item in tree if item.get("path") == path), None)
    if existing is None:
        existing = make_node(path)
        tree.append(existing)

    if len(parts) == 1:
        merge_node(existing, value, replace_namespace=replace_namespace)
    else:
        add_node(
            existing["children"],
            parts[1:],
            value,
            path + "/",
            replace_namespace=replace_namespace,
        )

    tree.sort(key=lambda x: x["title"])


def add_to_tree(tree, path, value, separator="/", replace_namespace=False):
    if not path:
        return
    add_node(tree, path.split(separator), value, replace_namespace=replace_namespace)


def namespace_from_path(path: str) -> str:
    if not path:
        return base_namespace
    return base_namespace + "::" + path.replace("/", "::")


def fill_missing_namespaces(nodes):
    for node in nodes:
        source = node.get("source") or {}
        if source.get("header") and not source.get("namespace"):
            source["namespace"] = namespace_from_path(node.get("path", ""))
        fill_missing_namespaces(node.get("children") or [])


def ensure_header_node(tree, key: str, header: str):
    add_to_tree(
        tree,
        key,
        {
            "title": pascal_to_spaced(key.split("/")[-1]),
            "filename": pascal_to_spaced(key.split("/")[-1]),
            "source": {
                "header": header,
                "namespace": None,
                "types": [],
                "enums": [],
                "constructors": [],
                "destructors": [],
                "functions": [],
                "members": [],
                "description": None,
            },
        },
        "/",
    )


def seed_headers(tree):
    root = os.path.normpath(headers_dir)
    if not os.path.isdir(root):
        raise Exception(f"Headers directory [{root}] doesn't exist")

    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            if not filename.endswith(".hpp"):
                continue

            absolute = os.path.join(dirpath, filename)
            relative = normalize_slashes(os.path.relpath(absolute, root))
            key = relative.rsplit(".", 1)[0]
            header = normalize_slashes(os.path.join(include_dir, relative))
            ensure_header_node(tree, key, header)


def add_reference(component, path):
    refid = component.attrib.get("id", "")
    if not refid:
        return
    references[refid] = path


def get_reference(refid):
    if refid is None:
        return None

    filepath = os.path.join(input_dir, f"{refid}.xml")
    if not os.path.isfile(filepath):
        return None

    root = ET.parse(filepath).getroot()
    if root is None:
        return None

    return root.find("compounddef")


def member_header(memberdef, fallback_header: str) -> str:
    location = memberdef.find("location")
    if location is None:
        return fallback_header
    return location.attrib.get("file", fallback_header) or fallback_header


def append_source_list(tree, key: str, header: str, list_name: str, item, namespace=None):
    ensure_header_node(tree, key, header)
    add_to_tree(
        tree,
        key,
        {
            "source": {
                "header": header,
                "namespace": namespace,
                list_name: [item],
                "description": namespace_to_snake(namespace) + "_DESCRIPTION" if namespace else None,
            }
        },
        "/",
        replace_namespace=False,
    )


def add_enum_definition(result, memberdef):
    title = memberdef.find("name").text
    namespace = memberdef.find("qualifiedname").text
    typing = memberdef.find("type")
    header = member_header(memberdef, "")
    key = header_to_key(header)
    filename = key.split("/")[-1]

    values = []
    for enumvalue in memberdef.findall("enumvalue"):
        enum_name = enumvalue.find("name").text
        values.append(
            {
                "type": get_type_def(typing),
                "name": enum_name,
                "description": namespace_to_snake(namespace) + "_" + enum_name.upper() + "_DESCRIPTION",
            }
        )

    data = {
        "title": pascal_to_spaced(title),
        "filename": pascal_to_spaced(filename),
        "source": {
            "header": normalize_slashes(header) if header else None,
            "namespace": namespace,
            "types": [],
            "enums": values,
            "constructors": [],
            "destructors": [],
            "functions": [],
            "members": [],
            "description": namespace_to_snake(namespace) + "_DESCRIPTION",
        },
    }

    add_to_tree(result, key, data, "/", replace_namespace=True)
    add_reference(memberdef, key)


def add_definition(result, compounddef):
    if compounddef is None:
        return

    namespace = compounddef.find("compoundname").text
    if base_namespace not in namespace:
        return

    kind = compounddef.attrib.get("kind", "")
    title = namespace.split("::")[-1]
    location = compounddef.find("location")
    header = location.attrib.get("file", "") if location is not None else ""
    compound_key = header_to_key(header) if header else title
    filename = compound_key.split("/")[-1]

    print(namespace, header)

    # Class/struct compounds own their file node. Namespace compounds only
    # distribute members into the headers where those members were declared.
    owns_file = kind in ["class", "struct"]
    file_namespace = namespace
    if owns_file:
        ensure_header_node(result, compound_key, normalize_slashes(header))
        add_to_tree(
            result,
            compound_key,
            {
                "title": pascal_to_spaced(title),
                "filename": pascal_to_spaced(filename),
                "source": {
                    "header": normalize_slashes(header),
                    "namespace": namespace,
                    "description": namespace_to_snake(namespace) + "_DESCRIPTION",
                },
            },
            "/",
            replace_namespace=True,
        )
        add_reference(compounddef, compound_key)

    for sectiondef in compounddef.findall("sectiondef"):
        for memberdef in sectiondef.findall("memberdef"):
            inner_kind = memberdef.attrib.get("kind", "")
            inner_typing = memberdef.find("type")
            inner_name = memberdef.find("name").text
            inner_namespace = memberdef.find("qualifiedname").text
            inner_accessor = memberdef.attrib.get("prot", "public")

            inner_modifiers = []
            if memberdef.attrib.get("static", "no") == "yes":
                inner_modifiers.append("static")
            if memberdef.attrib.get("inline", "no") == "yes":
                inner_modifiers.append("inline")
            if memberdef.attrib.get("constexpr", "no") == "yes":
                inner_modifiers.append("constexpr")
            if memberdef.attrib.get("const", "no") == "yes":
                inner_modifiers.append("const")

            declared_header = member_header(memberdef, header)
            declared_key = header_to_key(declared_header) if declared_header else compound_key
            target_key = compound_key if owns_file else declared_key
            target_header = normalize_slashes(header if owns_file else declared_header)

            if inner_kind == "typedef":
                append_source_list(
                    result,
                    declared_key,
                    normalize_slashes(declared_header),
                    "types",
                    {
                        "name": inner_name,
                        "type": get_type_def(inner_typing),
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION",
                    },
                    file_namespace,
                )
                add_reference(memberdef, declared_key)
                continue

            if inner_kind == "enum":
                add_enum_definition(result, memberdef)
                continue

            if inner_kind in ["function", "friend"]:
                data = {
                    "accessor": inner_accessor,
                    "type": "standard",
                    "return": get_type_def(inner_typing),
                    "name": inner_name,
                    "parameters": [],
                    "modifiers": inner_modifiers,
                    "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION",
                }

                if memberdef.attrib.get("virt", "") == "virtual":
                    if memberdef.find("reimplements") is None:
                        data["type"] = "virtual"
                    else:
                        data["type"] = "override"

                for parameter in memberdef.findall("param"):
                    if parameter.find("declname") is not None:
                        data["parameters"].append(
                            {
                                "type": get_type_def(parameter.find("type")),
                                "name": parameter.find("declname").text,
                            }
                        )

                list_name = "functions"
                if owns_file and inner_name in (title, "~" + title):
                    list_name = "destructors" if "~" in inner_name else "constructors"

                append_source_list(
                    result,
                    target_key,
                    target_header,
                    list_name,
                    data,
                    file_namespace,
                )
                continue

            if inner_kind == "variable":
                append_source_list(
                    result,
                    target_key,
                    target_header,
                    "members",
                    {
                        "accessor": inner_accessor,
                        "modifiers": inner_modifiers,
                        "type": get_type_def(inner_typing),
                        "name": inner_name,
                        "description": namespace_to_snake(inner_namespace) + "_DESCRIPTION",
                    },
                    file_namespace,
                )


def generate_metadata(root):
    result = []
    seed_headers(result)

    for compound in root.findall("compound"):
        refid = compound.get("refid", None)
        if refid is None:
            continue

        compounddef = get_reference(refid)
        if compounddef is None:
            continue

        namespace = compounddef.find("compoundname").text
        if base_namespace not in namespace:
            continue

        kind = compounddef.attrib.get("kind", "")
        if kind not in ["namespace", "class", "struct"]:
            continue

        add_definition(result, compounddef)

    fill_missing_namespaces(result)
    return result


def generate_index(result, metadata):
    for data in metadata:
        add_to_tree(
            result,
            data["path"],
            {
                "title": data.get("title"),
                "filename": data.get("filename"),
                "source": {
                    "header": data.get("source").get("header"),
                    "namespace": data.get("source").get("namespace"),
                },
                "children": [],
            },
            "/",
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
parser = argparse.ArgumentParser(description="Generate Chicane documentation JSON references")
parser.add_argument("--input", "-i", help="Input XML folder")
parser.add_argument("--output", "-o", help="Output folder")
parser.add_argument("--directory", "-d", help="Include folder prefix used in Doxygen paths")
parser.add_argument("--headers", "-H", help="Headers root to seed the documentation tree from")
parser.add_argument("--namespace", "-n", help="Base namespace")
parser.add_argument("--systems", "-s", help="Base systems")

args = parser.parse_args()

input_dir = os.path.normpath(args.input)
if not os.path.exists(input_dir):
    raise Exception(f"Input directory [{input_dir}] doesn't exist")

index_file = os.path.join(input_dir, "index.xml")
if not os.path.exists(index_file):
    raise Exception(f"Index file [{index_file}] doesn't exist")

output_dir = os.path.normpath(args.output)
include_dir = args.directory or ""
headers_dir = args.headers or include_dir
base_namespace = args.namespace or ""
base_systems = (args.systems or "").split(",")

print(f"Input Folder:  {input_dir}")
print(f"Output Folder: {output_dir}")
print(f"Directory:     {include_dir}")
print(f"Headers:       {headers_dir}")
print(f"Namespace:     {base_namespace}")
print(f"Systems:       {base_systems}")

references = {}
metadata = generate_metadata(ET.parse(index_file).getroot())

add_index_file(metadata)
add_references_paths(metadata, metadata)
add_definition_files(metadata, metadata)
