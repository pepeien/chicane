class Program
{
    static int Main(string[] args)
    {
        List<string> inputFiles = [];
        List<string> lookUpFolders = [];

        string baseDir = "";
        string sourceDir = "";
        string outputDir = "";

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

                default:
                    break;
            }
        }

        if (inputFiles.Count == 0 || string.IsNullOrEmpty(baseDir) || string.IsNullOrEmpty(outputDir))
        {
            Console.WriteLine("Usage: program -i <input files> -b <base folder> -s <source folder> -o <output folder>");

            return 1;
        }

        Dictionary<string, List<Reflector.EnumModel>> allEnums = [];
        Dictionary<string, List<Reflector.TypeModel>> allTypes = [];
        foreach (string file in inputFiles)
        {
            var (types, enums) = Reflector.Parser.Parse(file, lookUpFolders);

            allEnums[file] = enums;
            allTypes[file] = types;
        }

        List<Reflector.TypeModel> flatTypes = [.. allTypes.Values.SelectMany(t => t)];
        foreach (string file in inputFiles)
        {
            if (!file.StartsWith(sourceDir, StringComparison.OrdinalIgnoreCase))
            {
                continue;
            }

            List<Reflector.EnumModel> enums = allEnums.GetValueOrDefault(file, []);
            List<Reflector.TypeModel> types = allTypes.GetValueOrDefault(file, []);

            if (enums.Count <= 0 && types.Count <= 0)
            {
                continue;
            }

            string relativePath = Path.GetRelativePath(baseDir, file);
            string directory = Path.GetDirectoryName(relativePath) ?? "";
            string baseName = Path.GetFileNameWithoutExtension(relativePath);
            string extension = Path.GetExtension(relativePath);
            string outputPath = Path.Combine(outputDir, directory, $"{baseName}.reflected{extension}");

            Directory.CreateDirectory(Path.GetDirectoryName(outputPath)!);

            File.WriteAllText(outputPath, Reflector.Emitter.Emit(file, types, enums, flatTypes, baseDir));
        }

        return 0;
    }
}