using System.Collections.Concurrent;

class Program
{
    static HashSet<string> LoadReflectedTypeNames(string typesDir)
    {
        HashSet<string> names = new(StringComparer.Ordinal);

        if (string.IsNullOrEmpty(typesDir) || !Directory.Exists(typesDir))
        {
            return names;
        }

        foreach (string file in Directory.EnumerateFiles(typesDir, "*.types"))
        {
            foreach (string line in File.ReadLines(file))
            {
                string trimmed = line.Trim();

                if (trimmed.Length == 0 || trimmed.StartsWith('#'))
                {
                    continue;
                }

                names.Add(trimmed);
            }
        }

        return names;
    }

    static void WriteReflectedTypeNames(string typesDir, string targetName, IEnumerable<string> typeNames)
    {
        Directory.CreateDirectory(typesDir);

        string path = Path.Combine(typesDir, $"{targetName}.types");
        IEnumerable<string> lines = typeNames
            .Where(n => !string.IsNullOrWhiteSpace(n))
            .Distinct(StringComparer.Ordinal)
            .OrderBy(n => n, StringComparer.Ordinal);

        File.WriteAllLines(path, lines);
    }

    static int Main(string[] args)
    {
        List<string> inputFiles = [];
        List<string> lookUpFolders = [];

        string baseDir = "";
        string sourceDir = "";
        string outputDir = "";
        string typesDir = "";
        string targetName = "";

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "-i":
                    while (i + 1 < args.Length && !args[i + 1].StartsWith('-'))
                    {
                        inputFiles.Add(Path.GetFullPath(args[++i]));
                    }

                    break;

                case "-f":
                    if (i + 1 >= args.Length)
                    {
                        Console.WriteLine("Error: -f requires an inputs list file.");

                        return 1;
                    }

                    {
                        string listPath = Path.GetFullPath(args[++i]);

                        if (!File.Exists(listPath))
                        {
                            Console.WriteLine($"Error: inputs list not found: {listPath}");

                            return 1;
                        }

                        foreach (string line in File.ReadLines(listPath))
                        {
                            string trimmed = line.Trim();

                            if (trimmed.Length == 0 || trimmed.StartsWith('#'))
                            {
                                continue;
                            }

                            inputFiles.Add(Path.GetFullPath(trimmed));
                        }
                    }

                    break;

                case "-b":
                    if (i + 1 < args.Length)
                    {
                        baseDir = Path.GetFullPath(args[++i]);
                    }
                    else
                    {
                        Console.WriteLine("Error: -b requires a base folder path.");

                        return 1;
                    }

                    break;

                case "-s":
                    if (i + 1 < args.Length)
                    {
                        sourceDir = Path.GetFullPath(args[++i]);
                    }
                    else
                    {
                        Console.WriteLine("Error: -s requires a source folder path.");

                        return 1;
                    }

                    break;

                case "-l":
                    while (i + 1 < args.Length && !args[i + 1].StartsWith('-'))
                    {
                        lookUpFolders.Add(Path.GetFullPath(args[++i]));
                    }

                    break;

                case "-L":
                    if (i + 1 >= args.Length)
                    {
                        Console.WriteLine("Error: -L requires an includes list file.");

                        return 1;
                    }

                    {
                        string listPath = Path.GetFullPath(args[++i]);

                        if (!File.Exists(listPath))
                        {
                            Console.WriteLine($"Error: includes list not found: {listPath}");

                            return 1;
                        }

                        foreach (string line in File.ReadLines(listPath))
                        {
                            string trimmed = line.Trim();

                            if (trimmed.Length == 0 || trimmed.StartsWith('#'))
                            {
                                continue;
                            }

                            lookUpFolders.Add(Path.GetFullPath(trimmed));
                        }
                    }

                    break;

                case "-o":
                    if (i + 1 < args.Length)
                    {
                        outputDir = Path.GetFullPath(args[++i]);
                    }
                    else
                    {
                        Console.WriteLine("Error: -o requires an output folder path.");

                        return 1;
                    }

                    break;

                case "-t":
                    if (i + 1 < args.Length)
                    {
                        typesDir = Path.GetFullPath(args[++i]);
                    }
                    else
                    {
                        Console.WriteLine("Error: -t requires a types directory path.");

                        return 1;
                    }

                    break;

                case "-n":
                    if (i + 1 < args.Length)
                    {
                        targetName = args[++i];
                    }
                    else
                    {
                        Console.WriteLine("Error: -n requires a target name.");

                        return 1;
                    }

                    break;

                default:
                    break;
            }
        }

        if (inputFiles.Count == 0 || string.IsNullOrEmpty(baseDir) || string.IsNullOrEmpty(outputDir))
        {
            Console.WriteLine("Usage: program (-i <input files> | -f <inputs list>) -b <base folder> -s <source folder> -o <output folder> [-l <look-up folders>] [-L <includes list>] [-t <types dir> -n <target name>]");

            return 1;
        }

        HashSet<string> reflectedTypeNames = LoadReflectedTypeNames(typesDir);

        ConcurrentDictionary<string, List<Reflector.EnumModel>> allEnums = new();
        ConcurrentDictionary<string, List<Reflector.TypeModel>> allTypes = new();
        Parallel.ForEach(
            inputFiles,
            file =>
            {
                var (types, enums) = Reflector.Parser.Parse(file, lookUpFolders);

                allEnums[file] = enums;
                allTypes[file] = types;
            }
        );

        List<Reflector.TypeModel> flatTypes = [.. allTypes.Values.SelectMany(t => t)];

        foreach (Reflector.TypeModel type in flatTypes)
        {
            reflectedTypeNames.Add(type.Name);
        }

        Parallel.ForEach(
            inputFiles,
            file =>
            {
                if (!file.StartsWith(sourceDir, StringComparison.OrdinalIgnoreCase))
                {
                    return;
                }

                List<Reflector.EnumModel> enums = allEnums.GetValueOrDefault(file, []);
                List<Reflector.TypeModel> types = allTypes.GetValueOrDefault(file, []);

                if (enums.Count <= 0 && types.Count <= 0)
                {
                    return;
                }

                string relativePath = Path.GetRelativePath(baseDir, file);
                string directory = Path.GetDirectoryName(relativePath) ?? "";
                string baseName = Path.GetFileNameWithoutExtension(relativePath);
                string extension = Path.GetExtension(relativePath);
                string outputPath = Path.Combine(outputDir, directory, $"{baseName}.reflected{extension}");

                Directory.CreateDirectory(Path.GetDirectoryName(outputPath)!);

                File.WriteAllText(outputPath, Reflector.Emitter.Emit(file, types, enums, reflectedTypeNames, baseDir));
            }
        );

        if (!string.IsNullOrEmpty(typesDir) && !string.IsNullOrEmpty(targetName))
        {
            WriteReflectedTypeNames(typesDir, targetName, flatTypes.Select(t => t.Name));
        }

        return 0;
    }
}
