namespace Reflector
{
    record EnumeratorModel(string Name, int Value);
    record EnumModel(string Name, string Namespace, List<EnumeratorModel> Enumerators);
    record ConstructorModel(List<string> ParamTypes);
    record MethodModel(string ReturnType, string Name, List<string> ParamTypes);
    record FieldModel(string TypeName, List<string> Names, bool IsPointer = false)
    {
        public string Name => Names.FirstOrDefault() ?? "";
    }
    record TypeModel(
        string Kind,
        string Name,
        string Namespace,
        List<ConstructorModel> Constructors,
        List<MethodModel> Methods,
        List<FieldModel> Fields,
        string BaseType = ""
    )
    {
        public bool Resolved { get; set; } = false;
    }
}