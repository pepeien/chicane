module Poly3D;

public static const int POLY_FLAG_LIT        = 1;
public static const int POLY_FLAG_SHADOW     = 2;
public static const int POLY_FLAG_FOREGROUND = 4;
public static const int POLY_FLAG_OUTLINED   = 8;

public struct PolyInstance3D
{
    public float4x4 model;
    public int4 textures0;
    public int4 textures1;
    public int4 textures2;
    public int flags;
    public float emissiveStrength;
    public float tileSize;
    public int _pad2;
}
