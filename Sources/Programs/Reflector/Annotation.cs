using System;

namespace Reflector
{
    enum Annotation
    {
        [StringValue("CH_ENUM")]
        Enum,
        [StringValue("CH_TYPE")]
        Type,
        [StringValue("CH_CONSTRUCTOR")]
        Constructor,
        [StringValue("CH_FUNCTION")]
        Method,
        [StringValue("CH_FIELD")]
        Field,
        [StringValue("Undefined")]
        Undefined
    }

    enum AnnotationInclusion
    {
        [StringValue("Automatic")]
        Automatic,
        [StringValue("Manual")]
        Manual,
    }
}
