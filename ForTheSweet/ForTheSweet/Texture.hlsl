
#define NUM_MAX_UITEXTURE 4

Texture2D gUITextures1 : register(t9);
Texture2D gUITextures2 : register(t10);
Texture2D gUITextures3 : register(t11);
Texture2D gUITextures4 : register(t12);


struct VS_TEXTURED_INPUT
{
    float3 position : POSITION; // 
    float2 uv : TEXCOORD; // 
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
