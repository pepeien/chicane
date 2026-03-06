namespace Reflector
{
    record FieldModel(string TypeName, string Name);
    record MethodModel(string ReturnType, string Name, List<string> ParamTypes);
    record EnumeratorModel(string Name, int Value);
    record TypeModel(string Kind, string Name, string Namespace, List<FieldModel> Fields, List<MethodModel> Methods);
    record EnumModel(string Name, string Namespace, List<EnumeratorModel> Enumerators);
}