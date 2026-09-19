module LightInstance;

import SceneLight;

public struct LightInstance
{
    public float4 splits;
    public float4 info;
    public SceneLight lights[8];
    public float4x4 views[4];
    public float4x4 projections[4];
}
