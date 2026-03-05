using System.Text;
using System.Text.RegularExpressions;

namespace Reflector
{
    static class Parser
    {
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

            for (int i = 0; i < lines.Length; i++)
            {
                var line = lines[i].Trim();

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

                if (nextEnum)
                {
                    var e = ParseEnum(lines, ref i);

                    if (e != null)
                    {
                        enums.Add(e);
                    }

                    nextEnum = false;

                    continue;
                }

                if (nextReflect)
                {
                    var t = ParseType(lines, ref i);

                    if (t != null)
                    {
                        types.Add(t);
                    }

                    nextReflect = false;

                    continue;
                }
            }

            return (types, enums);
        }

        static TypeModel? ParseType(string[] lines, ref int i)
        {
            var m = Regex.Match(lines[i].Trim(), @"^(struct|class)\s+(\w+)");
            if (!m.Success) return null;

            string kind = m.Groups[1].Value, name = m.Groups[2].Value;
            var fields = new List<FieldModel>(); var methods = new List<MethodModel>();
            bool takeField = false, takeMethod = false; int depth = 0;

            for (; i < lines.Length; i++)
            {
                var line = lines[i].Trim();
                depth += Count(line, '{') - Count(line, '}');
                if (line.StartsWith(Enum.GetStringValue(Annotation.Field))) { takeField = true; continue; }
                if (line.StartsWith(Enum.GetStringValue(Annotation.Method))) { takeMethod = true; continue; }
                if (takeField) { var f = ParseField(line); if (f != null) fields.Add(f); takeField = false; }
                else if (takeMethod) { var me = ParseMethod(line); if (me != null) methods.Add(me); takeMethod = false; }
                if (depth == 0 && i > 0 && line.Contains('}')) break;
            }
            return new(kind, name, fields, methods);
        }

        static EnumModel? ParseEnum(string[] lines, ref int i)
        {
            var m = Regex.Match(lines[i].Trim(), @"^enum\s+class\s+(\w+)");
            if (!m.Success) return null;

            string name = m.Groups[1].Value;
            var list = new List<EnumeratorModel>(); int next = 0; bool take = false;

            for (; i < lines.Length; i++)
            {
                var line = lines[i].Trim();
                var clean = line.TrimEnd(',').Trim();
                var me = Regex.Match(clean, @"(\w+)\s*=\s*(-?\d+)");
                if (me.Success) { next = int.Parse(me.Groups[2].Value); list.Add(new(me.Groups[1].Value, next++)); }
                else { var s = Regex.Match(clean, @"(\w+)"); if (s.Success) list.Add(new(s.Groups[1].Value, next++)); }

                if (line.Contains('}')) break;
            }
            return new(name, list);
        }

        static FieldModel? ParseField(string line)
        {
            line = line.TrimEnd(';').Trim();
            var m = Regex.Match(line, @"^([\w:<>]+(?:\s*\*)?)\s+(\w+)$");
            return m.Success ? new(m.Groups[1].Value, m.Groups[2].Value) : null;
        }

        static MethodModel? ParseMethod(string line)
        {
            var m = Regex.Match(line, @"^([\w:<>]+)\s+(\w+)\s*\(([^)]*)\)");
            if (!m.Success) return null;
            var pts = new List<string>();
            var raw = m.Groups[3].Value.Trim();
            if (!string.IsNullOrWhiteSpace(raw))
                foreach (var p in raw.Split(','))
                { var pp = p.Trim().Split(' ', StringSplitOptions.RemoveEmptyEntries); if (pp.Length > 0) pts.Add(pp[0]); }
            return new(m.Groups[1].Value, m.Groups[2].Value, pts);
        }

        static int Count(string s, char c) { int n = 0; foreach (var ch in s) if (ch == c) n++; return n; }
    }
}
