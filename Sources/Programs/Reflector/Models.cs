namespace Reflector
{
    record FieldModel(string TypeName, List<string> Names, bool IsPointer = false)
    {
        public string Name => Names.FirstOrDefault() ?? "";
    }
    record MethodModel(string ReturnType, string Name, List<string> ParamTypes);
    record EnumeratorModel(string Name, int Value);
    record TypeModel(string Kind, string Name, string Namespace, List<FieldModel> Fields, List<MethodModel> Methods, string BaseType = "")
    {
        public bool Resolved { get; set; } = false;
    }
    record EnumModel(string Name, string Namespace, List<EnumeratorModel> Enumerators);
}