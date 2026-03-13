namespace Reflector
{
    record EnumeratorModel(string Name, int Value);
    record EnumModel(string Name, List<EnumeratorModel> Enumerators);
    record ConstructorModel(List<string> ParamTypes);
    record FunctionModel(string ReturnType, string Name, List<string> ParamTypes);
    record FieldModel(string TypeName, List<string> Names, bool IsPointer = false)
    {
        public string Name => Names.FirstOrDefault() ?? "";
    }
    record TypeModel(
        string Kind,
        string Name,
        List<ConstructorModel> Constructors,
        List<FunctionModel> Functions,
        List<FieldModel> Fields
    )
    {
        public bool Resolved { get; set; } = false;
    }
}