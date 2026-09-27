using System.Text;

namespace Reflector
{
    // Emits EmmyLua definition files so a language server can see the same members the
    // reflected metatables expose at runtime. Nothing here is loaded by the engine.
    static class Lua
    {
        public static string ShortName(string qualifiedName)
        {
            int split = qualifiedName.LastIndexOf(':');

            return split < 0 ? qualifiedName : qualifiedName[(split + 1)..];
        }

        static string Clean(string typeName)
        {
            return typeName.Replace("const ", "").Replace("&", "").Replace(" ", "").Trim();
        }

        static string ElementOf(string typeName)
        {
            int start = typeName.IndexOf('<');
            int end = typeName.LastIndexOf('>');

            if (start < 0 || end <= start)
            {
                return "";
            }

            string inner = typeName[(start + 1)..end];
            int comma = inner.IndexOf(',');

            return comma < 0 ? inner.Trim() : inner[..comma].Trim();
        }

        // C++ spelling to the annotation the marshalling layer actually produces
        static string TypeOf(string typeName, IReadOnlySet<string> objects, IReadOnlySet<string> enums)
        {
            string name = Clean(typeName);

            if (name.Length == 0 || name == "void")
            {
                return "";
            }

            if (name.StartsWith("std::vector<") || name.StartsWith("std::array<") ||
                name.StartsWith("std::list<") || name.StartsWith("std::deque<"))
            {
                string element = ElementOf(name);

                return element.Length == 0 ? "any[]" : $"{TypeOf(element, objects, enums)}[]";
            }

            bool isPointer = name.EndsWith('*');
            if (isPointer)
            {
                name = name.TrimEnd('*');
            }

            string tail = ShortName(name);

            if (objects.Contains(name))
            {
                return isPointer ? $"{tail}|nil" : tail;
            }

            if (enums.Contains(name))
            {
                return tail;
            }

            switch (tail)
            {
                case "bool":
                    return "boolean";

                case "float":
                case "double":
                    return "number";

                case "int":
                case "int32_t":
                case "uint32_t":
                case "size_t":
                    return "integer";

                case "String":
                case "string":
                case "Path":
                    return "string";

                case "Vec2":
                case "Vec3":
                case "Vec4":
                case "Rotator":
                    return tail;

                case "Rgba":
                    return "Color";

                default:
                    return "any";
            }
        }

        static void EmitEnum(StringBuilder sb, EnumModel model)
        {
            IEnumerable<string> values = model.Enumerators
                .Select(e => $"\"{ShortName(e.Name)}\"")
                .Distinct(StringComparer.Ordinal);

            sb.AppendLine($"---@alias {ShortName(model.Name)} {string.Join("|", values)}");
            sb.AppendLine();
        }

        static void EmitType(
            StringBuilder sb,
            TypeModel model,
            IReadOnlySet<string> objects,
            IReadOnlySet<string> enums
        )
        {
            string name = ShortName(model.Name);
            string parent = model.Bases.FirstOrDefault(objects.Contains) ?? "";

            sb.AppendLine(
                parent.Length == 0
                    ? $"---@class {name}"
                    : $"---@class {name} : {ShortName(parent)}"
            );

            foreach (FieldModel field in model.OwnFields)
            {
                if (field.Names.Count == 0)
                {
                    continue;
                }

                string fieldType = field.IsIterable
                    ? $"{TypeOf(field.ElementName, objects, enums)}[]"
                    : TypeOf(field.TypeName + (field.IsPointer ? "*" : ""), objects, enums);

                string description = string.IsNullOrWhiteSpace(field.Description)
                    ? ""
                    : $" {field.Description}";

                sb.AppendLine($"---@field {field.Name} {fieldType}{description}");
            }

            sb.AppendLine($"local {name} = {{}}");
            sb.AppendLine();

            foreach (FunctionModel method in model.OwnFunctions)
            {
                foreach ((string paramName, string paramType) in method.ParamTypes.Select(
                    (p, i) => (Name: $"arg{i + 1}", Type: TypeOf(p, objects, enums))
                ))
                {
                    sb.AppendLine($"---@param {paramName} {paramType}");
                }

                string returns = method.IsIterable
                    ? $"{TypeOf(method.ElementName, objects, enums)}[]"
                    : TypeOf(method.ReturnType, objects, enums);

                if (returns.Length > 0)
                {
                    sb.AppendLine($"---@return {returns}");
                }

                string arguments = string.Join(
                    ", ",
                    Enumerable.Range(1, method.ParamTypes.Count).Select(i => $"arg{i}")
                );

                sb.AppendLine($"function {name}:{method.Name}({arguments}) end");
                sb.AppendLine();
            }
        }

        public static string? Emit(
            List<TypeModel> types,
            List<EnumModel> enums,
            IReadOnlySet<string> objects,
            IReadOnlySet<string> enumNames
        )
        {
            List<TypeModel> emitted = [.. types.Where(t => objects.Contains(t.Name))];

            if (emitted.Count == 0 && enums.Count == 0)
            {
                return null;
            }

            var sb = new StringBuilder();
            sb.AppendLine("---@meta");
            sb.AppendLine("-- AUTO-GENERATED by Reflector - do not edit by hand");
            sb.AppendLine();

            foreach (EnumModel model in enums)
            {
                EmitEnum(sb, model);
            }

            foreach (TypeModel model in emitted)
            {
                EmitType(sb, model, objects, enumNames);
            }

            return sb.ToString();
        }
    }
}
