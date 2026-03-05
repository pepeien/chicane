// ReflectTool.cs — Parses annotated C++ headers, emits *.generated.h
// Usage:  dotnet run -- path/to/MyFile.h

class Program
{
    static int Main(string[] args)
    {
        string inputFile = null;
        string outputFile = null;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "-i":
                    if (i + 1 < args.Length)
                    {
                        inputFile = args[++i];
                    }
                    else
                    {
                        Console.WriteLine("Error: -i requires an input file path.");

                        return 1;
                    }

                    break;

                case "-o":
                    if (i + 1 < args.Length)
                    {
                        outputFile = args[++i];
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

        if (string.IsNullOrEmpty(inputFile) || string.IsNullOrEmpty(outputFile))
        {
            Console.WriteLine("Usage: program -i <input file> -o <output folder>");

            return 1;
        }

        if (!File.Exists(inputFile))
        {
            Console.WriteLine($"Error: Input file does not exist: {inputFile}");

            return 1;
        }

        Console.WriteLine($"Processing: {inputFile}");

        var (types, enums) = Reflector.Parser.Parse(File.ReadAllText(inputFile));

        File.WriteAllText(outputFile, Reflector.Emitter.Emit(inputFile, types, enums));

        return 0;
    }
}
