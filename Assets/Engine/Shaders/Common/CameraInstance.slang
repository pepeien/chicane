module CameraInstance;

public struct CameraInstance
{
    public float4 clip;
    public float4x4 view;
    public float4x4 projection;
    public float4 up;
    public float4 right;
    public float4 forward;
    public float4 translation;
}
