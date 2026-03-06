using System;

namespace Reflector
{
    enum Annotation
    {
        [StringValue("CH_TYPE()")]
        Type,
        [StringValue("CH_FIELD()")]
        Field,
        [StringValue("CH_FUNCTION()")]
        Method,
        [StringValue("CH_ENUM()")]
        Enum,
    }
}
