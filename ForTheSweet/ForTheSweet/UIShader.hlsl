#ifndef UI_SHADER_HLSL
#define UI_SHADER_HLSL


#include "Texture.hlsl"
#include "Sampler.hlsl"
#include "UICbuffer.hlsl"
//SamplerState gDefaultSamplerState : register(s0);


VS_TEXTURED_OUTPUT VSUITextured(uint nVertexID : SV_VertexID)
{
    VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT) 0.0f;

    float2 screenpos = (float2) 0.0f;
    screenpos.x = (gxmf2ScreenPos.x - gxmf2ScreenSize.x / 2.0f) / (gxmf2ScreenSize.x / 2.0f);
    screenpos.y = (gxmf2ScreenPos.y - gxmf2ScreenSize.y / 2.0f) / (gxmf2ScreenSize.y / 2.0f);

    float2 size = (float2) 0.0f;
    size.x = ((float) gnSize.x / gxmf2ScreenSize.x) * gfScale.x;
    size.y = ((float) gnSize.y / gxmf2ScreenSize.y) * gfScale.y;

    if (nVertexID == 0)
    {
        output.uv = (float2(0.0f, 0.0f)); // 스크린 왼쪽 위 
        output.position = float4(screenpos.x - size.x, screenpos.y + size.y, 0.0f, 1.0f);
    }
    if (nVertexID == 1)
    {
        output.uv = (float2(1.0f, 0.0f)); // 스크린 오른쪽 위
        output.position = float4(screenpos.x + size.x, screenpos.y + size.y, 0.0f, 1.0f);

    }
    if (nVertexID == 2)
    {
        output.uv = (float2(1.0f, 1.0f)); // 스크린 오른쪽 아래
        output.position = float4(screenpos.x + size.x, screenpos.y - size.y, 0.0f, 1.0f);

    }
    if (nVertexID == 3)
    {
        output.uv = (float2(0.0f, 0.0f)); // 스크린 왼쪽 위
        output.position = float4(screenpos.x - size.x, screenpos.y + size.y, 0.0f, 1.0f);

    }
    if (nVertexID == 4)
    {
        output.uv = (float2(1.0f, 1.0f)); // 스크린 왼쪽 아래
        output.position = float4(screenpos.x + size.x, screenpos.y - size.y, 0.0f, 1.0f);

    }
    if (nVertexID == 5)
    {
        output.uv = (float2(0.0f, 1.0f)); // 스크린 오른쪽 아래
        output.position = float4(screenpos.x - size.x, screenpos.y - size.y, 0.0f, 1.0f);
    }

    return output;
};

float4 PSDefaultUI(VS_TEXTURED_OUTPUT input) : SV_Target
{
    float4 finalColor = (float4) 0.0f;
    float2 uv = input.uv;
 
    uv = float2(
    input.uv.x / gnNumSprite.x + float(gnNowSprite.x) / float(gnNumSprite.x),
    input.uv.y / gnNumSprite.y + float(gnNowSprite.y) / float(gnNumSprite.y)
    );

    finalColor = gUITextures1.Sample(gDefaultSamplerState, uv);
    finalColor.a *= gfAlpha;

    return finalColor;
}

float4 PSUIHPBar(VS_TEXTURED_OUTPUT input) : SV_Target
{

    float4 finalColor = (float4) 0.0f;
    float2 uv = input.uv;
 
    if (gnTexType == 0) 
        finalColor = gUITextures1.Sample(gDefaultSamplerState, uv);
    else if (gnTexType == 1)
    {
       // if (uv.x < gfData)
            finalColor = gUITextures2.Sample(gDefaultSamplerState, uv);
    }
    else if (gnTexType == 2)
    {
       // if (uv.x < gfData)
            finalColor = gUITextures3.Sample(gDefaultSamplerState, uv);
    }
    else if (gnTexType == 3)
    {
       // if (uv.x < gfData)
        finalColor = gUITextures4.Sample(gDefaultSamplerState, uv);
    }
    else if (gnTexType == 4)
    {
       // if (uv.x < gfData)
        finalColor = gUITextures5.Sample(gDefaultSamplerState, uv);
    }
    finalColor.a *= gfAlpha;

    return finalColor;

}
#endif