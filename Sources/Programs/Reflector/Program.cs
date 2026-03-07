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
                    {
                        baseDir = args[++i];
                    }
                    else
                    {
                        Console.WriteLine("Error: -s requires an base folder path.");

                        return 1;
                    }

                    break;

                case "-s":
                    if (i + 1 < args.Length)
                    {
                        sourceDir = args[++i];
                    }

                    break;

                case "-o":
                    if (i + 1 < args.Length)
                    {
                        outputDir = args[++i];
                    }
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

        foreach (var file in inputFiles)
        {
            var (types, enums) = Reflector.Parser.Parse(File.ReadAllText(file));

            foreach (var t in types)
            {
                var qualified = string.IsNullOrWhiteSpace(t.Namespace)
                    ? t.Name
                    : $"{t.Namespace}::{t.Name}";

                allReflectedTypes.TryAdd(t.Name, qualified);

                allReflectedTypes.TryAdd(qualified, qualified);
            }
        }

        foreach (var file in inputFiles)
        {
            var (types, enums) = Reflector.Parser.Parse(File.ReadAllText(file));

            var fullFile = Path.GetFullPath(file);

            if (!fullFile.StartsWith(Path.GetFullPath(sourceDir)))
            {
                continue;
            }

            var relativePath = Path.GetRelativePath(baseDir, file);
            var directory = Path.GetDirectoryName(relativePath) ?? "";
            var baseName = Path.GetFileNameWithoutExtension(relativePath);
            var extension = Path.GetExtension(relativePath);
            var outputFileName = $"{baseName}.reflected{extension}";
            var outputPath = Path.Combine(outputDir, directory, outputFileName);

            Directory.CreateDirectory(Path.GetDirectoryName(outputPath)!);
            File.WriteAllText(
                outputPath,
                Reflector.Emitter.Emit(file, types, enums, allReflectedTypes, baseDir)
            );
        }

        return 0;
    }
}
