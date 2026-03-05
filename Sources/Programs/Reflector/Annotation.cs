using System;

namespace Reflector
{
    enum Annotation
    {
        [StringValue("CHTYPE()")]
        Type,
        [StringValue("CHFIELD()")]
        Field,
        [StringValue("CHMETHOD()")]
        Method,
        [StringValue("CHENUM()")]
        Enum,
    }
}
