namespace Reflector
{
    record EnumeratorModel(string Name, int Value);
    record EnumModel(string Name, List<EnumeratorModel> Enumerators);
    record ConstructorModel(List<string> ParamTypes);
    record PropertyModel(
        List<string> Names,
        string Group,
        string Description,
        bool IsTransient
    )
    {
        public string Name => Names.FirstOrDefault() ?? "";
    }
    record FunctionModel(
        PropertyModel Property,
        string ReturnType,
        List<string> ParamTypes,
        bool IsIterable,
        string ElementName,
        bool IsElementPointer,
        bool IsStatic
    )
    {
        public string Name => Property.Name;
    }
    record FieldModel(
        PropertyModel Property,
        string TypeName,
        bool IsPointer,
        bool IsIterable,
        string ElementName,
        bool IsElementPointer
    )
    {
        public string Name => Property.Name;
        public List<string> Names => Property.Names;
        public string Group => Property.Group;
        public string Description => Property.Description;
    }
    record TypeModel(
        string Kind,
        PropertyModel Property,
        List<ConstructorModel> Constructors,
        List<FunctionModel> Functions,
        List<FieldModel> Fields,
        List<string> Bases,
        List<FunctionModel> OwnFunctions,
        List<FieldModel> OwnFields
    )
    {
        public bool Resolved { get; set; } = false;

        public string Name => Property.Name;
        public List<string> Names => Property.Names;
        public string Group => Property.Group;
        public string Description => Property.Description;
    }
}
