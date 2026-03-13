using ClangSharp.Interop;

namespace Reflector
{
    static class Parser
    {
        static unsafe string FindAnnotation(CXCursor cursor, Annotation annotation)
        {
            string found = "";
            string prefix = Enum.GetStringValue(annotation);

            cursor.VisitChildren(
                (child, _, _) =>
                {
                    if (child.Kind == CXCursorKind.CXCursor_AnnotateAttr && child.Spelling.CString.StartsWith(prefix))
                    {
                        found = child.Spelling.CString;

                        return CXChildVisitResult.CXChildVisit_Break;
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;
                },
                default
            );

            return found;
        }

        static unsafe bool HasAnnotation(CXCursor cursor, Annotation annotation)
        {
            return !string.IsNullOrWhiteSpace(FindAnnotation(cursor, annotation));
        }

        static string GetAnnotationParam(string annotationValue)
        {
            if (string.IsNullOrWhiteSpace(annotationValue))
            {
                return "";
            }

            var idx = annotationValue.IndexOf(':');

            return idx >= 0 ? annotationValue[(idx + 1)..].Trim() : "";
        }

        static string GetTypeName(CXType type)
        {
            return type.CanonicalType.Spelling.CString.Replace("const ", "")
                  .Replace("&", "")
                  .Trim();
        }

        public static (List<TypeModel>, List<EnumModel>) Parse(string filePath, List<string> lookUpFolders)
        {
            var types = new List<TypeModel>();
            var enums = new List<EnumModel>();

            var argList = new List<string>
            {
                "-std=c++17",
                "-xc++",
                "-fms-extensions",
                "-fparse-all-comments",
                "-DCH_TYPE(...)=__attribute__((annotate(\"CH_TYPE:\" #__VA_ARGS__)))",
                "-DCH_ENUM(...)=__attribute__((annotate(\"CH_ENUM:\" #__VA_ARGS__)))",
                "-DCH_FIELD(...)=__attribute__((annotate(\"CH_FIELD:\" #__VA_ARGS__)))",
                "-DCH_FUNCTION(...)=__attribute__((annotate(\"CH_FUNCTION:\" #__VA_ARGS__)))",
                "-DCH_CONSTRUCTOR(...)=__attribute__((annotate(\"CH_CONSTRUCTOR:\" #__VA_ARGS__)))"
            };

            foreach (var dir in lookUpFolders)
            {
                argList.Add($"-I{dir}");
            }

            var resourceDir = GetClangResourceDir();
            if (resourceDir is not null)
            {
                argList.Add("-resource-dir");
                argList.Add(resourceDir);
            }

            using var index = CXIndex.Create(false, false);

            CXTranslationUnit.TryParse(
                index,
                filePath,
                argList.ToArray(),
                [],
                CXTranslationUnit_Flags.CXTranslationUnit_DetailedPreprocessingRecord |
                CXTranslationUnit_Flags.CXTranslationUnit_SkipFunctionBodies |
                CXTranslationUnit_Flags.CXTranslationUnit_KeepGoing,
                out var tu
            );

            using (tu)
            {
                VisitChildren(tu.Cursor, types, enums, filePath, Annotation.Undefined, []);
            }

            return (types, enums);
        }

        static string? GetClangResourceDir()
        {
            try
            {
                var psi = new System.Diagnostics.ProcessStartInfo("clang", "-print-resource-dir")
                {
                    RedirectStandardOutput = true,
                    UseShellExecute = false
                };

                using var proc = System.Diagnostics.Process.Start(psi)!;
                var dir = proc.StandardOutput.ReadToEnd().Trim();
                proc.WaitForExit();
                return string.IsNullOrEmpty(dir) ? null : dir;
            }
            catch
            {
                return null;
            }
        }

        static unsafe List<string> GetMacroArgs(CXCursor cursor)
        {
            var tu = cursor.TranslationUnit;
            var extent = cursor.Extent;
            var tokens = tu.Tokenize(extent);

            bool open = false;
            foreach (var token in tokens)
            {
                var text = clang.getTokenSpelling(tu, token).CString;

                if (text == "(")
                {
                    open = true;

                    continue;
                }

                if (text == ")")
                {
                    break;
                }

                if (open)
                {
                    return new List<string>(text.Trim().Split(','));
                }
            }

            return [];
        }

        static unsafe void CollectMembers(
            CXCursor cursor,
            List<string> args,
            List<FunctionModel> methods,
            List<FieldModel> fields
        )
        {
            bool isAutomatic = args.Contains(Enum.GetStringValue(AnnotationInclusion.Automatic));
            bool isPublic = cursor.Kind == CXCursorKind.CXCursor_StructDecl ||
                            cursor.Kind == CXCursorKind.CXCursor_UnionDecl;

            cursor.VisitChildren(
                (child, _, _) =>
                {
                    switch (child.Kind)
                    {
                        case CXCursorKind.CXCursor_CXXBaseSpecifier:
                            var baseDecl = clang.getTypeDeclaration(child.Type);

                            if (baseDecl.Kind != CXCursorKind.CXCursor_NoDeclFound && baseDecl.IsDefinition)
                            {
                                CollectMembers(baseDecl, args, methods, fields);
                            }

                            break;

                        case CXCursorKind.CXCursor_CXXAccessSpecifier:
                            isPublic = child.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic;

                            break;

                        case CXCursorKind.CXCursor_CXXMethod:
                            if (HasAnnotation(child, Annotation.Function))
                            {
                                methods.Add(ParseMethod(child));
                            }

                            break;

                        case CXCursorKind.CXCursor_FieldDecl:
                            if (HasAnnotation(child, Annotation.Field) || (isAutomatic && isPublic))
                            {
                                fields.Add(ParseField(child));
                            }

                            break;

                        case CXCursorKind.CXCursor_UnionDecl:
                            if (isAutomatic && isPublic)
                            {
                                fields.Add(ParseField(child));
                            }

                            break;

                        default:
                            break;
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;
                },
                default
            );
        }

        static unsafe void VisitChildren(
            CXCursor parent,
            List<TypeModel> types,
            List<EnumModel> enums,
            string fullPath,
            Annotation pending,
            List<string> pendingParams
        )
        {
            Annotation next = pending;
            List<string> nextParams = pendingParams;

            parent.VisitChildren((cursor, _, _) =>
            {
                if (!IsDefinedInFile(cursor, fullPath))
                {
                    return CXChildVisitResult.CXChildVisit_Continue;
                }

                switch (cursor.Kind)
                {
                    case CXCursorKind.CXCursor_Namespace:
                        VisitChildren(cursor, types, enums, fullPath, next, nextParams);

                        next = Annotation.Undefined;
                        nextParams = [];

                        break;

                    case CXCursorKind.CXCursor_MacroExpansion:
                        var spelling = cursor.Spelling.CString;

                        if (spelling.StartsWith(Enum.GetStringValue(Annotation.Type)))
                        {
                            next = Annotation.Type;
                            nextParams = GetMacroArgs(cursor);
                        }
                        else if (spelling.StartsWith(Enum.GetStringValue(Annotation.Enum)))
                        {
                            next = Annotation.Enum;
                            nextParams = GetMacroArgs(cursor);
                        }

                        break;

                    case CXCursorKind.CXCursor_StructDecl:
                    case CXCursorKind.CXCursor_ClassDecl:
                    case CXCursorKind.CXCursor_UnionDecl:
                        if (!cursor.IsDefinition)
                        {
                            break;
                        }

                        if (next == Annotation.Type)
                        {
                            types.Add(ParseType(cursor, nextParams));
                        }

                        next = Annotation.Undefined;
                        nextParams = [];

                        break;

                    case CXCursorKind.CXCursor_EnumDecl:
                        if (!cursor.IsDefinition)
                        {
                            break;
                        }

                        if (next == Annotation.Enum)
                        {
                            enums.Add(ParseEnum(cursor, nextParams));
                        }

                        next = Annotation.Undefined;
                        nextParams = [];

                        break;

                    default:
                        break;
                }

                return CXChildVisitResult.CXChildVisit_Continue;
            }, default);
        }

        static unsafe EnumModel ParseEnum(CXCursor cursor, List<string> args)
        {
            var enumerators = new List<EnumeratorModel>();

            cursor.VisitChildren((child, _, _) =>
            {
                if (child.Kind == CXCursorKind.CXCursor_EnumConstantDecl)
                    enumerators.Add(
                        new(
                            child.Spelling.CString,
                            (int)child.EnumConstantDeclValue
                        )
                    );

                return CXChildVisitResult.CXChildVisit_Continue;
            }, default);

            string ns = GetNamespace(cursor);
            return new((string.IsNullOrWhiteSpace(ns) ? "" : $"{ns}::") + cursor.Spelling.CString, enumerators);
        }

        static unsafe TypeModel ParseType(CXCursor cursor, List<string> args)
        {
            var name = cursor.Spelling.CString;
            var ns = GetNamespace(cursor);
            var kind = cursor.Kind == CXCursorKind.CXCursor_StructDecl ? "struct" : "class";

            var constructors = new List<ConstructorModel>();
            var methods = new List<FunctionModel>();
            var fields = new List<FieldModel>();

            bool isAutomatic = args.Contains(Enum.GetStringValue(AnnotationInclusion.Automatic));
            bool isPublic = kind == "struct" || kind == "union";

            cursor.VisitChildren(
                (child, _, _) =>
                {
                    switch (child.Kind)
                    {
                        case CXCursorKind.CXCursor_Constructor:
                            if (HasAnnotation(child, Annotation.Constructor))
                            {
                                constructors.Add(ParseConstructor(child));
                            }

                            break;

                        case CXCursorKind.CXCursor_CXXMethod:
                            if (HasAnnotation(child, Annotation.Function))
                            {
                                methods.Add(ParseMethod(child));
                            }

                            break;

                        case CXCursorKind.CXCursor_UnionDecl:
                        case CXCursorKind.CXCursor_FieldDecl:
                            if (HasAnnotation(child, Annotation.Field) || (isAutomatic && isPublic))
                            {
                                fields.Add(ParseField(child));
                            }

                            break;

                        case CXCursorKind.CXCursor_CXXBaseSpecifier:
                            var baseDecl = clang.getTypeDeclaration(child.Type);

                            if (baseDecl.Kind != CXCursorKind.CXCursor_NoDeclFound && baseDecl.IsDefinition)
                            {
                                CollectMembers(baseDecl, args, methods, fields);
                            }

                            break;

                        case CXCursorKind.CXCursor_CXXAccessSpecifier:
                            isPublic = child.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic;

                            break;

                        default:
                            break;
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;
                },
                default
            );

            return new(kind, (string.IsNullOrWhiteSpace(ns) ? "" : $"{ns}::") + name, constructors, methods, fields);
        }

        static unsafe ConstructorModel ParseConstructor(CXCursor cursor)
        {
            List<string> paramTypes = [];

            cursor.VisitChildren((child, _, _) =>
            {
                if (child.Kind == CXCursorKind.CXCursor_ParmDecl)
                {
                    paramTypes.Add(GetTypeName(child.Type));
                }

                return CXChildVisitResult.CXChildVisit_Continue;
            }, default);

            return new(paramTypes);
        }

        static unsafe FunctionModel ParseMethod(CXCursor cursor)
        {
            List<string> paramTypes = [];

            cursor.VisitChildren((child, _, _) =>
            {
                if (child.Kind == CXCursorKind.CXCursor_ParmDecl)
                {
                    paramTypes.Add(GetTypeName(child.Type));
                }

                return CXChildVisitResult.CXChildVisit_Continue;
            }, default);

            return new(
                GetTypeName(cursor.ResultType),
                cursor.Spelling.CString,
                paramTypes
            );
        }

        static unsafe FieldModel ParseField(CXCursor cursor)
        {
            List<string> names = [];
            var type = cursor.Type.CanonicalType;

            if (cursor.Kind == CXCursorKind.CXCursor_UnionDecl)
            {
                cursor.VisitChildren((child, _, _) =>
                {
                    if (child.Kind == CXCursorKind.CXCursor_FieldDecl)
                    {
                        type = child.Type.CanonicalType;

                        names.Add(child.Spelling.CString);
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;
                }, default);
            }
            else
            {
                names.Add(cursor.Spelling.CString);
            }

            bool isPointer = type.kind == CXTypeKind.CXType_Pointer;

            return new(
                GetTypeName(isPointer ? type.PointeeType : type),
                names,
                isPointer
            );
        }

        static string GetNamespace(CXCursor cursor)
        {
            var parts = new List<string>();
            var parent = cursor.SemanticParent;

            while (parent.Kind == CXCursorKind.CXCursor_Namespace)
            {
                parts.Insert(0, parent.Spelling.CString);

                parent = parent.SemanticParent;
            }

            return string.Join("::", parts);
        }

        static bool IsDefinedInFile(CXCursor cursor, string fullPath)
        {
            cursor.Location.GetFileLocation(out var cxFile, out _, out _, out _);
            var name = cxFile.Name.CString;
            return !string.IsNullOrEmpty(name) &&
                    string.Equals(
                       Path.GetFullPath(name),
                       fullPath,
                       StringComparison.OrdinalIgnoreCase
                    );
        }
    }
}