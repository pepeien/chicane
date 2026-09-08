namespace Reflector
{
    readonly record struct MacroArg(string Name, string Value)
    {
        public bool IsNamed => Name.Length > 0;
    }

    static class MacroArgs
    {
        static readonly string Automatic = Enum.GetStringValue(AnnotationInclusion.Automatic);
        static readonly string Manual    = Enum.GetStringValue(AnnotationInclusion.Manual);

        static readonly string[] TypeKeys =
        [
            Enum.GetStringValue(AnnotationParam.Type),
            "Inclusion",
            "Mode"
        ];

        static readonly string[] AliasKeys =
        [
            Enum.GetStringValue(AnnotationParam.Alias),
            "Aliases",
            "typename",
            "Name"
        ];

        public static List<string> Split(string value)
        {
            var parts = new List<string>();
            if (string.IsNullOrWhiteSpace(value))
            {
                return parts;
            }

            var current = new System.Text.StringBuilder();
            int depth = 0;
            bool inQuote = false;

            for (int i = 0; i < value.Length; i++)
            {
                char character = value[i];

                if (character == '"' && (i == 0 || value[i - 1] != '\\'))
                {
                    inQuote = !inQuote;
                    current.Append(character);
                    continue;
                }

                if (!inQuote)
                {
                    if (character is '(' or '[' or '{' or '<')
                    {
                        depth++;
                    }
                    else if (character is ')' or ']' or '}' or '>')
                    {
                        depth = Math.Max(0, depth - 1);
                    }
                    else if (character == ',' && depth == 0)
                    {
                        string part = current.ToString().Trim();
                        if (part.Length > 0)
                        {
                            parts.Add(part);
                        }

                        current.Clear();
                        continue;
                    }
                }

                current.Append(character);
            }

            string last = current.ToString().Trim();
            if (last.Length > 0)
            {
                parts.Add(last);
            }

            return parts;
        }

        public static List<MacroArg> Parse(IEnumerable<string> parts)
        {
            var args = new List<MacroArg>();

            foreach (string part in parts)
            {
                string value = part.Trim();
                if (value.Length == 0)
                {
                    continue;
                }

                int split = IndexOfTopLevel(value, '=');
                if (split <= 0)
                {
                    foreach (string item in Split(Unwrap(value)))
                    {
                        args.Add(new("", item.Trim().Trim('"')));
                    }

                    continue;
                }

                string name = value[..split].Trim();
                if (!IsIdentifier(name))
                {
                    foreach (string item in Split(Unwrap(value)))
                    {
                        args.Add(new("", item.Trim().Trim('"')));
                    }

                    continue;
                }

                foreach (string item in Split(Unwrap(value[(split + 1)..])))
                {
                    string itemValue = item.Trim().Trim('"');
                    if (itemValue.Length > 0)
                    {
                        args.Add(new(name, itemValue));
                    }
                }
            }

            return args;
        }

        public static List<MacroArg> Parse(string value)
        {
            return Parse(Split(GetPayload(value)));
        }

        public static bool IsAutomatic(IEnumerable<MacroArg> args)
        {
            MacroArg? named = null;
            foreach (MacroArg arg in args)
            {
                if (arg.IsNamed && Matches(arg.Name, TypeKeys))
                {
                    named = arg;
                }
            }

            if (named.HasValue)
            {
                return named.Value.Value.Equals(Automatic, StringComparison.OrdinalIgnoreCase);
            }

            foreach (MacroArg arg in args)
            {
                if (!arg.IsNamed && arg.Value.Equals(Automatic, StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }

            return false;
        }

        public static List<string> Aliases(IEnumerable<MacroArg> args)
        {
            var aliases = new List<string>();

            foreach (MacroArg arg in args)
            {
                if (arg.IsNamed)
                {
                    if (Matches(arg.Name, AliasKeys))
                    {
                        Add(aliases, arg.Value);
                    }

                    continue;
                }

                if (!IsInclusion(arg.Value))
                {
                    Add(aliases, arg.Value);
                }
            }

            return aliases;
        }

        static string GetPayload(string annotationValue)
        {
            if (string.IsNullOrWhiteSpace(annotationValue))
            {
                return "";
            }

            int idx = annotationValue.IndexOf(':');
            return idx >= 0 ? annotationValue[(idx + 1)..].Trim() : annotationValue.Trim();
        }

        static string Unwrap(string value)
        {
            string result = value.Trim().Trim('"');
            while (IsWrapped(result))
            {
                result = result[1..^1].Trim().Trim('"');
            }

            return result;
        }

        static bool IsWrapped(string value)
        {
            if (value.Length < 2 || value[0] != '(' || value[^1] != ')')
            {
                return false;
            }

            int depth = 0;
            for (int i = 0; i < value.Length; i++)
            {
                char character = value[i];
                if (character == '(')
                {
                    depth++;
                }
                else if (character == ')')
                {
                    depth--;
                    if (depth == 0)
                    {
                        return i == value.Length - 1;
                    }
                }
            }

            return false;
        }

        static int IndexOfTopLevel(string value, char target)
        {
            int depth = 0;
            bool inQuote = false;

            for (int i = 0; i < value.Length; i++)
            {
                char character = value[i];

                if (character == '"' && (i == 0 || value[i - 1] != '\\'))
                {
                    inQuote = !inQuote;
                    continue;
                }

                if (inQuote)
                {
                    continue;
                }

                if (character is '(' or '[' or '{' or '<')
                {
                    depth++;
                    continue;
                }

                if (character is ')' or ']' or '}' or '>')
                {
                    depth = Math.Max(0, depth - 1);
                    continue;
                }

                if (character == target && depth == 0)
                {
                    return i;
                }
            }

            return -1;
        }

        static bool IsIdentifier(string value)
        {
            if (value.Length == 0 || (!char.IsLetter(value[0]) && value[0] != '_'))
            {
                return false;
            }

            for (int i = 1; i < value.Length; i++)
            {
                if (!char.IsLetterOrDigit(value[i]) && value[i] != '_')
                {
                    return false;
                }
            }

            return true;
        }

        static bool IsInclusion(string value)
        {
            return value.Equals(Automatic, StringComparison.OrdinalIgnoreCase) ||
                   value.Equals(Manual, StringComparison.OrdinalIgnoreCase);
        }

        static bool Matches(string value, string[] names)
        {
            foreach (string name in names)
            {
                if (value.Equals(name, StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }

            return false;
        }

        static void Add(List<string> values, string value)
        {
            if (value.Length == 0 || values.Contains(value))
            {
                return;
            }

            values.Add(value);
        }
    }
}
