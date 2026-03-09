class Program
{
    static int Main(string[] args)
    {
        List<string> inputFiles = [];
        string baseDir = null;
        string sourceDir = null;
        string outputDir = null;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "-i":
                    while (i + 1 < args.Length && !args[i + 1].StartsWith('-'))
                    {
                        inputFiles.Add(args[++i]);
                    }

                    break;
                case "-b":
                    if (i + 1 < args.Length)
                        baseDir = args[++i];
                    else
                    {
                        Console.WriteLine("Error: -b requires a base folder path.");

                        return 1;
                    }

                    break;
                case "-s":
                    if (i + 1 < args.Length)
                        sourceDir = args[++i];
                    break;
                case "-o":
                    if (i + 1 < args.Length)
                        outputDir = args[++i];
                    else
                    {
                        Console.WriteLine("Error: -o requires an output folder path.");

                        return 1;
                    }

                    break;
                default:
                    Console.WriteLine($"Unknown argument: {args[i]}");

                    return 1;
            }
        }

        if (inputFiles.Count == 0 || string.IsNullOrEmpty(baseDir) || string.IsNullOrEmpty(sourceDir) || string.IsNullOrEmpty(outputDir))
        {
            Console.WriteLine("Usage: program -i <input files> -b <base folder> -s <source folder> -o <output folder>");

            return 1;
        }

        if (!Directory.Exists(outputDir))
        {
            Directory.CreateDirectory(outputDir);
        }

        var allReflectedTypes = new Dictionary<string, string>();
        var allTypeModels = new List<(string File, Reflector.TypeModel Type)>();
        var allEnumModels = new List<(string File, Reflector.EnumModel Enum)>();

        foreach (var file in inputFiles)
        {
            var (types, enums) = Reflector.Parser.Parse(File.ReadAllText(file));

            foreach (var e in enums)
            {
                allEnumModels.Add((file, e));
            }

            foreach (var t in types)
            {
                var qualified = string.IsNullOrWhiteSpace(t.Namespace)
                    ? t.Name
                    : $"{t.Namespace}::{t.Name}";

                allReflectedTypes.TryAdd(t.Name, qualified);
                allReflectedTypes.TryAdd(qualified, qualified);

                if (!string.IsNullOrWhiteSpace(t.Namespace))
                {
                    var nsParts = t.Namespace.Split("::");
                    for (int j = nsParts.Length - 1; j >= 0; j--)
                    {
                        var partial = string.Join("::", nsParts[j..]) + "::" + t.Name;

                        allReflectedTypes.TryAdd(partial, qualified);
                    }
                }

                allTypeModels.Add((file, t));
            }
        }

        var typeMap = new Dictionary<string, Reflector.TypeModel>();
        foreach (var (_, t) in allTypeModels)
        {
            var qualified = string.IsNullOrWhiteSpace(t.Namespace)
                ? t.Name
                : $"{t.Namespace}::{t.Name}";

            typeMap[qualified] = t;
        }

        void ResolveInheritance(Reflector.TypeModel t, HashSet<string> visited)
        {
            string qualified = string.IsNullOrWhiteSpace(t.Namespace)
                ? t.Name
                : $"{t.Namespace}::{t.Name}";

            if (!visited.Add(qualified) || t.Resolved)
            {
                return;
            }

            if (string.IsNullOrWhiteSpace(t.BaseType))
            {
                return;
            }

            Reflector.TypeModel? baseModel = null;

            if (typeMap.TryGetValue(t.BaseType, out var found))
            {
                baseModel = found;
            }
            else
            {
                var nsParts = t.Namespace?.Split("::") ?? Array.Empty<string>();

                for (int i = nsParts.Length; i >= 0; i--)
                {
                    var prefix = string.Join("::", nsParts.Take(i));
                    var candidate = string.IsNullOrEmpty(prefix)
                        ? t.BaseType
                        : $"{prefix}::{t.BaseType}";

                    if (typeMap.TryGetValue(candidate, out found))
                    {
                        baseModel = found;
                        break;
                    }
                }
            }

            if (baseModel == null)
                return;

            ResolveInheritance(baseModel, visited);

            var inherited = baseModel.Fields
                .Select(f => f with { Names = new List<string>(f.Names) })
                .ToList();

            t.Fields.InsertRange(0, inherited);

            t.Resolved = true;
        }

        foreach (var (_, t) in allTypeModels)
        {
            ResolveInheritance(t, new HashSet<string>());
        }

        var emittedFiles = new HashSet<string>();
        foreach (var (file, _) in allTypeModels.Concat(allEnumModels.Select(e => (e.File, (Reflector.TypeModel)null!))))
        {
            if (!emittedFiles.Add(file))
            {
                continue;
            }

            var fullFile = Path.GetFullPath(file);
            if (!fullFile.StartsWith(Path.GetFullPath(sourceDir)))
            {
                continue;
            }

            var types = allTypeModels.Where(x => x.File == file).Select(x => x.Type).ToList();
            var enums = allEnumModels.Where(x => x.File == file).Select(x => x.Enum).ToList();

            var relativePath = Path.GetRelativePath(baseDir, file);
            var directory = Path.GetDirectoryName(relativePath) ?? "";
            var baseName = Path.GetFileNameWithoutExtension(relativePath);
            var extension = Path.GetExtension(relativePath);
            var outputPath = Path.Combine(outputDir, directory, $"{baseName}.reflected{extension}");

            Directory.CreateDirectory(Path.GetDirectoryName(outputPath)!);
            File.WriteAllText(
                outputPath,
                Reflector.Emitter.Emit(file, types, enums, allReflectedTypes, baseDir)
            );
        }

        return 0;
    }
}