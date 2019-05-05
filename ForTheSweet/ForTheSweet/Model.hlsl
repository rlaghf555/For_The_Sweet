#define NUM_DIRECTION_LIGHTS 1

Texture2DArray gBoxTextured : register(t0);
SamplerState gDefaultSamplerState : register(s0);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView;
	matrix gmtxProjection;
	matrix gmtxInvProjection;
	float3 gvCameraPosition;
};

cbuffer cbObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint			gnMaterial : packoffset(c4);
}

cbuffer cbAnimateInfo : register(b3)
{
	matrix      gmtxObject;
	matrix      gBoneTransforms[96];
	uint        gnMat;
}

struct VS_MODEL_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tan : TANGENT;
	float2 uv : TEXCOORD;
	uint4 index : BORNINDEX;
	float3 weight : WEIGHT;
	uint texindex : TEXINDEX;
};

struct VS_MODEL_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 color : SV_TARGET0;
	float4 nrmoutline : SV_TARGET1;
	float4 nrm : SV_TARGET2;
	float4 pos : SV_TARGET3;
};

VS_MODEL_TEXTURED_OUTPUT VSDynamicModel(VS_MODEL_INPUT input)
{
	VS_MODEL_TEXTURED_OUTPUT output;

	float3   posL = float3(0.0f, 0.0f, 0.0f);
	float3   normalL = float3(0.0f, 0.0f, 0.0f);
	float3   tanL = float3(0.0f, 0.0f, 0.0f);
	float   weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	weights[0] = input.weight.x;
	weights[1] = input.weight.y;
	weights[2] = input.weight.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransforms[input.index[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransforms[input.index[i]]).xyz;
		tanL += weights[i] * mul(input.tan, (float3x3)gBoneTransforms[input.index[i]]).xyz;
	}

	// Right + Texture
	output.normalW = mul(normalL, (float3x3)gmtxObject);
	output.positionW = (float3)mul(float4(posL, 1.0f), gmtxObject);
	output.position = mul(mul(mul(float4(posL, 1.0f), gmtxObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSDynamicModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
	input.normalW = normalize(input.normalW);
	
	output.color = cColor;
	//output.color = float4(1, 1, 1, 1);
	output.nrmoutline = float4(input.normalW, 1.0f);
	output.nrm = output.nrmoutline;
	output.pos = float4(input.positionW, 1.0f);
	
	return (output);
};

VS_MODEL_TEXTURED_OUTPUT VSStaticModel(VS_MODEL_INPUT input)
{
	VS_MODEL_TEXTURED_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

	//for (int i = 0; i < NUM_DIRECTION_LIGHTS; i++) output.ShadowPosH[i] = mul(float4(output.positionW, 1.0f), gmtxShadowProjection[i]);
       
	return(output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSCottonModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSWaveModel(VS_MODEL_TEXTURED_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)   // nPrimitiveID : 삼각형의 정보 
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
    output.color.w = 0.2f;
	//output.color = float4(1, 1, 1, 1);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tan : TANGENT;
    float2 uv : TEXCOORD;
    uint texindex : TEXINDEX;
};
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};

VS_MODEL_TEXTURED_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_MODEL_TEXTURED_OUTPUT output;
    
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

/*
float4 PSDiffused(VS_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    	
    float3 uvw = float3(input.uv, nPrimitiveID / 2);
    float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	
    input.normalW = normalize(input.normalW);
	
    output.color = cColor;
	//output.color = float4(1, 1, 1, 1);
    output.nrmoutline = float4(input.normalW, 1.0f);
    output.nrm = output.nrmoutline;
    output.pos = float4(input.positionW, 1.0f);
	
    return (output);
}*/

/*
// nPrimitiveID : 삼각형의 정보 
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSStaticModel(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	//float4 cColor = gBoxTextured.Sample(gDefaultSamplerState, uvw);
	input.normalW = normalize(input.normalW);

	output.color = float4(0,1,0,1);
	output.nrmoutline = float4(input.normalW, 1.0f);
	output.nrm = output.nrmoutline;
	output.pos = float4(input.positionW, 1.f);
	return (output);
};
*/