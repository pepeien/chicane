using System;
using System.Reflection;

namespace Reflector
{
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public class StringValueAttribute : Attribute
    {
        public string Value { get; }

        public StringValueAttribute(string value)
        {
            Value = value;
        }
    }

    public class Enum
    {
        public static string GetStringValue(System.Enum value)
        {
            FieldInfo fi = value.GetType().GetField(value.ToString());
            StringValueAttribute[] attributes = (StringValueAttribute[])fi.GetCustomAttributes(typeof(StringValueAttribute), false);

            return attributes.Length > 0 ? attributes[0].Value : value.ToString();
        }
    }
}
