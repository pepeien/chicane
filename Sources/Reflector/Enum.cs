using System;
using System.Reflection;

namespace Reflector
{
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public class StringValueAttribute(string value) : Attribute
    {
        public string Value { get; } = value;
    }

    public class Enum
    {
        public static string GetStringValue(System.Enum value)
        {
            FieldInfo? fieldInfo = value.GetType().GetField(value.ToString());
            if (fieldInfo is null)
            {
                return value.ToString();
            }

            StringValueAttribute[] attributes = (StringValueAttribute[])fieldInfo.GetCustomAttributes(typeof(StringValueAttribute), false);
            return attributes.Length > 0 ? attributes[0].Value : value.ToString();
        }
    }
}
