using System.Text;
using System.Text.RegularExpressions;

namespace Reflector
{
    static class Parser
    {
        static readonly HashSet<string> s_keywords = new() {
            "enum", "class", "struct", "public", "private", "protected"
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

            bool nextReflect = false, nextEnum = false;

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
                    nextReflect = true;

                    continue;
                }

                if (line.StartsWith(Enum.GetStringValue(Annotation.Enum)))
                {
                    nextEnum = true;

                    continue;
                }

                if (nextReflect)
                {
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }

                    var t = ParseType(lines, ref i, CurrentNamespace());

                    if (t != null)
                    {
                        types.Add(t);
                    }

                    nextReflect = false;

                    continue;
                }

                if (nextEnum)
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

                    nextEnum = false;

                    continue;
                }
            }

            return (types, enums);
        }

        static TypeModel? ParseType(string[] lines, ref int i, string currentNamespace)
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

            var fields = new List<FieldModel>();
            var methods = new List<MethodModel>();
            bool takeField = false;
            bool takeMethod = false;
            int depth = 0;

            for (; i < lines.Length; i++)
            {
                var line = lines[i].Trim();

                depth += Count(line, '{') - Count(line, '}');

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
                    var f = ParseField(line);

                    if (f != null)
                    {
                        fields.Add(f);
                    }

                    takeField = false;
                }
                else if (takeMethod)
                {
                    var me = ParseMethod(line);

                    if (me != null)
                    {
                        methods.Add(me);
                    }

                    takeMethod = false;
                }

                if (depth == 0 && i > 0 && line.Contains('}'))
                {
                    break;
                }
            }

            return new(kind, name, currentNamespace, fields, methods);
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

        static FieldModel? ParseField(string line)
        {
            line = line.TrimEnd(';').Trim();

            bool isPointer = line.Contains('*');

            var m = Regex.Match(line, @"^(?:const\s+)?([\w:<>]+)\s*[*&]?\s+(\w+)$");

            if (!m.Success)
            {
                return null;
            }

            return new(m.Groups[1].Value.Trim(), m.Groups[2].Value.Trim(), isPointer);
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