#include "common.hlsli"

struct v2p
{
    float4 c0 : COLOR0; // color
};

// Pixel
float4 main(v2p I) : COLOR
{
    // out
    return I.c0;
}
