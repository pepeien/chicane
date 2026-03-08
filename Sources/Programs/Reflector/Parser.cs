using System.Text;
using System.Text.RegularExpressions;

namespace Reflector
{
    static class Parser
    {
        static readonly HashSet<string> s_keywords = new() {
            "enum", "class", "struct", "public", "private", "protected"
        };

        static readonly HashSet<string> s_fieldSkip = new()
        {
            "public", "private", "protected", "virtual", "override",
            "explicit", "static", "inline", "friend", "mutable", "constexpr"
        };

        public static string StripComments(string src)
        {
            src = Regex.Replace(src, @"//[^\n]*", "");
            src = Regex.Replace(src, @"/\*.*?\*/", "", RegexOptions.Singleline);

            return src;
        }

        public static (List<TypeModel>, List<EnumModel>) Parse(string src)
        {
            src = StripComments(src);

            var lines = src.Split('\n');
            var types = new List<TypeModel>();
            var enums = new List<EnumModel>();

            Annotation next = Annotation.Undefined;
            AnnotationInclusion nextInclusion = AnnotationInclusion.Manual;

            var namespaceStack = new Stack<string>();
            string CurrentNamespace() => namespaceStack.Count > 0
                ? string.Join("::", namespaceStack.Reverse())
                : string.Empty;

            var namespaceBraceDepth = new Stack<int>();
            int braceDepth = 0;

            for (int i = 0; i < lines.Length; i++)
            {
                var line = lines[i].Trim();

                int opened = Count(line, '{');
                int closed = Count(line, '}');

                var nsMatch = Regex.Match(line, @"^namespace\s+(\w+)\s*\{?");

                if (nsMatch.Success && !line.StartsWith("//"))
                {
                    namespaceStack.Push(nsMatch.Groups[1].Value);
                    namespaceBraceDepth.Push(braceDepth + (opened > 0 ? 1 : 0));
                }

                braceDepth += opened - closed;

                while (namespaceBraceDepth.Count > 0 && braceDepth < namespaceBraceDepth.Peek())
                {
                    namespaceBraceDepth.Pop();
                    namespaceStack.Pop();
                }

                if (line.StartsWith(Enum.GetStringValue(Annotation.Type)))
                {
                    next = Annotation.Type;
                    nextInclusion = line.Contains(Enum.GetStringValue(AnnotationInclusion.Automatic)) ?
                        AnnotationInclusion.Automatic :
                        AnnotationInclusion.Manual;

                    continue;
                }

                if (line.StartsWith(Enum.GetStringValue(Annotation.Enum)))
                {
                    next = Annotation.Enum;

                    continue;
                }

                if (next == Annotation.Type)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var t = ParseType(lines, ref i, CurrentNamespace(), nextInclusion);

                    if (t != null)
                    {
                        types.Add(t);
                    }

                    next = Annotation.Undefined;

                    continue;
                }

                if (next == Annotation.Enum)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var e = ParseEnum(lines, ref i, CurrentNamespace());

                    if (e != null)
                    {
                        enums.Add(e);
                    }

                    next = Annotation.Undefined;

                    continue;
                }
            }

            return (types, enums);
        }

        static TypeModel? ParseType(string[] lines, ref int i, string currentNamespace, AnnotationInclusion inclusion)
        {
            var m = Regex.Match(
                lines[i].Trim(),
                @"^(struct|class)\s+(?:[A-Z_][A-Z0-9_]*\s+)?(\w+)(?:\s*[:{].*)?$"
            );

            if (!m.Success)
            {
                return null;
            }

            string kind = m.Groups[1].Value;
            string name = m.Groups[2].Value;

            // Extract first base class
            var baseMatch = Regex.Match(lines[i].Trim(), @":\s*(?:public|private|protected)\s+([\w:]+)");
            string baseType = baseMatch.Success ? baseMatch.Groups[1].Value.Trim() : "";

            List<FieldModel> fields = [];
            List<MethodModel> methods = [];

            bool takeField = false;
            bool takeMethod = false;

            bool isPublic = kind == "struct";

            bool inUnion = false;
            bool tookUnionField = false;

            int depth = 0;

            for (; i < lines.Length; i++)
            {
                var line = lines[i].Trim();

                depth += Count(line, '{') - Count(line, '}');

                if (line == "public:")
                {
                    isPublic = true;

                    continue;
                }

                if (line == "private:")
                {
                    isPublic = false;

                    continue;
                }

                if (line == "protected:")
                {
                    isPublic = false;

                    continue;
                }

                if (!inUnion && (line == "union" || line == "union{" || line.StartsWith("union ")))
                {
                    inUnion = true;

                    tookUnionField = false;

                    continue;
                }

                if (inUnion)
                {
                    if (line.Contains('}'))
                    {
                        inUnion = false;
                        tookUnionField = false;

                        continue;
                    }

                    if (!tookUnionField && inclusion == AnnotationInclusion.Automatic && isPublic)
                    {
                        if (string.IsNullOrWhiteSpace(line))
                        {
                            continue;
                        }

                        var uf = ParseField(line);
                        if (uf != null)
                        {
                            fields.Add(uf);

                            tookUnionField = true;
                        }
                    }

                    continue;
                }

                if (line.StartsWith(Enum.GetStringValue(Annotation.Field)))
                {
                    takeField = true;

                    continue;
                }

                if (line.StartsWith(Enum.GetStringValue(Annotation.Method)))
                {
                    takeMethod = true;

                    continue;
                }

                if (takeField)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var f = ParseField(line);
                    if (f != null)
                    {
                        fields.Add(f);
                    }

                    takeField = false;
                }
                else if (takeMethod)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var me = ParseMethod(line);
                    if (me != null)
                    {
                        methods.Add(me);
                    }

                    takeMethod = false;
                }
                else if (inclusion == AnnotationInclusion.Automatic && isPublic && depth > 0)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var f = ParseField(line);
                    if (f != null)
                    {
                        fields.Add(f);
                    }
                }

                if (depth == 0 && i > 0 && line.Contains('}'))
                {
                    break;
                }
            }

            return new(kind, name, currentNamespace, fields, methods, baseType);
        }

        static FieldModel? ParseField(string line)
        {
            line = line.TrimEnd(';').Trim();

            if (
                string.IsNullOrWhiteSpace(line) ||
                line.StartsWith("//") ||
                line.Contains('(') ||
                line.Contains('{') ||
                line.Contains('}')
            )
            {
                return null;
            }

            if (Regex.IsMatch(line, @"(?<!:):(?!:)"))
            {
                return null;
            }

            var firstWord = line.Split(' ')[0].TrimStart('~');
            if (s_fieldSkip.Contains(firstWord))
            {
                return null;
            }

            var member = Regex.Match(line, @"^(?:const\s+)?([\w:<>]+)\s*[*&]?\s+([\w\s,]+)$");
            if (!member.Success)
            {
                return null;
            }

            string typeName = member.Groups[1].Value.Trim();
            var propertyNames = member.Groups[2].Value
                .Split(',')
                .Select(n => n.Trim())
                .Where(n => !string.IsNullOrWhiteSpace(n))
                .ToList();

            if (propertyNames.Count == 0)
            {
                return null;
            }

            return new(typeName, propertyNames, line.Contains('*'));
        }

        static EnumModel? ParseEnum(string[] lines, ref int i, string currentNamespace)
        {
            var m = Regex.Match(lines[i].Trim(), @"^enum\s+class\s+(\w+)");

            if (!m.Success)
            {
                return null;
            }

            string name = m.Groups[1].Value;
            var list = new List<EnumeratorModel>();
            int next = 0;

            i++;

            for (; i < lines.Length; i++)
            {
                var line = lines[i].Trim();
                var clean = line.TrimEnd(',').Trim();

                if (clean == "{" || clean == "}" || string.IsNullOrWhiteSpace(clean))
                {
                    if (line.Contains('}'))
                    {
                        break;
                    }

                    continue;
                }

                var me = Regex.Match(clean, @"(\w+)\s*=\s*(-?\d+)");

                if (me.Success)
                {
                    next = int.Parse(me.Groups[2].Value);

                    list.Add(new(me.Groups[1].Value, next++));
                }
                else
                {
                    var s = Regex.Match(clean, @"^(\w+)");

                    if (s.Success && !s_keywords.Contains(s.Groups[1].Value))
                    {
                        list.Add(new(s.Groups[1].Value, next++));
                    }
                }

                if (line.Contains('}'))
                {
                    break;
                }
            }

            return new(name, currentNamespace, list);
        }

        static MethodModel? ParseMethod(string line)
        {
            var m = Regex.Match(line, @"^([\w:<>]+)\s+(\w+)\s*\(([^)]*)\)");

            if (!m.Success)
            {
                return null;
            }

            var pts = new List<string>();
            var raw = m.Groups[3].Value.Trim();

            if (!string.IsNullOrWhiteSpace(raw))
            {
                foreach (var p in raw.Split(','))
                {
                    var param = p.Trim();
                    if (string.IsNullOrWhiteSpace(param)) continue;

                    var lastSpace = param.LastIndexOf(' ');
                    var typePart = lastSpace >= 0 ? param[..lastSpace].Trim() : param;

                    typePart = typePart
                        .Replace("&", "")
                        .Replace("const", "")
                        .Trim();

                    pts.Add(typePart);
                }
            }

            return new(m.Groups[1].Value, m.Groups[2].Value, pts);
        }

        static int Count(string s, char c)
        {
            int n = 0;

            foreach (var ch in s)
            {
                if (ch == c)
                {
                    n++;
                }
            }

            return n;
        }
    }
}