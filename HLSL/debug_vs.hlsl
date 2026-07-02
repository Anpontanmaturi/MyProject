#include "debug.hlsli"

VS_OUT main(float4 position : POSITION)
{
    VS_OUT vout;
    vout.position = mul(position, world_view_projection);
    vout.color = color;
    
    return vout;
}